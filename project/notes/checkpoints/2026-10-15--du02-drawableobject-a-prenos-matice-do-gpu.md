# Learning checkpoint – DU02: DrawableObject a přenos transformační matice do GPU

**Datum:** 2026-10-15  
**Etapa:** propojení dříve vytvořené transformační hierarchie s OpenGL, uniformy a první instancí `DrawableObject`  
**Předchozí checkpoint:** `2026-10-15--du02-transformace-a-composite-implementace.md` (samostatná transformační hierarchie)  
**Poslední ověřený code commit:** `af678a0cdbc29958bb7404c2af7fb8a8615dd012`  
**Ověření:** statická kontrola aktuálního kódu a Git diffů; student v chatu potvrdil, že aplikace po opravě funguje. Asistent samostatně build ani běh neprovedl.

## Stav řešení po etapě

Funkční test přes `Translation(0.25f, 0.0f, 0.0f)` dříve prokázal, že se čtverec na obrazovce posune doprava. Po zapojení nové třídy `DrawableObject` student potvrdil, že aplikace opět funguje. Aktuální render loop volá `drawableObject->draw()`.

Aktuální datový a řídicí tok:

1. `Application::createResources()` vytvoří `Model` (GPU geometrie), `ShaderProgram` a `Translation`.
2. `Application` vlastní `Model`, `ShaderProgram` a `DrawableObject` přes `std::unique_ptr`.
3. `DrawableObject` **vlastní** `std::unique_ptr<Transformation>`; `Model*` a `ShaderProgram*` drží **nevlastnicky**.
4. `DrawableObject::draw()` volá `shaderProgram->use()`, získá `transformation->getMatrix()`, předá matici `shaderProgram->setMat4("modelMatrix", ...)` a nakonec `model->draw()`.
5. `ShaderProgram::setMat4()` používá `glGetUniformLocation`, `glm::value_ptr` a `glUniformMatrix4fv`; při lokaci `-1` vypíše hlášení.
6. Vertex shader má `uniform mat4 modelMatrix;` a počítá `gl_Position = modelMatrix * vec4(position, 1.0);`.
7. `Application::~Application()` explicitně ruší `drawableObject`, `shaderProgram` a `model` před zničením GLFW okna / OpenGL contextu.

Stále jde o **jediný vykreslovaný čtverec s jednoduchou translací**. `CompositeTransformation`, vnořené transformace, více instancí, `Scene`, pohledová/projekční matice ani animovaná rotace nejsou v render pipeline zatím vyzkoušeny.

## Samostatně prokázáno

- Student **sám zapsal a pushnul** konkrétní implementace `ShaderProgram::setMat4()`, změny vertex shaderu a postupně `DrawableObject.h/.cpp`; to dokládá schopnost uvedené API a rozhraní použít. Jejich návrh a seznam funkcí však byly předem poskytnuty s vedením, proto samotný commit **není důkaz nezávislého odvození**.
- V první verzi `DrawableObject` zvolil **nevlastnící `Model*` a `ShaderProgram*`**, což odpovídá předchozímu návrhu a jeho životnímu cyklu; šlo o vlastní konkretizaci v kódu po explicitní otázce, ale význam byl už probírán.
- Student postupně promítl opravy do reálného projektu místo vytvoření izolovaného příkladu: odstranil zbytečné gettery VAO/VBO, přesměroval vykreslení do `Model::draw()` a přesunul transformaci do polymorfního `unique_ptr`. Tyto opravy však byly **navedené rozborem**.
- Student oznámil a ověřil, že **Reload CMake odstranil problém se starou verzí shaderu**. Diagnostická hypotéza a příkaz byly dodány asistentem; samotné řešení proto nelze vydávat za studentem samostatně odhalené.
- Závěrečný běhový stav po vytvoření `DrawableObject` student v chatu výslovně potvrdil („ok funguje to“). Neznamená to ověření všech transformací, obecného lifecycle ani nezávislého testování.

## Pochopeno s vedením

