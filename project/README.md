# ZPG – mapa znalostí

Tento soubor je průběžná mapa aktuálního pochopení získaného při práci na projektu ZPG.

Zdrojový kód a Git historie ukazují, **co se změnilo**. Checkpointy a konceptové poznámky zachycují, **proč se to změnilo a co bylo pochopeno**.

## Aktuální stav

### Samostatně prokázáno

- **DU02 – návrh více scén (27. 10.):** student bez nabídnutých typů navrhl kolekci (např. `vector`) a samostatnou členskou proměnnou pro výběr aktivní scény. Na základě vlastní úvahy zpochybnil dědičnost `ForestScene`/`SolarSystemScene` a upozornil, že společná `Scene` by neměla obsahovat všechna konkrétní pravidla.
- **DU02 – přístup ze vstupu:** jako část řešení přístupu ke stavu `Application` ze statického callbacku student sám navrhl veřejný getter/setter. Specifický GLFW user-pointer mechanismus dodal asistent.

- **DU02 (27. 10. – dynamická transformace):** student vlastními slovy správně popsal ownership `Scene → DrawableObject → Transformation/Composite → Rotation`, rozlišil použití sdílených `Model*/ShaderProgram*` a navrhl nevlastnící přístup nebo alternativu getterů.
- **DU02 (27. 10.):** bez podání hotového řešení zrekonstruoval tok `Scene::draw → DrawableObject::draw → Transformation::getMatrix → ShaderProgram::setMat4 → Model::draw/glDrawArrays` a správně odvodil, proč po `setAngle` není nutný nový `setTransformation`.

- DU02: správně vysvětlena asociativita a jednotková matice jako neutrální prvek; rozlišen parametr posunu od jeho matice a aplikace na vrchol.
- DU02: u vnořeného Composite rozpoznáno, že stejné polymorfní rozhraní `getMatrix()` stačí i bez rozpoznávání konkrétního potomka; zvolen přístup `Rotation` dostává nový úhel zvenčí.

- Rozdělení monolitického OpenGL programu podle odpovědností: režie aplikace, geometrie, shadery, shader program a vyšší koordinace vykreslování.
- VAO/VBO jsou součástí geometrické reprezentace; `modelMatrix` naopak popisuje konkrétní instanci objektu.
- Geometrie a shader program mohou být sdílené mezi více objekty, zatímco transformační stav je per-instance.
- `Model::draw()` může používat vlastní členský `vertexCount`; není nutné jej při každém volání znovu předávat.
- Pro DU1 bylo vědomě rozhodnuto nezavádět `Scene` předčasně.

### Pochopeno s vedením

- **DU02 – animační komponenty a polymorfismus (27. 10.):** vytvořeny `Animation` a `RotationAnimation`, přidány `Scene::addAnimation()` a `Scene::update(time)`. Mechanismus kompozice animací a doporučené rozhraní dodal asistent; student implementoval a pushnul jednotlivé etapy.
- **DU02 – přepínání scén (27. 10.):** `Application` vlastní `vector<unique_ptr<Scene>>`, vybírá `activeSceneIdx`, callback získává `Application*` přes GLFW window user pointer. Konkrétní API, práce s `.get()` a nutnost kontroly indexu byly vysvětleny s vedením.
- **DU02 – oprava lifetime chyb:** první verze přesunula `sceneForest` do kolekce před jejím dalším používáním; s vedením přesunuta registrace na konec konstrukce scény. Destruktor nyní používá `scenes.clear()` místo resetu jediné aktivní scény.

- **DU02 (27. 10. – první animace):** rozlišení jednorázového `createResources()` od aktualizace v `run()`; použití nevlastnícího `Rotation*` získaného přes `.get()` před `std::move()`; oprava původního `nullptr` a změna pořadí matic na `T * R * S` (Scale → Rotation → Translation, osa Y). Konkrétní opravy poskytl asistent, student je integroval a funkčnost potvrdil.

