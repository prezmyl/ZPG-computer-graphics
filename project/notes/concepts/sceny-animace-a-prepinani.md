# Scény, animační komponenty a přepínání – DU02

**Stav podle GitHubu:** 2026-10-27, commit `a421f094bd8fe2b8f810919a4bcf59689de91d2f`.

## Proč samostatné instance `Scene`, ne nutně dědičnost?

Pokud se scény liší obsahem (objekty) a připojeným chováním (animace), stačí jedna konkrétní `Scene` s kolekcemi `DrawableObject` a `Animation`. Nepotřebujeme podtřídu pro každý les či planetární systém ani `switch(sceneType)` uvnitř `Scene`. Specializované typy scén by měly smysl až při potřebě výrazně odlišného veřejného rozhraní nebo vlastní koordinace životního cyklu. Kompozice animačních komponent zde připomíná Strategy, ale nejde o nutnost doslovně implementovat tento vzor.

## Kdo co vlastní a kdo pouze používá?

```text
Application
  ├── vector<unique_ptr<Scene>> scenes
  │    ├── Scene (forest)
  │    │    ├── vector<unique_ptr<DrawableObject>> objects
  │    │    │      └── unique_ptr<Transformation> → Composite → Rotation
  │    │    └── vector<unique_ptr<Animation>> animations
  │    │           └── RotationAnimation → Rotation* [nevlastnící]
  │    └── Scene (login) [zatím prázdná]
  ├── Model pro tree a bushes
  └── ShaderProgram
```

`Rotation` vlastní kompozit; `RotationAnimation` ji pouze aktualizuje prostřednictvím nevlastnícího ukazatele. `Scene` vlastní obojí, ale nepleteme si společného nadřazeného vlastníka s vlastnictvím mezi sourozeneckými objekty.

Členské `objects` je deklarované **před** `animations`. Při běžném zániku `Scene` jsou členy zničeny opačně, tedy animace dříve než objekty. Při odebrání jednotlivého `DrawableObject` během života `Scene` je naopak potřeba samostatně odregistrovat příslušnou animaci, aby nezůstal dangling pointer.

## Aktualizace a vykreslování

```text
Application::run()
    Scene* selected = scenes[activeSceneIdx].get()
    selected->update(time)
        Animation::update(time) [virtuální]
            RotationAnimation::update(time)
                Rotation::setAngle(angularSpeed * time)
    selected->draw()
        DrawableObject::draw()
            transformation->getMatrix()
            ShaderProgram::setMat4("modelMatrix", M)
            Model::draw()
```

Čas je dodán koordinátorem, `RotationAnimation` ví, jak úhel vypočítat, `Rotation` zná matici a `DrawableObject` zná přenos k shaderu. `Scene` nemusí znát typ konkrétní animace. Absolutní čas znamená, že po přepnutí na delší dobu neaktivní scénu a zpět se stav spočítá pro **globální** čas, tedy animace během neaktivity nepřímo pokračuje. Pokud bychom chtěli pauzu, potřebujeme jinou politiku měření času.

## Změna vlastnictví není změna identity objektu

```cpp
auto sceneForest = std::make_unique<Scene>();
// vytvořit všechny objekty a animace scény
addScene(std::move(sceneForest)); // po přesunu už sceneForest nevlastní Scene
```

Po `std::move(sceneForest)` původní `unique_ptr` už objekt nevlastní; použití `sceneForest->addObject()` by bylo chybné. Pokud si před přesunem uložíme `Scene* alias = sceneForest.get()`, adresa objektu zůstává platná přes samotný move, ale jen dokud nový vlastník scénu drží. Pro tuto fázi je jednoduché a bezpečné nejprve scénu naplnit a až pak převést vlastnictví.

## Přepínání aktivní scény

`Application` vlastní `vector<unique_ptr<Scene>>` a přidává `activeSceneIdx` jako **index**, nikoli dalšího vlastníka. GLFW callback získá `Application*` přes dvojici `glfwSetWindowUserPointer(window, this)` a `glfwGetWindowUserPointer(window)`. Knihovna si pouze uchovává adresu, aplikaci nevlastní.

Veřejný `setActiveScene(idx)` má chránit invariant: `idx < scenes.size()`. **Současný commit tuto kontrolu neplní:**

```cpp
if (activeSceneIdx < scenes.size()) {
    activeSceneIdx = idx; // ověřen starý index, ne nově požadovaný
}
```

Pravidlo: při setru nejprve validuj **nový vstup**. Kontrola současné platné hodnoty nic neříká o platnosti požadavku.

## Kdy zvážit další abstrakci?

Dokud máme několik scén s jednoduchými animacemi, stačí `Scene`, `Animation` a `Application`. `SceneManager`, hierarchie `ForestScene`, `LoginScene` apod. nebo pokročilý scheduler nejsou samy o sobě cílem. Novou abstrahovanou odpovědnost přidej až s konkrétní potřebou (např. pause/resume, čas nezávislý na době běhu, výměna scén za běhu, nezávislé kamery/osvětlení).
