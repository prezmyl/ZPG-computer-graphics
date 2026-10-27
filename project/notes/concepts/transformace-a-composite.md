# Transformace a Composite v DU02 – koncepty a pracovní architektura

Tato poznámka shrnuje aktuálně používanou transformační hierarchii, její propojení s vykreslováním i první animaci. Navazuje na checkpointy z 2026-10-13 a 2026-10-15; stav dynamické transformace odpovídá ověřenému code commitu `da67ac4` z 2026-10-27.

## Odpovědnosti a rozhraní

- `Transformation`: abstraktní kontrakt `glm::mat4 getMatrix() const` a virtuální destruktor; nezávisí na GLFW, OpenGL kontextu, `Model` ani `DrawableObject`.
- `Translation`: uchovává vektor posunu `glm::vec3`; vrací matici posunutí.
- `Rotation`: uchovává osu `glm::vec3` a `float angle` **v radiánech**; poskytuje `setAngle` a matici rotace. Čas a rychlost animace do této třídy nyní nepatří.
- `Scale`: uchovává tři faktory měřítka v `glm::vec3`.
- `CompositeTransformation`: uchovává **uspořádanou** kolekci polymorfních transformací v `vector<unique_ptr<Transformation>>`; jediným algoritmem skládá výslednou matici, bez dotazů na konkrétní potomky.

Praktické rozdělení: **elementární transformace zná, jak se tvoří její matice; Composite zná, jak se skládají výsledky; vykreslovací vrstva bude znát, jak výslednou matici použít na GPU.**

## Hodnota parametru vs. odvozená matice

`glm::vec3(2,3,0)` je parametr posunu, nikoli hotová matice a nikoli již posunutý bod. `glm::translate(glm::mat4(1.0f), offset)` konstruuje novou matici `glm::mat4`. Teprve `matrix * vertex` aktualizuje souřadnice bodu.

Ukládání parametrů místo `mat4` je současná volba návrhu, ne jediná možná implementace (předpočítaná matice může dávat smysl při jiných požadavcích). Je důležité zachovat jeden srozumitelný zdroj pravdy o aktuálním stavu.

Jednotková matice `I` je neutrální prvek: `I*T = T*I = T`. Pro prázdný Composite má `getMatrix()` vrátit `I`.

## Pořadí násobení je součást kontraktu

GLM zde používáme se **sloupcovými vektory**: `p' = M * p`. Pravý faktor se na bod aplikuje jako první. Násobení matic je asociativní, nikoli obecně komutativní:

- `(A*B)*C = A*(B*C)`
- `R*T != T*R` v obecném případě.

Pokud pořadí přidání do Composite je `Translation`, `Rotation`, `Scale` a chceme aplikovat právě v tomto pořadí, pak `M = S * R * T`.

Současná implementace:

```cpp
glm::mat4 result(1.0f);
for (const auto& transformation : transformations) {
    result = transformation->getMatrix() * result;
}
return result;
```

Přidávaná matice je vlevo, aby operace působily na bod v pořadí uložení. Díky virtuálnímu `getMatrix()` může být položkou i další Composite – bez větve typu `if (isComposite)`.

Příklad transformace kolem jiného středu: `T(center) * R(angle) * T(-center)`. Pozor na rozdíl mezi pořadím **aplikace na bod** a zápisem výsledného součinu.

## Vlastnictví, vazby a dynamický stav

`CompositeTransformation::addTransformation(std::unique_ptr<Transformation>)` přebírá vlastnictví části (`std::move` při vložení). Abstraktní základ může být pointee typem; faktická instance je `Translation`, `Rotation`, `Scale` nebo další Composite. Virtuální destruktor zajišťuje korektní polymorfní zánik.