- DU02 (15. 10.): zprovozněno předání `modelMatrix` do vertex shaderu přes `ShaderProgram::setMat4()` a nové `DrawableObject::draw()`. Implementace vznikala po vysvětlení jednotlivých OpenGL funkcí a po nápovědách k rozdělení odpovědností.
- DU02: v první verzi `DrawableObject` student duplikoval Composite kolekcí transformací a `Model` přímým OpenGL draw; s vedením opraveno na jedinou vlastněnou `Transformation` a volání `Model::draw()`.
- DU02: v `Application` opraveno předání nevlastnících adres přes `.get()` oproti předání vlastnictví transformace přes `std::move()`. Student potvrdil fungující vykreslení jedné instance.

- DU02: vytvořeny konkrétní `Transformation`, `Translation`, `Rotation`, `Scale` a `CompositeTransformation`; podoba rozhraní, GLM API a algoritmus Composite však byly z velké části dodány s vedením.
- DU02: samostatně zdůvodněno oddělení výpočtu transformační matice od jejího použití při vykreslování; rozpoznána potřeba nezávislého mutable transformačního stavu objektů.
- DU02: s vedením opraveno chybné rozhraní první `Translation` (`mat4` a `getTranslation()`), osvojeny `glm::vec3`, jednotková `glm::mat4`, `getMatrix() const override`, radiány a přenos ownershipu přes `std::move`.
- DU02: správné pořadí násobení v Composite bylo zvoleno mezi předloženými variantami a implementováno; první chybějící `return result;` odhalil asistent a student opravil.

- Rozlišení „koordinátor rozhoduje co/kdy“ vs. „specializovaný objekt ví jak“.
- Forward declaration pro `GLFWwindow*` a omezení závislostí hlaviček.
- Rozdíl mezi lokální a členskou proměnnou na příkladu zastínění `window`.
- VBO jako OpenGL handle, nikoli velikost dat.
- Ztráta informace o velikosti C pole po převodu na pointer a důsledky pro `sizeof(points)`.
- Rozdíl mezi počtem vertexů, počtem `float` hodnot a velikostí dat v bajtech; pro aktuální layout je jeden vertex `6 * sizeof(float)`.
- Význam `stride` a `offset` v `glVertexAttribPointer` pro layout `position.xyz + color.rgb`.
- Použití `const float*` a member initializer listu v konstruktoru `Model`.
- U `Shader` rozlišení trvalého stavu (`id`) od dočasných dat konstrukce (typ, cesta, source, compile status, log).
- U `ShaderProgram` rozlišení kompilovaného shaderu od slinkovaného programu, použití vstupních `Shader` objektů přes `const&` a hranice cleanupu podle ownershipu.

### Potřebuje upevnit

- **OTEVŘENÁ CHYBA na `a421f09`:** `setActiveScene(idx)` chybně validuje `activeSceneIdx < scenes.size()` místo vstupního `idx < scenes.size()`. Bez opravy není kontrakt setteru bezpečný pro neplatný index. Otestovat i chování bez scén.
- **DU02 – vlastnictví a runtime lifetime:** samostatně vysvětlit použití `unique_ptr` po `std::move`, podmínku platnosti nevlastnícího `Rotation*` při odebrání objektu, pořadí zániku `animations`/ `objects` a `scenes.clear()`.
- **DU02 – nové animační rozhraní:** bez nápovědy navrhnout jiný druh animace, obhájit absolutní čas proti `deltaTime` a vyhodnotit chování při přepnutí neaktivní scény.

- DU02: vztahy typový (IS-A), strukturální (HAS-A/USES-A) a ownership/lifetime, polymorfní vlastnictví prostřednictvím abstraktní třídy – vysvětleno s vedením, samostatné použití zatím neověřeno.
- DU02: `CompositeTransformation::getMatrix()` už existuje a vrací matici, ale samostatné odvození algoritmu i jeho jednotkové/běhové testy jsou zatím nevyhodnocené.
- DU02: propojení `DrawableObject` → `ShaderProgram` → `modelMatrix` už student vyzkoušel funkčně; samostatná obhajoba uniform API, lifecycle a ownershipu je stále k upevnění.
- DU02: funkční jedna animovaná rotace v render loop a Composite použitý ve více instancích lesa; **k upevnění:** samostatný návrh aktualizace více objektů, životnost nevlastnících odkazů při změně scény a samostatné odvození správného pořadí matic.
- DU02: důvod destrukce `DrawableObject` před `Model`/`ShaderProgram` – kód má správné pořadí, ale komentář mylně uvádí, že `DrawableObject` oba objekty vlastní; tento mentální model je potřeba ověřit a opravit.