- **Uniformy CPU → GPU:** při neznalosti API se student výslovně zeptal na vysvětlení funkcí. Asistent vysvětlil rozdíl `glGetUniformLocation` (lokace v programu), `glm::value_ptr` (ukazatel na 16 čísel) a `glUniformMatrix4fv` (nahrání hodnoty do aktivního programu). Student pak dodal správný kód.
- **Sémantika uniformu vs. hodnota pro konkrétní instanci:** `ShaderProgram` zná techniku nahrání uniformu podle jména a programu; `DrawableObject` ví, jakou aktuální `modelMatrix` poslat.
- **Refaktor podle odpovědností:** první `DrawableObject` uchovával `vector<unique_ptr<Transformation>>` a sám volal `glBindVertexArray/glDrawArrays`. S vedením byl odhalen překryv s `CompositeTransformation` a `Model`. Oprava na `unique_ptr<Transformation>` a `model->draw()` byla následně realizována.
- **Ownership transfer:** v `Application` byl nejprve neplatný konstrukční výraz s předáním `unique_ptr` namísto raw pointerů a nesprávně umístěným `std::move`. Po vysvětlení student opravil na `model.get()`, `shaderProgram.get()` a `std::move(move)`. Přechod `unique_ptr<Translation> → unique_ptr<Transformation>` je v kódu využit, jeho slovní zdůvodnění ještě nebylo samostatně předvedeno.
- **Správný pořádek destrukce:** asistent upozornil na potřebu zničit `DrawableObject` před objekty, na které nevlastnicky odkazuje, a všechny OpenGL resources před kontextem. Student implementoval `reset()` ve správném pořadí.

## Vývoj mentálního modelu a slepé cesty

1. **Výchozí stav:** funkční `Model` a `ShaderProgram` kreslí čtverec; nová `Transformation` zatím existovala pouze jako CPU matice, nebyla napojena na GPU.
2. **Neznámé API:** student nevěděl, jak implementovat `setMat4` ani co OpenGL funkce potřebují; po rozebrání jednotlivých kroků doplnil uniform do `basic.vert` a přenos matice v C++.
3. **Zavádějící symptom:** vykreslený čtverec se nepohnul, přestože `Translation::getMatrix()` i volání `setMat4` byly správné. V CMake bylo `file(COPY ...)`, které probíhá při konfiguraci. **Reload CMake** obnovil zkopírovaný shader. Poučení: zdrojová verze externího souboru nemusí být verze používaná během běhu.
4. **První `DrawableObject`:** třída převzala jak správu celé kolekce transformací (duplicitně k Composite), tak nízkoúrovňové OpenGL vykreslování (duplicitně k Model). Po upozornění odstranila oba přesahy.
5. **Předávání objektů:** potřeba rozlišit získání nevlastnící adresy (`.get()`) od přesunu vlastnictví (`std::move`) se projevila přímo v nekompilujícím konstruktoru; opraveno s vedením.
6. **Závěrečný stav:** koordinace vykreslení je v `DrawableObject::draw()` a `Application::run()` pouze deleguje. Student potvrdil funkčnost tohoto konkrétního zapojení.

### Zbývající chybný výrok v kódu – důležitý k prověření

Současný komentář v `Application::~Application()` zní přibližně **„drawable first, since it owns shaderPorgram and model“**. To je v rozporu s aktuálními typy: `DrawableObject` **nevlastní** `Model` ani `ShaderProgram`; má jen nevlastnící ukazatele. Správný **výsledek** (destroy DrawableObject first) neprokazuje správné **zdůvodnění** (vlastnictví). Bez další studentovy odpovědi je tento díl mentálního modelu **potřeba upevnit**, nikoli označit za vyřešený. Při checkpointu zdrojový komentář nebyl změněn.

## Generalizovatelné poučení / nejvyšší abstrakce

**Aplikační objekt má držet významový stav, adaptérová vrstva překládá význam do API.** `DrawableObject` zná aktuální transformaci konkrétní instance a koordinuje operace; `ShaderProgram` zná mechanismus nahrání typu `mat4`, nikoli význam každé modelové matice; `Model` zná svůj VAO, draw call a počet vertexů.

**Prostředek, odkaz a právo zničit jsou odlišné věci.** `model.get()` zpřístupní adresu bez předání vlastnictví, kdežto `std::move(move)` převádí vlastnictví `Translation` na `DrawableObject`. Zánik závislého objektu před poskytovatelem je pravidlo životnosti, nikoli důkaz, že závislý objekt poskytovatele vlastnil.

