# Learning checkpoint – funkční OOP render pipeline a dokončení lifecycle etapy

**Datum:** 2026-10-11  
**Navazuje na code commit:** `1717f0138bd5440a2c2d9c2fc735f78df36e679d`

## Kontext etapy

Tento checkpoint uzavírá etapu navazující na mezicheckpoint `2026-10-11--raii-ownership-lifecycle-mezistav.md`.

Aktuální aplikace už:

- vytvoří GLFW window a OpenGL context,
- načte GLAD,
- až poté vytvoří GPU resources,
- vytvoří `Model`, dva dočasné `Shader` objekty a dlouhodobý `ShaderProgram`,
- po linkování nechá jednotlivé `Shader` objekty zaniknout,
- v render loopu používá pouze `shaderProgram->use()` a `model->draw()`,
- při zániku `Application` odstraní GPU resources dříve než window/context,
- buildí a vykresluje barevný čtverec.

## Samostatně prokázáno

- Bylo správně rozpoznáno, že jednotlivé `Shader` objekty jsou potřeba jen při konstrukci/linkování programu a po úspěšném linkování mohou zaniknout.
- Při návrhu `createResources()` bylo správně určeno, že `ShaderProgram` musí vzniknout až po vertex a fragment shaderu; `Model` je na této shaderové závislosti nezávislý.
- `createResources()` bylo implementováno tak, že dlouhodobé objekty jsou uloženy do `unique_ptr`, zatímco shadery zůstávají lokální.
- Render loop byl převeden z přímých OpenGL detailů na objektová rozhraní:
  `shaderProgram->use(); model->draw();`.
- CMake byl doplněn tak, aby kopíroval shader soubory do build adresáře.

## Pochopeno s vedením

- Cleanup v `Application::~Application()` musí explicitně zničit GPU objekty před `glfwDestroyWindow()`, protože jejich destruktory ještě potřebují platný OpenGL context.
- RAII umožňuje odstranit duplicitní cleanup z chybových větví `initialize()` a soustředit cleanup do vlastníků.
- `GLAD_GL_IMPLEMENTATION` je preprocesorové makro aktivní až do `#undef` nebo konce translation unit. U header-only GLAD proto opětovné includování `glad/gl.h` se stále definovaným makrem vedlo k redefinition chybám.
- Forward declaration + `std::unique_ptr<T>` má hranici: speciální členské funkce `Application`, které mohou potřebovat destrukci členů, je bezpečné generovat out-of-line v `.cpp`, kde jsou `Model` a `ShaderProgram` kompletní typy.
- Proto bylo potřeba explicitně deklarovat `Application();` v hlavičce a definovat `Application::Application() = default;` v `Application.cpp`.

## Vývoj pochopení

Na začátku této etapy byly jednotlivé OpenGL resources už rozdělené do tříd, ale jejich životní cyklus nebyl ještě propojen s životním cyklem contextu.

Postupně se ukázalo, že architektura není jen rozdělení funkcí do tříd. Musí současně respektovat:

1. kdo co vlastní,
2. na čem vlastník závisí,
3. kdy může resource vzniknout,
4. kdy musí zaniknout,
5. které objekty jsou dočasné a které dlouhodobé,
6. jaké detaily mají zůstat skryté za rozhraním.

Výsledkem je tok:

```text
Application
    ↓ initialize
GLFW + window + context + GLAD
    ↓
createResources
    ├─ Model -------------------------┐
    ├─ vertex Shader --┐              │
    ├─ fragment Shader ├→ ShaderProgram
    └──────────────────┘              │
        shadery zaniknou              │
                                      ↓
run
    shaderProgram->use()
    model->draw()
                                      ↓
~Application
    ShaderProgram + Model zaniknou
    ↓
    window/context zanikne
```

## Generalizovatelné poučení / nejvyšší abstrakce

### Pořadí konstrukce odvozuj ze závislostí

Pořadí objektů nemá být náhodné ani pouze podle čitelnosti kódu.

Pokud `B` potřebuje `A` během konstrukce, platí:

```text
A musí existovat
↓
vznikne B
```