- Samostatné odvození stride a offsetů u jiného vertex layoutu než současného `position + color`.
- Základní RAII lifecycle pro DU1 je funkční: GPU resources vznikají po vytvoření contextu a jsou explicitně zničeny před zničením window/contextu.
- Ownership/lifecycle `Shader`, `ShaderProgram` a `Model` je prakticky implementovaný, ale zatím převážně pochopený s vedením; je potřeba jej později samostatně zopakovat.
- Uniformy: rozdíl mezi location uloženou vůči programu a významovou hodnotou vlastněnou objektem/scénou.
- `Application` nyní vyjadřuje jednoznačné vlastnictví `Model` a `ShaderProgram` přes `std::unique_ptr`; případné skutečné shared ownership je zatím nevyhodnoceno.

### Další procvičení

- **Nejbližší krok:** opravit `setActiveScene(idx)` tak, aby kontroloval nově požadovaný index, a ověřit výběr scén 0/1 i odmítnutí neexistujícího indexu.
- Vytvořit skutečnou druhou scénu s rotujícím loginem a vlastní `RotationAnimation`, vyzkoušet klávesy 1/2 a odůvodnit lifetime všech používaných odkazů.
- Před solárním systémem navrhnout vnořené transformace oběhu Země a Měsíce, oddělit rotaci kolem vlastní osy od oběhu.

- DU02: bez nápovědy vysvětlit a otestovat pořadí `T*R` proti `R*T` na konkrétním vrcholu, prázdný a vnořený Composite.
- DU02: doplnit CPU testy transformací a Composite (včetně vnoření), samostatně navrhnout aktualizaci více rotací a vyzkoušet změnu scény při existujícím nevlastnícím `Rotation*`.
- DU02: bez nápovědy vysvětlit `.get()` vs. `std::move()`, uniform location vs. hodnotu a správnou příčinu pořadí destrukce.
- Opravit deployment GLSL v CMake, aby změny shaderu nemusely vyžadovat ruční Reload CMake.

- U dalšího vertex layoutu samostatně určit počet hodnot, velikost v bajtech, stride a offsety.
- U dalšího resource-owning typu samostatně odvodit ownership, konstrukční závislosti, copy/move politiku a pořadí destrukce.
- Později nahradit `exit(EXIT_FAILURE)` propagací chyby kompatibilní s RAII.
- Pro více animací ověřit rozdělení řízení času mezi `Application`, `Scene` a jednotlivé `Animation`; současná `Scene` již má `addAnimation()` a `update()`.

## Generalizovatelné programovací a návrhové principy

- **Proměnlivé chování lze skládat, ne dědit celé kontejnery:** různé instance `Scene` mohou obsahovat odlišné `Animation` implementující jednotné `update(time)`. Tím se zabrání pravidlům typu `if(sceneType)` v obecném kontejneru.
- **Vlastnictví, aktuální volba a možnost přístupu jsou různé vztahy:** `scenes` vlastní objekty, `activeSceneIdx` pouze vybírá a `glfwGetWindowUserPointer` poskytuje nevlastnící přístup k aplikaci.
- **Při změně validuj nový požadavek, ne dosavadní hodnotu:** `setActiveScene(idx)` musí ověřit parametr `idx`, ne `activeSceneIdx`; současný kód zde má otevřenou chybu.
- **Přesun vlastnictví mění platnost vlastnícího handle, ne adresu objektu:** po `std::move(unique_ptr)` původní `unique_ptr` objekt nevlastní; dříve získaný raw pointer může zůstat platný, dokud pointee existuje.

- **Odděl změnu stavu od výpočtu odvozeného výsledku a jeho použití:** úhel `Rotation` se mění v čase, `getMatrix()` jej při kreslení převádí na matici a `DrawableObject` ji předává do GPU; není nutná duplicita uložené matice či nový `setTransformation()`.
- **Vlastník a pozorovatel mohou být různé objekty:** `unique_ptr` řídí zánik, raw pointer umožní přístup; po přesunu `unique_ptr` pointee zůstává, po zániku vlastníka však pozorovatel nesmí být použit.
- **Rozlišuj lokální rotaci a oběžný pohyb:** u sloupcových vektorů `T * R * S` značí jiný pohyb než `R * T * S`; pořadí skládání vyjadřuje požadovanou sémantiku, ne jen technický detail.

