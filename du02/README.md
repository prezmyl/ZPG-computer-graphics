# ZPG – mapa znalostí

Tento soubor je průběžná mapa aktuálního pochopení získaného při práci na projektu ZPG.

Zdrojový kód a Git historie ukazují, **co se změnilo**. Checkpointy a konceptové poznámky zachycují, **proč se to změnilo a co bylo pochopeno**.

## Aktuální stav

### Samostatně prokázáno

- **DU02 (28. 10.) – asociativita a přenos modelové matice:** student vlastním vyjádřením správně odvodil, že jiné závorkování stejného řetězce matic nemění matematický výsledek, a že složená `modelMatrix` se nahraje do GPU pro všechny vrcholy konkrétního vykreslovaného objektu. Neprokazuje to ještě samostatné sestavení nového pořadí transformací.
- **DU02 (28. 10.) – aktivní porovnání přístupů:** student sám připomněl `T(P)*R*T(-P)` pro rotaci kolem obecného bodu a rozpoznal, že ho musí sladit s lokálním transformačním řetězcem planet. Samostatné zdůvodnění rozdílu zatím neověřeno.
- **DU02 (28. 10.) – rozpoznání meze implementace:** student výslovně upozornil, že dynamická je zatím jen `Rotation`, a ptal se na translaci a škálování. Nepokládáme to za důkaz samostatného návrhu těchto animací.

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

- **DU02 (28. 10.) – sluneční soustava a skládání transformací:** Slunce, Země a Měsíc mají správné transformační řetězce, ale konkrétní vzorec a kód Měsíce dodal na žádost asistent. Student výsledek integroval a pushnul; samostatná konstrukce hierarchických matic zatím nebyla prokázána.
- **DU02 (28. 10.) – pořadí matic:** výslovně přiznaný zmatek mezi pořadím přidávání do Composite, násobením matic a působením na vrchol byl vysvětlen na `R*T` a `T*R`. Další samostatný příklad je potřebný.
- **DU02 (28. 10.) – statické a dynamické parametry:** rozdělení `Transformation` jako zdroje aktuální matice a `Animation` jako pravidla změny stavu bylo vysvětleno i pro možnou animovanou translaci a škálování; tyto animace zatím nejsou implementované.

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

- **Opravená chyba (commit `897793f`):** `setActiveScene(idx)` nyní kontroluje vstupní `idx < scenes.size()`; cílený test chování při prázdné kolekci a obecné životnosti scén zatím není doložen.
- **DU02 – vlastnictví a runtime lifetime:** samostatně vysvětlit použití `unique_ptr` po `std::move`, podmínku platnosti nevlastnícího `Rotation*` při odebrání objektu, pořadí zániku `animations`/ `objects` a `scenes.clear()`.
- **DU02 – transformační řetězec:** samostatně spočítat `R*T` a `T*R` na bodu; odlišit vložení transformací, zápis jejich součinu a skutečné působení na vrchol. Vysvětleno s vedením.
- **DU02 – nové animační rozhraní:** bez nápovědy navrhnout jiný druh animace, obhájit absolutní čas proti `deltaTime` a vyhodnotit chování při přepnutí neaktivní scény.

- DU02: vztahy typový (IS-A), strukturální (HAS-A/USES-A) a ownership/lifetime, polymorfní vlastnictví prostřednictvím abstraktní třídy – vysvětleno s vedením, samostatné použití zatím neověřeno.
- DU02: `CompositeTransformation::getMatrix()` už existuje a vrací matici, ale samostatné odvození algoritmu i jeho jednotkové/běhové testy jsou zatím nevyhodnocené.
- DU02: propojení `DrawableObject` → `ShaderProgram` → `modelMatrix` už student vyzkoušel funkčně; samostatná obhajoba uniform API, lifecycle a ownershipu je stále k upevnění.
- DU02: funkční rotace v lese i několik animací ve sluneční soustavě; **k upevnění:** samostatný návrh další animace, životnost nevlastnících odkazů při odebrání objektu a odvození správného pořadí matic.
- DU02: důvod destrukce `DrawableObject` před `Model`/`ShaderProgram` – kód má správné pořadí, ale komentář mylně uvádí, že `DrawableObject` oba objekty vlastní; tento mentální model je potřeba ověřit a opravit.

- Samostatné odvození stride a offsetů u jiného vertex layoutu než současného `position + color`.
- Základní RAII lifecycle pro DU1 je funkční: GPU resources vznikají po vytvoření contextu a jsou explicitně zničeny před zničením window/contextu.
- Ownership/lifecycle `Shader`, `ShaderProgram` a `Model` je prakticky implementovaný, ale zatím převážně pochopený s vedením; je potřeba jej později samostatně zopakovat.
- Uniformy: rozdíl mezi location uloženou vůči programu a významovou hodnotou vlastněnou objektem/scénou.
- `Application` nyní vyjadřuje jednoznačné vlastnictví `Model` a `ShaderProgram` přes `std::unique_ptr`; případné skutečné shared ownership je zatím nevyhodnoceno.

### Další procvičení

- **Nejbližší krok:** zkontrolovat aktuální `souhrn.txt` a požadované snímky pro odevzdání DU02; pokračovat DU03 – osvětlení a dynamický aspect ratio projekční matice.
- **Odložené:** rotující 3D login POL0511 (bod 7 je vědomě nesplněný; chybí Blender a místo na instalaci).
- Později procvičit bez nápovědy obecnou rotaci kolem bodu vs. oběh pomocí lokální soustavy a případnou animovanou translaci/škálování.

