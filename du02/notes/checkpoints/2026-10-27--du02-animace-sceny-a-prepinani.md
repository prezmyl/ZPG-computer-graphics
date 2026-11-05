# Learning checkpoint – DU02: kompozice animací, scény a přepínání

**Datum:** 2026-10-27  
**Etapa:** od `Rotation*` přímo v `Application` k polymorfním animacím, vlastnictví scén a přepínání klávesami  
**Předchozí checkpoint:** `2026-10-27--du02-dynamicke-transformace-a-pristup-ke-stavu.md` (první animovaný strom)  
**Poslední ověřený zdrojový commit:** `a421f094bd8fe2b8f810919a4bcf59689de91d2f`  
**Rozsah ověření:** GitHub aktuální soubory, změny commitů a diskuse. Nový build ani běh aplikace asistent neprováděl. Název commitu „Check“ není náhradou za nezávislý běhový test; v tomto úseku student výslovně nepotvrdil výsledek přepínání.

## Stav řešení

1. Nové `Animation` je abstraktní rozhraní s `virtual void update(float time) = 0` a virtuálním destruktorem.
2. `RotationAnimation` toto rozhraní implementuje. Drží **nevlastnící** `Rotation*` a úhlovou rychlost `float angularSpeed`; metoda `update(time)` nastavuje úhel `angularSpeed * time` (absolutní čas od spuštění, počáteční úhel 0).
3. `Scene` je **nadále konkrétní třída**, ne abstraktní předek `ForestScene`/ `LoginScene`. Vlastní dvě kolekce: `objects` (`vector<unique_ptr<DrawableObject>>`) a `animations` (`vector<unique_ptr<Animation>>`). `Scene::update(time)` aktualizuje animace a `Scene::draw()` vykresluje objekty.
4. `Application` nyní vlastní `vector<unique_ptr<Scene>> scenes` a `std::size_t activeSceneIdx`; v hlavní smyčce volá `update(glfwGetTime())` a `draw()` na **jedné** vybrané scéně. `key_callback` přes `glfwGetWindowUserPointer(window)` získá `Application*` a pro klávesy `1`/`2` volá `setActiveScene(0/1)`.
5. `createResources()` naplní scénu lesa stromy/keři a jednou `RotationAnimation`, poté ji přesune do `scenes`. Druhá položka je **prázdná** `Scene` označená `sceneLogin`: model loginu ani jeho animace ještě nejsou vytvořené.
6. Zánik `Application` začíná `scenes.clear()`, takže všechny scény/jejich závislé objekty zaniknou před `ShaderProgram` a `Model` a před zničením OpenGL contextu. V `Scene` jsou členské `objects` deklarované před `animations`, takže při běžném zániku členů zaniknou **animace první**.

**Otevřená funkční chyba ve finálním commitu:** `Application::setActiveScene(std::size_t idx)` kontroluje `if (activeSceneIdx < scenes.size())`, avšak následně ukládá `activeSceneIdx = idx`. Kontrola se týká dosavadního indexu, ne navrhovaného `idx`; neplatná hodnota by mohla vést k přístupu `scenes[activeSceneIdx]` mimo kolekci. Oprava dosud **není** na GitHubu: zkontrolovat `idx < scenes.size()` (a pro širší robustnost zajistit ne-prázdnou kolekci před `run()`). Klávesy 1/2 v dnešním stavu předávají existující indexy; chyba se tak při nich nutně neprojeví.

## Samostatně prokázáno – důkazy z vlastních úvah

- **Kritika nepotřebné dědičnosti.** Student se sám zeptal, proč potřebujeme specializované `ForestScene` a `SolarSystemScene`, co by se stalo s `Scene` a zda nestačí více instancí téže třídy s jinými pravidly. Správně upozornil na riziko, že obecná `Scene` by jinak musela znát pravidla všech scén. To je nezávislá návrhová úvaha; konkrétní mechanismus oddělení chování do `Animation` následně vysvětlil asistent.
- **Oddělení kolekce a výběru.** Na návrhovou otázku student bez hotových typů odpověděl, že `Application` má vlastnit kolekci, například `vector`, a další členskou proměnnou označující aktivní scénu. Typ `vector<unique_ptr<Scene>>`, `std::size_t` a detail render loop následně dodal asistent.
- **Návrh veřejné metody.** Na problém komunikace statického GLFW callbacku s instancí `Application` student navrhl getter/setter. To je validní část řešení; mechanismus `glfwSetWindowUserPointer` a `glfwGetWindowUserPointer` byl už novým, studentovi neznámým API vysvětlen a dodán asistentem.
- **Aktivní práce s vlastnictvím:** v posledním commitu zanechal student komentář vysvětlující, proč se `addScene(std::move(sceneForest))` přesunulo až po naplnění scény. Je to doklad, že korekci použil se zdůvodněním; původní chybu i konkrétní opravu označil asistent, proto *nezávislé* odvození zůstává k prověření.