- **Životnost informace určuje místo uložení:** co musí přežít jedno volání a tvoří stav objektu, patří typicky do členské proměnné; krátkodobé pomocné údaje patří do lokálních proměnných; vstup použitý jen pro jednu operaci nemusí objekt uchovávat.
- **Koordinátor rozhoduje co a kdy, specializovaný objekt zná jak:** např. vyšší vrstva rozhodne, že se model vykreslí nebo shader použije, ale `Model` a `ShaderProgram` znají konkrétní OpenGL mechanismus.
- **Odpovědnost má následovat důvod ke změně:** VBO/VAO patří ke geometrii, protože se mění spolu s reprezentací modelu; transformační stav naopak popisuje konkrétní instanci objektu.
- **Rozhraní musí respektovat významové jednotky API:** počet vertexů, počet hodnot a počet bajtů nejsou zaměnitelné, i když jsou všechny reprezentovány číslem.
- **Nezavádět abstrakci bez problému, který řeší:** `Scene` a `DrawableObject` byly odloženy, dokud pro ně nevznikne skutečná odpovědnost.
- **Procedurální kód převádět do OOP podle odpovědností, ne podle bloků řádků:** najít důvod ke změně, vstupy, závislosti, ownership, trvalý stav a minimální veřejné rozhraní.
- **Použití není vlastnictví:** objekt nemá řídit životní cyklus závislosti jen proto, že ji při své práci potřebuje.
- **Cleanup následuje ownership:** prostředek má být uvolněn vlastníkem v okamžiku odpovídajícím jeho životnímu cyklu.
- **RAII není jen destruktor:** resource-owning typ musí řešit i copy/move semantiku a závislosti potřebné při destrukci.
- **Typ pointeru je důsledek ownershipu a životnosti:** `unique_ptr` vyjadřuje jednoho vlastníka a umožňuje odložený vznik/zánik objektu.
- **Ownership a dependency jsou odlišné vztahy:** objekt může resource vlastnit a současně při jeho cleanupu záviset na jiném živém systému.
- **Konstrukce a destrukce mají pořadí odvozené ze závislostí:** OpenGL resources musí vzniknout po contextu a zaniknout před ním.
- **Scope modeluje životnost:** krátkodobé objekty mají zůstat lokální, dlouhodobý stav má být uložen jen tam, kde je skutečně potřeba.
- **Fáze s různým důvodem ke změně lze oddělit metodou dříve než novou třídou:** `initialize()` řeší infrastrukturu, `createResources()` GPU obsah.
- **Compiler chyby analyzuj přes include/instantiation řetězec:** hledej skutečný mechanismus chyby, ne pouze poslední řádek diagnostiky.

- **Rozlišuj typ, vazbu a vlastnictví:** IS-A určuje substituovatelnost, HAS-A/USES-A spolupráci a ownership zánik; v DU02 byl princip vysvětlen na `DrawableObject` a `Transformation`, samostatné upevnění ještě čeká.
- **Stejné parametry nejsou stejná instance:** dvě rotace o stejném úhlu mohou být nezávislé, sdílená mutable rotace přenáší změny mezi uživateli.

- **Parametry jsou zdrojem pravdy, matice je odvozený výsledek:** u elementárních transformací DU02 jsou uloženy `vec3`/`float`, `getMatrix()` vypočítá `mat4`.
- **Pořadí je součástí kontraktu skládání:** výsledek transformací závisí na pořadí členů; Composite používá jednotné polymorfní rozhraní pro část i celek.
- **Význam dat a mechanismus jejich přenosu jsou různé odpovědnosti:** `DrawableObject` má modelovou transformaci, `ShaderProgram` umí obecně nastavit uniform a `Model` zná vykreslení geometrie.
- **Skutečně použité runtime soubory je nutné ověřit:** starý GLSL shader v build adresáři může přežít recompilaci C++; CMake konfigurace, kopírování a runtime načtení nejsou totéž.

## Aktuální architektonický stav

**DU02 – animace a dvě instance scén (GitHub code commit `a421f09`, 2026-10-27):**

