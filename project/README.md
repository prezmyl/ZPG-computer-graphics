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

- Rozlišení „koordinátor rozhoduje co/kdy“ vs. „specializovaný objekt ví jak“.
- Forward declaration pro `GLFWwindow*` a omezení závislostí hlaviček.
- Rozdíl mezi lokální a členskou proměnnou na příkladu zastínění `window`.
- VBO jako OpenGL handle, nikoli velikost dat.
- Ztráta informace o velikosti C pole po převodu na pointer a důsledky pro `sizeof(points)`.
- Použití `const float*` a member initializer listu v konstruktoru `Model`.

### Potřebuje upevnit

- Převod mezi počtem vrcholů, počtem atributových hodnot a velikostí dat v bajtech. Aktuální `Model.cpp` stále počítá velikost VBO dat neúplně.
- RAII a destrukce OpenGL prostředků v návaznosti na životní cyklus contextu.
- Praktické rozdělení `Shader` a `ShaderProgram`, jejich ownership a životní cyklus.
- Uniformy: rozdíl mezi location uloženou vůči programu a významovou hodnotou vlastněnou objektem/scénou.
- Konkrétní C++ reprezentace sdíleného vlastnictví modelů a shader programů je zatím nevyhodnocena.

### Další procvičení

- Opravit a samostatně vysvětlit výpočet velikosti VBO pro pevný layout 6 `float`/vertex.
- Implementovat `Shader` a `ShaderProgram` bez přesunu jejich odpovědností zpět do `Application`.
- Doplnit korektní uvolnění VAO/VBO a obhájit pořadí destrukce vůči OpenGL contextu.
- Až vznikne více objektů, znovu posoudit potřebu `DrawableObject` a `Scene` podle skutečných odpovědností.

## Aktuální architektonický stav

Pro první domácí úkol je pracovní minimum:

`main → Application → Model + budoucí ShaderProgram`

`Scene`, `DrawableObject`, transformace a další vrstvy jsou záměrně odložené, dokud pro ně nevznikne praktická potřeba.

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

## Learning checkpointy

Datované checkpointy jsou v:

`project/notes/checkpoints/`

Aktuálně:
- `2026-10-09--application-a-model.md`

Checkpoint se v učitelském chatu spouští slovem:

`checkpoint`

Podrobná pravidla jsou v `project/AGENTS.md`.