## Pochopeno / implementováno s vedením

- Rozdělení odpovědností `Application → Scene::update() → Animation::update() → Rotation::setAngle()`; konstrukce `Animation` a `RotationAnimation` i migrace volání do scény probíhaly podle dílčích instrukcí. Klíčovou volbu uchovávat `Rotation*` a `angularSpeed` v `RotationAnimation` dodal asistent, student původně odpověděl „čas a úhel“.
- `Scene` drží *libovolné* animační objekty za společným polymorfním rozhraním; nepotřebuje znát konkrétní `RotationAnimation`. Vztah připomíná skládání strategií chování, ale zde jde především o kompozici více aktualizačních komponent, nikoli povinné zavedení GoF Strategy.
- Destrukce a lifetime: student navrhoval nulovat nevlastnící pointer před zničením transformace; s vysvětlením se ukázalo, že důležité je ukončit jeho **používání** před zánikem vlastníka. `if (ptr)` nechrání před dangling pointerem; destrukce členů C++ probíhá opačně než jejich deklarace. Samostatné vysvětlení po korekci **zatím nevyhodnoceno**.
- Callback jako adaptér na objektovou aplikaci: `this` je uložen v GLFW window user pointeru; callback si jej vyžádá jako `Application*` a zavolá veřejné `setActiveScene()`. GLFW tím `Application` nevlastní.
- Opravy chyb v multi-scene commitu `6343457` po code review: `sceneForest` byl nejprve použit *po* `std::move`; `~Application` původně resetoval pouze aktivní scénu. Commit `a421f09` přesunul `addScene(move)` na konec stavby lesa a použil `scenes.clear()`. Oprava indexového guardu je ale stále chybná.

## Vývoj mentálního modelu

1. **Po funkční jediné animaci** chtěl student odlišné scény, ale nerozuměl nutnosti potomků `Scene`. Sám zpochybnil dědičnost a formuloval její nevýhody.
2. **Nový návrh vyšel z kompozice:** jedna konkrétní `Scene` získá obsah i animace jako objekty. Pravidla jednotlivých animačních komponent se implementují polymorfně; `Scene` sama nepotřebuje znát jejich typy.
3. **Od přímého řízení k delegaci:** `Application` dříve držela `Rotation*`; nyní jen určuje čas a aktivní scénu. `Scene` rozhoduje, které animace aktualizuje, a `RotationAnimation` počítá úhel.
4. **Další stupeň vlastnictví:** místo jednoho `unique_ptr<Scene>` aplikace vlastní několik scén. Student sám navrhl kolekci + výběr, ale první implementace přesunula `sceneForest` před jeho dalším používáním; po nápovědě student opravil místo převodu vlastnictví.
5. **Přesnost validace stále kolísá:** při opravě ochrany indexu student zaměnil `idx` za `activeSceneIdx`. Potřebujeme samostatně procvičit rozlišení *požadované hodnoty* a *současného stavu*.

## Generalizovatelné poučení / nejvyšší abstrakce

**Proměnlivé chování nemusí vést k nové podtřídě kontejneru.** Pokud se instance liší obsahem nebo pravidly, lze odlišné chování vložit přes společné rozhraní do kompozice. `Scene` tak zůstává konkrétní a může agregovat různé `Animation`. Obecná `Scene` není nucena růst o větve `if(sceneType)`.

**Vlastnictví, identita a výběr jsou různé informace.** `vector<unique_ptr<Scene>>` zajišťuje životní cyklus, `activeSceneIdx` identifikuje pouze aktuální volbu. Přesun `unique_ptr` mění vlastníka, nikoli identitu či adresu pointee; *přesunutý* `unique_ptr` však typicky už objekt neobsahuje.