**Runtime artefakt nemusí odpovídat nejnovějšímu zdrojovému souboru.** Při ladění odděluj překlad C++, konfiguraci CMake, kopírování dat/shaderů a načtení konkrétního souboru v běhu. Úspěšný build sám o sobě neříká, který externí shader se skutečně použil.

## Potřebuje upevnit / zatím nevyhodnoceno

- Samostatně vysvětlit rozdíl mezi `.get()`, `std::move`, vlastnictvím, nevlastnící referencí a životností; **zejména opravit zdůvodnění v komentáři destruktoru**.
- Samostatně popsat přenos `glm::mat4` z CPU přes OpenGL uniform do GLSL, rozdíl `location` vs. hodnoty, vazbu uniformu na aktivní program a `GL_FALSE`; implementace je správná, ale vznikla po explicitní nápovědě.
- Test `CompositeTransformation`, různá pořadí, vnořené kompozity, časově měněná `Rotation::setAngle()` a více `DrawableObject`: **zatím nevyhodnoceno**.
- Skutečné sdílení jednoho `Model` a `ShaderProgram` mezi více vykreslovanými instancemi; ověření lifetime a zániku: **zatím nevyhodnoceno**.
- Vylepšení shader deploymentu v CMake (aby změna GLSL nevyžadovala ruční reload) nebylo implementováno; jde o následnou údržbu.
- Odstranění nepoužívaných `<algorithm>` / `<ranges>` a nadbytečného `<vector>` v `DrawableObject`; explicitní `<utility>` pro `std::move`; příp. kontroly nenulovosti nevlastnících pointerů – menší úklid, ne podmínka fungování.
- Závislost na OpenGL contextu při destrukci i při selhání konstrukce resources: širší robustnost a ošetření chyb **zatím nevyhodnoceno**.

## Další procvičení

1. Bez nápovědy zakreslit ownership a použití: `Application → DrawableObject (own) → Transformation (own)`; `DrawableObject → Model/ShaderProgram (use)`. Slovně vysvětlit, proč při `~Application()` probíhá destrukce v tomto pořadí.
2. Na malém příkladu rozhodnout, kdy použít `.get()` vs. `std::move()`; uvést, který `unique_ptr` po přesunu už nevlastní objekt.
3. Nakreslit tok `Translation → getMatrix → setMat4 → glGetUniformLocation / glUniformMatrix4fv → GLSL → gl_Position` a objasnit aktivní program.
4. Vyzkoušet `DrawableObject` s `CompositeTransformation` (posun + rotace + měřítko), zkontrolovat pořadí na konkrétním vrcholu.
5. Vykreslit dvě nezávislé instance stejného `Model` a `ShaderProgram` s odlišnými transformacemi; doplnit kontrolu lifetime a až poté rozhodnout, zda je potřeba `Scene`.
6. Zabezpečit aktualizaci kopie `shaders/basic.vert` při úpravách a ověřit odlišnost zdrojového a běhového souboru.

## Git důkazy

- `4d0ce7fc`: GLSL `modelMatrix` a C++ metoda `ShaderProgram::setMat4()`.
- `af89031c`: první test posunu přes `Translation` přímo v `Application::run()`.
- `8bcd2c5b`: první `DrawableObject` – nadbytečná kolekce transformací a ruční vykreslování přes gettery `Model`.
- `09a3630e`: oprava odpovědností – `unique_ptr<Transformation>`, `shaderProgram->use()`, `setMat4()`, `model->draw()`; gettery `Model` odstraněny.
- `9ce7c23e`: první, chybná integrace `DrawableObject` do `Application` a oprava initializer listu.
- `af678a0c`: správné `.get()` a `std::move()`, `drawableObject->draw()` v render loop a explicitní pořadí `reset()`.
- Diskuse v tomto chatu 15. 10. 2026: žádost o vysvětlení uniformních OpenGL funkcí, potvrzení řešení problému přes Reload CMake a závěrečné potvrzení „ok funguje to“.

**Rozsah checkpointu:** dokumentační aktualizace po kódovém commitu; bez změn zdrojového kódu nebo automatického vytváření testů.