Stejná logika často funguje opačně při destrukci:

```text
B zanikne
↓
až potom může zaniknout A, pokud na něm B při cleanupu závisí
```

V tomto projektu:

- `ShaderProgram` při konstrukci závisí na `Shader`;
- GPU resources při konstrukci i destrukci závisí na OpenGL contextu.

### Rozsah platnosti je nástroj pro modelování životnosti

Objekt nemusí být členská proměnná jen proto, že je důležitý při inicializaci.

Pokud jej po skončení jedné fáze už nepotřebujeme, lokální scope je přesnější vyjádření životnosti.

Proto jsou:

- `Shader vertexShader` a `Shader fragmentShader` lokální v `createResources()`;
- `Model` a `ShaderProgram` jsou členy `Application`.

### Vyšší vrstva má pracovat s významem, ne s implementačními detaily

Render loop už nepracuje s:

- VAO,
- program ID,
- `glUseProgram`,
- `glDrawArrays`.

Místo toho používá:

```cpp
shaderProgram->use();
model->draw();
```

To je praktický příklad zapouzdření: `Application` koordinuje **co a kdy**, specializované objekty řeší **jak**.

### Odděluj fáze podle rozdílných důvodů změny

`initialize()` řeší infrastrukturu:

- GLFW,
- window,
- context,
- GLAD,
- callbacks.

`createResources()` řeší obsah renderovací aplikace:

- model,
- shadery,
- shader program.

Tyto části se mohou měnit z jiných důvodů, proto dává smysl je oddělit už na úrovni metod, aniž by bylo nutné zavádět další třídu.

### Chyby buildu mohou odhalit hranice abstrakce

Dvě chyby v této etapě nebyly náhodné:

- redefinition v GLAD ukázala skutečný rozsah preprocesorového makra;
- incomplete type u `unique_ptr` ukázal rozdíl mezi deklarací typu v rozhraní a místem, kde musí být typ kompletní.

Obecný přístup k takové chybě:

1. najít první konkrétní typ/funkci, na které compiler padá;
2. sledovat include/instantiation řetězec;
3. určit, zda jde o chybu návrhu, lifetime, preprocesoru nebo pouze syntaxe;
4. opravit příčinu, ne pouze potlačit symptom.

## Potřebuje upevnit

- Tyto lifecycle a ownership principy byly z velké části zavedeny s vedením; je potřeba je později samostatně aplikovat na další resource-owning třídu.
- Move semantics pro resource-owning typy zůstávají zatím neřešené.
- `Shader` a `ShaderProgram` při chybách stále používají `exit(EXIT_FAILURE)`; to obchází běžné stack unwinding a není ideální pokračování čistého RAII návrhu.
- Vertex layout je stále pevně svázaný s `position.xyz + color.rgb`; zobecnění layoutu zatím není cílem DU1.
- `points` jsou zatím globální data v `Application.cpp`; umístění modelových dat je další možný návrhový krok, ne blocker tohoto checkpointu.

## Další procvičení

- U dalšího OpenGL resource samostatně určit konstrukční závislosti, ownership, potřebnou copy/move semantiku a správný cleanup.
- Později nahradit `exit()` způsobem propagace chyby, který zachová RAII.
- Při přidání více objektů znovu posoudit, zda `Application` nezačíná nést příliš mnoho odpovědností a zda už vzniká skutečná potřeba `Scene` nebo jiné vrstvy.

## Důkazy

- `af43036` – explicitní cleanup GPU resources před zničením window/contextu a centralizace cleanupu.
- `490c35e` – vznik `createResources()`, dlouhodobý `Model` a `ShaderProgram`, lokální shadery.
- `f3a5d7d` – render loop používá `shaderProgram->use()` a `model->draw()`; shader directory je kopírován přes CMake.
- `4bab05e` – oprava opakované GLAD implementace pomocí `#undef GLAD_GL_IMPLEMENTATION`.
- `1717f01` – out-of-line default konstruktor `Application`; build i běh aplikace jsou funkční.
- Výsledek byl v diskusi ověřen vykresleným barevným čtvercem.