- DU02: bez nápovědy vysvětlit a otestovat pořadí `T*R` proti `R*T` na konkrétním vrcholu, prázdný a vnořený Composite.
- DU02: doplnit CPU testy transformací a Composite (včetně vnoření), samostatně navrhnout aktualizaci více rotací a vyzkoušet změnu scény při existujícím nevlastnícím `Rotation*`.
- DU02: bez nápovědy vysvětlit `.get()` vs. `std::move()`, uniform location vs. hodnotu a správnou příčinu pořadí destrukce.
- Opravit deployment GLSL v CMake, aby změny shaderu nemusely vyžadovat ruční Reload CMake.

- U dalšího vertex layoutu samostatně určit počet hodnot, velikost v bajtech, stride a offsety.
- U dalšího resource-owning typu samostatně odvodit ownership, konstrukční závislosti, copy/move politiku a pořadí destrukce.
- Později nahradit `exit(EXIT_FAILURE)` propagací chyby kompatibilní s RAII.
- Pro více animací ověřit rozdělení řízení času mezi `Application`, `Scene` a jednotlivé `Animation`; současná `Scene` již má `addAnimation()` a `update()`.

## Generalizovatelné programovací a návrhové principy

- **Rozlišuj sestavení operace od její aplikace:** Composite ukládá `T → R` a sestaví `R*T`, GPU tento součin aplikuje jako `R*(T*p)`. Asociativita dovoluje závorkování, ne změnu pořadí faktorů.
- **Lokální souřadnice snižují potřebu kompenzačních transformací:** model kolem lokálního počátku lze otočit a umístit; obecná rotace světového bodu kolem `P` má tvar `T(P)*R*T(-P)`.
- **Možnost animace a skutečná animace nejsou totéž:** společné `getMatrix()` podporuje obě varianty; časově měníme jen parametry `Rotation`. Přidat `TranslationAnimation` a `ScaleAnimation` by šlo bez změn `Composite`, ale zatím není potřeba.

- **Proměnlivé chování lze skládat, ne dědit celé kontejnery:** různé instance `Scene` mohou obsahovat odlišné `Animation` implementující jednotné `update(time)`. Tím se zabrání pravidlům typu `if(sceneType)` v obecném kontejneru.
- **Vlastnictví, aktuální volba a možnost přístupu jsou různé vztahy:** `scenes` vlastní objekty, `activeSceneIdx` pouze vybírá a `glfwGetWindowUserPointer` poskytuje nevlastnící přístup k aplikaci.
- **Při změně validuj nový požadavek, ne dosavadní hodnotu:** `setActiveScene(idx)` musí ověřit parametr `idx`, ne `activeSceneIdx`; původní chyba byla opravena commitem `897793f`.
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

**DU02 – tři scény s animacemi (ověřený GitHub commit `b9a53b6`, 28. 10. 2026):**

- `Application` vlastní `vector<unique_ptr<Scene>>`, modely stromů, keřů a koule, plus sdílený `ShaderProgram`; aktivní `Scene` vybírá `activeSceneIdx`. Pro klávesy 1/2/3 callback používá GLFW window user pointer a veřejnou metodu `setActiveScene()`. Index se kontroluje správně.
- `Scene` je stále **konkrétní třída** vlastnící `vector<unique_ptr<DrawableObject>>` a `vector<unique_ptr<Animation>>`; vykonává `update(time)` a `draw()`. `RotationAnimation` má nevlastnící `Rotation*` a časově mění úhel.
- **Scéna 1 – les:** 20 stromů a 30 keřů, vlastní modelové transformace, jedna rotující instance stromu.
- **Scéna 2 – login:** pouze prázdný placeholder. Rotující 3D login nebyl vytvořen; bod 7 DU02 je **vědomě nesplněný** kvůli momentálně nedostupnému Blenderu a prostoru pro instalaci.
- **Scéna 3 – sluneční soustava:** Slunce, Země a Měsíc používají společnou `sphereModel` a vlastní `CompositeTransformation`; Země se otáčí kolem vlastní osy a obíhá Slunce, Měsíc obíhá Země a spolu s ní Slunce. Oběh Země je ve transformaci Měsíce replikován synchronizovanou rotací (není parent-child scene graph). Commit obsahuje snímek `pol0511_8.png`.
- Dynamické jsou **rotace**, nikoli translace či měřítko. Pro geometrie se při `DrawableObject::draw()` vždy sestaví aktuální `modelMatrix` a nahraje do GLSL.
- Destruktor explicitně uvolňuje všechny scény, potom shader program a modely před zánikem OpenGL contextu. Při runtime odebrání objektu by však nevlastnící animační `Rotation*` mohl být neplatný; takové odebírání aplikace neimplementuje.
- Přepínání/pohyb má podle uživatele funkční výsledek; nezávislý build ani test asistenta neproběhl. Pro DU03 zbývá implementovat osvětlovací shadery a správný aspect ratio po změně rozměrů okna.

**DU02 odevzdání:** aktualizovaný školní souhrn je v `project/souhrn.txt`; aktuální seznam snímků zahrnuje `pol0511_4a.png`, `pol0511_4b.png`, `pol0511_6.png`, `pol0511_8.png`. Úplnost a naming dle požadavků zadání je nutné ještě zkontrolovat.

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

Poznámka `transformace-a-composite.md` aktualizována 2026-10-28 o řetězce matic, lokální souřadnice a sluneční soustavu.

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
- `2026-10-28--du02-slunecni-soustava-a-matice.md` (hierarchie pohybu, skládání matic, dynamické parametry, omezení DU02)

Checkpoint se v učitelském chatu spouští slovem:

`checkpoint`

Podrobná pravidla jsou v `project/AGENTS.md`.