- `Application` vlastní dvě scény přes `vector<unique_ptr<Scene>>`, dva `Model` (strom, keře) a `ShaderProgram`. Proměnná `activeSceneIdx` vybírá aktuální scénu pro `update(time)` a `draw()`.
- `Scene` zůstává **konkrétní třídou**, vlastní `vector<unique_ptr<DrawableObject>> objects` a `vector<unique_ptr<Animation>> animations`. Není potřeba odvozovat potomky pro každý obsah.
- `Animation` je abstraktní rozhraní, `RotationAnimation` uchovává nevlastnící `Rotation*` a rychlost. `Application` už neukládá vlastní `animatedRotation`, pouze dodává scéně čas přes `Scene::update(glfwGetTime())`.
- Les má 20 stromů a 30 keřů, jedna instance stromu používá animovanou rotaci Y; všechny transformace se předávají při `DrawableObject::draw()` do GLSL přes `modelMatrix`. Shaderové úpravy ze cvičení zůstávají zachované.
- Druhá scéna (`sceneLogin`) je **zatím prázdná**; callback kláves 1/2 získává instanci aplikace přes GLFW user pointer a volá `setActiveScene(0/1)`.
- Destruktor používá `scenes.clear()` před uvolněním shader programu, modelů a OpenGL contextu. Ve `Scene` se díky pořadí deklarace při zániku ruší animace před objekty.
- **Zbývá opravit chybu indexu:** `setActiveScene(idx)` kontroluje `activeSceneIdx` místo `idx`. Zkontrolovaný zdrojový kód nemá potvrzený nezávislý build či test přepínání. Zatím není hotový rotující login, sluneční soustava ani osvětlení DU03 / aktualizace projection aspect ratio.

Přenos `unique_ptr` vyžaduje pozorně oddělit vlastnící proměnnou a identitu pointee. Při odstraňování `DrawableObject` za běhu může `RotationAnimation` zůstat s neplatným nevlastnícím pointerem; běžné pořadí destrukce celé scény samo takový případ nepokrývá.

## Oblasti

Mapa se bude průběžně rozšiřovat zejména o:

- OpenGL pipeline,
- GLFW a GLAD,
- shadery a GLSL,
- transformace a GLM,
- model/view/projection,
- návrh tříd aplikace, modelu a shader programu,
- práce s buffery, atributy a uniformy,
- render loop a životní cyklus OpenGL aplikace,
- debugging grafického programu.

## Konceptové poznámky

Obecně použitelné poznatky jsou v:

`project/notes/concepts/`

Aktuálně:
- `opengl-objekty-a-odpovednosti.md`
- `generalizovatelne-programovaci-principy.md`
- `transformace-a-composite.md`
- `uniformy-a-drawableobject.md`
- `sceny-animace-a-prepinani.md` (kompozice animačních pravidel, změna ownershipu, GLFW callback)

Poznámka `transformace-a-composite.md` aktualizována 2026-10-27 o dynamickou rotaci a lifetime nevlastnících ukazatelů.

## Learning checkpointy

Datované checkpointy jsou v:

`project/notes/checkpoints/`

Aktuálně:
- `2026-10-09--application-a-model.md`
- `2026-10-09--vertex-layout-vbo-vao.md`
- `2026-10-11--shader.md`
- `2026-10-11--shader-program.md`
- `2026-10-11--raii-ownership-lifecycle-mezistav.md`
- `2026-10-11--funkcni-oop-render-pipeline.md`
- `2026-10-13--du02-navrh-transformaci-a-ownership.md` (teoretický návrh, bez implementace)
- `2026-10-15--du02-transformace-a-composite-implementace.md` (samostatná hierarchie transformací)
- `2026-10-15--du02-drawableobject-a-prenos-matice-do-gpu.md` (uniformy, vlastnictví a první fungující DrawableObject)
- `2026-10-27--du02-dynamicke-transformace-a-pristup-ke-stavu.md` (první animovaný strom, vlastnictví vs. pozorování, změna stavu vs. render)
- `2026-10-27--du02-animace-sceny-a-prepinani.md` (kompozice chování, více scén, index, chyby ownershipu)

Checkpoint se v učitelském chatu spouští slovem:

`checkpoint`

Podrobná pravidla jsou v `project/AGENTS.md`.