**Při změně stavu validuj novou hodnotu, ne starou.** Kontrakt `setActiveScene(idx)` musí ověřit `idx` proti velikosti kolekce. Současný stav může být platný a nový vstup přesto neplatný. To je přenositelné na validace ID, indexů, limitů a změn konfigurace.

**Aktualizace a zobrazení tvoří dvě fáze se samostatnou odpovědností.** `Scene::update(time)` změní stav skrze animace, `Scene::draw()` ho vykreslí; odpovědnost za výpočet matrice zůstává `Transformation` a přenos dat do GPU `DrawableObject` / `ShaderProgram`.

## Potřebuje upevnit a otevřené otázky

- **Chyba:** `setActiveScene(idx)` používá v guardu `activeSceneIdx` místo `idx`. Třeba opravit a cíleně otestovat indexy 0, 1, 2 a prázdnou kolekci / chybějící scénu.
- **Návrhově/životnost:** co se stane s nevlastnícím `Rotation*`, když odstraníme `DrawableObject` za běhu, zatímco `RotationAnimation` zůstane ve scéně. Destrukční pořadí členů tento runtime případ samo neřeší.
- **Zatím nevyhodnoceno:** samostatné navržení obecné animační komponenty bez uvedeného rozhraní, rozhodnutí absolutní čas vs. deltaTime, pozastavení neaktivní scény a chování při opětovném aktivování.
- **Zatím nevyhodnoceno:** samostatná obhajoba GLFW user pointeru, `static_cast` a životnosti callbacku.
- **Funkční testy:** nezávislý build, běh a přepínání klávesami 1/2 nebyly v tomto checkpointu ověřeny asistentem; v chatu není explicitní výsledek testu. Druhá scéna je zatím úmyslně prázdná.
- **DU02/DU03:** chybí rotující 3D login, hierarchická sluneční soustava, následující shadery s osvětlením a správný aspect ratio při změně rozměru okna. Nejde o dokončené DU02 ani DU03.

## Další procvičení

1. Opravit `setActiveScene(idx)` s ověřením nové hodnoty; cíleně vyzkoušet případy platného a neplatného vstupu.
2. Bez nápovědy vysvětlit rozdíl mezi `std::move(sceneForest)` a uložením nevlastnícího `Scene* sceneRef = sceneForest.get()` před přesunem; určit, kdy by se `sceneRef` stal neplatný.
3. Vymezit fáze vytvoření/scény a modelů, `update(time)`, `draw()` a destrukci na konkrétním diagramu vlastnictví.
4. Doplnit druhou scénu skutečným modelem rotujícího loginu a vlastní `RotationAnimation`, aby byl prokázán rozdíl mezi dvěma instancemi `Scene`.
5. Připravit transformace Země/Měsíc s oběhem a vlastní rotací; u každé určit, kdo vlastní transformaci a která animační komponenta mění úhel.

## Důkazy – relevantní Git commity

- `e5d3056bbc79ec3d16ede9562bf9cdbed5d4fcd4` – vznik `Animation` a `RotationAnimation`.
- `494265253ff6c6077ab982497df5aa87e46fd567` – `Scene::addAnimation()`, `Scene::update()` a polymorfní kolekce animací.
- `a611dce94f86f2e631c637697277a98e1e87087d` – `Application` přestává přímo aktualizovat `Rotation`, používá `scene->update()`.
- `52a5ba9a5155eb945675938b8f5c7461239e2288` – úklid po první integraci a doplněný UML diagram.
- `6343457aa8cc9a166215b559db7f8e65b1933383` – první multi-scene implementace, chybný přesun `sceneForest` a destrukce pouze aktivní scény.
- `a421f094bd8fe2b8f810919a4bcf59689de91d2f` – přesun `addScene` po naplnění scény, `scenes.clear()`, dosud chybná validace indexu.
- Diskuse 2026-10-27: vlastní kritika specializovaných podtříd, návrh `vector` + aktivní proměnná, návrh setteru; opravy a přesné API s vedením.

**Rozsah checkpointu:** výhradně learning dokumentace. Žádný zdrojový soubor ani shader nebyl změněn.
