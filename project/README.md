# ZPG – mapa znalostí

Tento soubor je průběžná mapa aktuálního pochopení získaného při práci na projektu ZPG.

Zdrojový kód a Git historie ukazují, **co se změnilo**. Checkpointy a konceptové poznámky zachycují, **proč se to změnilo a co bylo pochopeno**.

## Aktuální stav

### Samostatně prokázáno

- Rozdělení monolitického OpenGL programu podle odpovědností: režie aplikace, geometrie, shadery, shader program a vyšší koordinace vykreslování.
- VAO/VBO jsou součástí geometrické reprezentace; `modelMatrix` naopak popisuje konkrétní instanci objektu.
- Geometrie a shader program mohou být sdílené mezi více objekty, zatímco transformační stav je per-instance.
- `Model::draw()` může používat vlastní členský `vertexCount`; není nutné jej při každém volání znovu předávat.
- Pro DU1 bylo vědomě rozhodnuto nezavádět `Scene` předčasně.

### Pochopeno s vedením

- DU02 (návrh): samostatně rozpoznána potřeba polymorfní kolekce transformací a skládání; konkrétní podoba Composite a rozhraní `getMatrix()` zatím diskutovány s vedením.
- DU02 (návrh): samostatně zdůvodněno oddělení výpočtu transformační matice od jejího použití při vykreslování; rozpoznána potřeba nezávislého mutable transformačního stavu objektů.

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

- DU02: vztahy typový (IS-A), strukturální (HAS-A/USES-A) a ownership/lifetime, polymorfní vlastnictví prostřednictvím abstraktní třídy – vysvětleno s vedením, samostatné použití zatím neověřeno.
- DU02: návrh `CompositeTransformation::getMatrix()`, pořadí matic, statické/dynamické transformace a finální ownership hierarchie – zatím nevyhodnoceno.

- Samostatné odvození stride a offsetů u jiného vertex layoutu než současného `position + color`.
- Základní RAII lifecycle pro DU1 je funkční: GPU resources vznikají po vytvoření contextu a jsou explicitně zničeny před zničením window/contextu.
- Ownership/lifecycle `Shader`, `ShaderProgram` a `Model` je prakticky implementovaný, ale zatím převážně pochopený s vedením; je potřeba jej později samostatně zopakovat.
- Uniformy: rozdíl mezi location uloženou vůči programu a významovou hodnotou vlastněnou objektem/scénou.
- `Application` nyní vyjadřuje jednoznačné vlastnictví `Model` a `ShaderProgram` přes `std::unique_ptr`; případné skutečné shared ownership je zatím nevyhodnoceno.

### Další procvičení

- DU02: samostatně navrhnout a otestovat algoritmus skládání matic a porovnat `T*R` proti `R*T`.
- DU02: rozhodnout životní cyklus složených transformací a navrhnout aktualizaci dynamické rotace; následně teprve implementovat.

- U dalšího vertex layoutu samostatně určit počet hodnot, velikost v bajtech, stride a offsety.
- U dalšího resource-owning typu samostatně odvodit ownership, konstrukční závislosti, copy/move politiku a pořadí destrukce.
- Později nahradit `exit(EXIT_FAILURE)` propagací chyby kompatibilní s RAII.
- Až vznikne více objektů, znovu posoudit potřebu `DrawableObject` a `Scene` podle skutečných odpovědností.

## Generalizovatelné programovací a návrhové principy

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

## Aktuální architektonický stav

Pro první domácí úkol je pracovní minimum:

`main → Application → unique ownership Model + ShaderProgram; Shader je krátkodobý konstrukční prostředek`

Pro DU01 byly `Scene`, `DrawableObject` a transformace záměrně odloženy. DU02 nyní přinesl důvod tyto odpovědnosti navrhovat; zatím jde o **teoretický návrh**, nikoli implementované vrstvy. Pracovní směr: `DrawableObject` používá model, shader program a transformaci; polymorfní `Transformation` poskytuje matici a `CompositeTransformation` ji skládá. Ownership zatím není definitivně uzavřen.

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

Checkpoint se v učitelském chatu spouští slovem:

`checkpoint`

Podrobná pravidla jsou v `project/AGENTS.md`.
