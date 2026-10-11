# Odpovědnosti OpenGL objektů v aktuálním návrhu

Tato poznámka zachycuje pracovní mentální model používaný při refaktoringu prvního domácího úkolu. Není to konečná architektura enginu.

## Application

`Application` řídí život aplikace na nejvyšší úrovni:

- inicializace GLFW a GLAD,
- vytvoření okna a OpenGL contextu,
- registrace jednoduchých callbacků,
- hlavní render loop,
- zpracování událostí a prezentace framebufferu.

Důležité rozlišení: to, že `Application` rozhoduje **kdy** se něco vykreslí, neznamená, že musí znát detaily **jak** se vytváří VBO, VAO nebo shader.

## Model

V aktuálním DU1 je `Model` GPU reprezentace geometrie:

- vlastní VBO,
- vlastní VAO,
- zná počet vrcholů,
- při vytvoření nahraje vertexová data,
- `draw()` bindne vlastní VAO a zavolá draw call.

Pracovní layout je zatím pevně 6 `float` hodnot na vertex:

`position.xyz + color.rgb`

Toto je vědomé omezení DU1, ne obecný návrh pro všechny budoucí modely.

## Tři různé velikosti

Je nutné rozlišovat:

- **vertexCount** – kolik vrcholů se vykresluje;
- **počet hodnot** – při současném layoutu `vertexCount * 6`;
- **velikost v bajtech** – `vertexCount * 6 * sizeof(float)`.

`glDrawArrays(..., vertexCount)` pracuje s počtem vrcholů.  
`glBufferData(..., dataSize, ...)` potřebuje velikost dat v bajtech.

## VBO a VAO

- **VBO** je GPU buffer obsahující vertexová data. Hodnota uložená v `GLuint VBO` je pouze OpenGL handle/ID.
- **VAO** uchovává konfiguraci vertexových atributů, tedy jak mají být data z bufferů interpretována při kreslení.

VAO proto patří ke geometrické reprezentaci, ne k shader programu. Mezi VAO a vertex shaderem ale existuje kontrakt: atributové indexy a jejich význam musí odpovídat vstupům shaderu.

## Shader a ShaderProgram

Aktuální pracovní rozdělení:

- `Shader` je už implementovaný a reprezentuje jeden kompilovaný shader (např. vertex nebo fragment);
- jeho trvalým stavem je OpenGL shader ID; typ shaderu, cesta k souboru, source code, compile status a log jsou jen dočasné údaje potřebné při konstrukci;
- `ShaderProgram` je implementovaný jako slinkovaný GPU program sestavený ze dvou `Shader` objektů;
- při konstrukci shadery pouze používá přes `const&`; neukládá je jako svůj stav ani zatím nepřebírá jejich ownership;
- `ShaderProgram` uchovává vlastní program ID a zná mechanismus `use()`; práce s uniform locations přijde později;
- vyšší vrstva rozhoduje, kdy má být konkrétní program aktivován.

Uniform location patří ke konkrétnímu slinkovanému programu. Významová hodnota typu `modelMatrix` ale typicky popisuje konkrétní objekt, nikoli samotný shader program.

## Sdílený prostředek vs. stav instance

Více objektů ve scéně může používat:

- stejný `Model`,
- stejný `ShaderProgram`.

Každý objekt ale může mít vlastní:

- pozici,
- rotaci,
- měřítko,
- výslednou modelovou transformaci.

To je základní důvod, proč později vznikne potřeba odlišit geometrii od konkrétního vykreslovaného objektu. Pro DU1 však tato další vrstva zatím není nutná.

## Životní cyklus

Aktuální implementace už používá základní RAII:

- `Model` uvolňuje VAO/VBO ve svém destruktoru;
- `Shader` uvolňuje shader ID;
- `ShaderProgram` uvolňuje program ID;
- resource-owning typy mají zakázané kopírování, aby dva C++ objekty nevlastnily stejný OpenGL handle;
- `Application` vlastní dlouhodobý `Model` a `ShaderProgram` přes `std::unique_ptr`;
- jednotlivé `Shader` objekty jsou lokální pouze při linkování programu;
- `Application::~Application()` explicitně zničí GPU resources před zničením GLFW window/contextu.

Aktuální pořadí je:

```text
context vznikne
↓
Model + ShaderProgram vzniknou
↓
render loop
↓
ShaderProgram + Model zaniknou
↓
window/context zanikne
```

Toto pořadí je nutné, protože OpenGL cleanup funkce potřebují platný context.