V aktuálním renderu vlastní `Scene` kolekci `DrawableObject`; každý `DrawableObject` vlastní jednu polymorfní `Transformation` a nevlastnicky používá `Model*` a `ShaderProgram*`. `CompositeTransformation` pak vlastní své části přes `unique_ptr`. Pro první animaci si `Application` uchovává pouze nevlastnící `Rotation*` z `rotation.get()` před `std::move(rotation)`. Tento pointer je platný jen po dobu života konkrétní rotace; `shared_ptr` není potřeba jen kvůli aktualizaci.

Nesdílet automaticky mutable rotace mezi nezávisle animovanými objekty. Stejné parametry nevyžadují stejnou instanci; sdílení geometrického `Model` a sdílení aktuálního transformačního stavu mají jiný význam.

## GLM a C++ – praktický kontrakt

- `glm::vec3`: trojice souřadnic/faktorů; `glm::mat4(1.0f)`: jednotková matice 4×4.
- `glm::translate`, `glm::rotate` a `glm::scale` jsou v `<glm/gtc/matrix_transform.hpp>` (nebo odpovídající GLM header variantě).
- `glm::rotate` očekává úhel v radiánech; převod ze stupňů: `glm::radians(degrees)`. Osa rotace má být nenulová a normalizovaná.
- V GLM `matrix[column][row]` (index sloupec–řádek).
- `getMatrix() const override` musí přesně odpovídat základní virtuální metodě; `override` nechává kompilátor kontrakt ověřit.
- `std::unique_ptr` se nekopíruje; vlastnictví se přesouvá. `std::vector` může držet více takových pointerů.
- Pro úplnost hlaviček deklaruj jejich přímo používané typy explicitními includes a v `.cpp` zahrnuj závislosti funkcí, které voláš.

## Dynamická rotace a znovuvyžádání odvozeného stavu (2026-10-27)

Při konstrukci první animované instance stromu se vytvoří `Rotation(Y, 0)`, získá se nevlastnící `Rotation*` přes `rotation.get()` a vlastnictví rotace se přes `std::move` přenese do Composite. V `Application::run()` se před vykreslením volá `animatedRotation->setAngle(glfwGetTime())`; tím se mění *parametr* rotace, nikoli přímo uniform v GPU. Rychlost odpovídá 1 rad/s.

Datový tok aktuálního snímku:

```text
Application::run()      -> setAngle(time)
Scene::draw()           -> DrawableObject::draw()
Transformation::getMatrix() -> Composite -> Rotation::getMatrix()
ShaderProgram::setMat4("modelMatrix", M) -> glUniformMatrix4fv
Model::draw()           -> glDrawArrays
```

`DrawableObject::draw()` žádá matici při každém volání, proto není nutné znovu volat `setTransformation()`. Stav rotace zůstává jediným zdrojem pravdy; výsledná matice se přepočítává.

Pro lokální rotaci objektu na jeho místě se vloží `Scale → Rotation → Translation`, výsledkem je `M = T * R * S`. Vložení `Translation` před `Rotation` by dalo `R * T * S`, tedy jiný pohyb kolem světového počátku.

**Bezpečnost nevlastnícího přístupu:** `unique_ptr` je vlastník; raw pointer může na jeho pointee dál ukazovat i po přesunu `unique_ptr`, ale zánik vlastníka pointer zneplatní. `if (ptr)` nezaručuje, že objekt stále existuje.

**Hranice současného řešení:** Aktualizace konkrétní rotace je přímo v `Application`, což je jednoduchý funkční experiment, nikoli cílový návrh pro více scén. Před DU03 bude vhodné umístit chování animace do příslušné scény/aktualizační komponenty a zachovat, aby `Rotation` počítala pouze matici.

## Co zatím není hotové / není ověřené

Render s `modelMatrix` a jedna animovaná rotace v lese jsou funkčně potvrzené uživatelem. Stále nejsou samostatné CPU testy složených matic, cílová architektura aktualizace scén, rotující login a hierarchická sluneční soustava. Asistent dosud nevykonal nezávislý lokální build.
