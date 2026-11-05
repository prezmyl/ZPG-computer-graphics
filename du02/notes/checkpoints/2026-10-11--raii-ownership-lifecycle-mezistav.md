# Learning checkpoint – RAII, ownership a životní cyklus (mezistav)

**Datum:** 2026-10-11  
**Navazuje na code commit:** `be10131f2803370957851835478ee517ff6cc67b`

## Kontext etapy

Tato etapa ještě není dokončená. Cílem mezicheckpointu je zachytit několik zásadních C++ principů dříve, než se propojí do finálního lifecycle řešení v `Application`.

Aktuálně:

- `Model` uvolňuje VAO/VBO ve svém destruktoru;
- `Shader` uvolňuje shader ID;
- `ShaderProgram` uvolňuje program ID;
- u všech tří resource-owning tříd je zakázán copy constructor a copy assignment;
- `Application` má připravené `std::unique_ptr<Model>` a `std::unique_ptr<ShaderProgram>`, protože GPU objekty musí vzniknout až po vytvoření OpenGL contextu;
- skutečné vytvoření, pořadí destrukce a cleanup window/contextu ještě nejsou dokončeny.

## Samostatně prokázáno

- Po vysvětlení problému s kopírováním OpenGL handle byly u `Model`, `Shader` a `ShaderProgram` korektně přidány deleted copy constructor a deleted copy assignment.
- V `Application.h` byly korektně použity forward declarations pro `Model` a `ShaderProgram` spolu s `std::unique_ptr`.
- `Application` nyní explicitně deklaruje vlastnictví dlouhodobých `Model` a `ShaderProgram` objektů.

## Pochopeno s vedením

- Základ RAII: objekt, který vlastní externí resource, má jeho získání a uvolnění svázat se svým vlastním životním cyklem.
- Prosté zkopírování OpenGL ID nevytvoří druhý GPU resource; vytvořilo by dva C++ vlastníky jednoho resource a hrozil by dvojí cleanup.
- `unique_ptr` zde není použit „protože smart pointer“, ale protože řeší konkrétní potřebu:
  - jednoznačné vlastnictví,
  - možnost odložené konstrukce po vytvoření contextu,
  - řízenou destrukci před zánikem contextu.
- Životnost GPU resource závisí na životnosti OpenGL contextu; nestačí tedy pouze mít destruktor, důležité je i pořadí destrukce.

## Vývoj pochopení

Původně se cleanup řešil jako závěrečný blok procedurálního `main()`. Po rozdělení do objektů už nestačí přepsat `glDelete*` do destruktorů. Musí být jasné:

1. kdo resource vlastní,
2. zda může být vlastník kopírován,
3. kdy resource vzniká,
4. na jakém jiném prostředí je jeho platnost závislá,
5. v jakém pořadí musí objekty zaniknout.

Tím se cleanup mění z „posledních několika řádků programu“ na součást návrhu ownershipu a životního cyklu.

## Generalizovatelné poučení / nejvyšší abstrakce

### RAII není jen automatický cleanup

RAII znamená, že vlastnictví resource je vyjádřeno životním cyklem objektu. Nestačí však pouze napsat destruktor.

Je nutné zároveň řešit:

- zda lze vlastníka kopírovat,
- zda lze ownership přesouvat,
- na jakých jiných resources vlastník závisí,
- zda tyto závislosti přežijí jeho destrukci.

Obecná otázka:

> Kdo resource vlastní a jaké podmínky musí stále platit ve chvíli, kdy jej vlastník uvolňuje?

### Resource-owning typ musí vědomě řešit speciální členské funkce

Pokud třída vlastní externí resource a má vlastní destruktor, nelze automaticky předpokládat, že implicitní kopírování má správný význam.

To je praktický vstup do pravidel **Rule of Three / Rule of Five / Rule of Zero**.

V aktuálním projektu bylo zvoleno jednoduché řešení:

```cpp
T(const T&) = delete;
T& operator=(const T&) = delete;
```

Tím se zabrání tomu, aby dva C++ objekty omylem vlastnily stejné OpenGL ID.

### Volba pointeru má vycházet z ownershipu a životního cyklu

`std::unique_ptr` zde vyjadřuje:

> Tento objekt má právě jednoho vlastníka, ale nemusí existovat po celou dobu života vlastníka.

To řeší případ `Application`:

```text
Application vznikne
    ↓
vznikne OpenGL context
    ↓
teprve potom Model / ShaderProgram
    ↓
GPU resources zaniknou
    ↓
až potom zanikne context
```

Volba value/reference/raw pointer/smart pointer má být důsledkem životního cyklu a ownershipu, ne stylistická preference.

### Závislosti mohou určovat pořadí destrukce

Objekt může vlastnit svůj resource, ale přesto při jeho destrukci potřebovat jiný živý systém.

`Model::~Model()` potřebuje platný OpenGL context, aby mohl volat `glDeleteBuffers` a `glDeleteVertexArrays`.

Proto je třeba přemýšlet nejen o vztahu:

```text
Application owns Model
```

ale také:

```text
Model resource depends-on OpenGL context
```

Ownership a dependency nejsou totéž, ale oba vztahy ovlivňují lifecycle.

### Forward declaration s owning pointerem má hranici

Forward declaration stačí v hlavičce, pokud je typ používán přes pointer/reference. U `std::unique_ptr<T>` ale defaultní deleter potřebuje kompletní `T` v místě, kde se destruktor vlastníka skutečně instancuje.

Proto je vhodné:

```cpp
// Application.h
class Model;
std::unique_ptr<Model> model;
~Application();
```

a definovat `~Application()` v `.cpp`, kde už jsou `Model.h` a `ShaderProgram.h` skutečně includované.

To je obecný příklad rozdílu mezi tím, co stačí **deklarovat v rozhraní**, a tím, co musí být **kompletně známé v implementaci**.

## Aktuální otevřený technický bod

`Application.cpp` nyní definuje:

```cpp
Application::~Application() {
}
```

ale zatím neincluduje `Model.h` ani `ShaderProgram.h`. Proto jsou tyto typy v místě definice destruktoru stále neúplné. To je další krok k opravě před pokračováním v lifecycle implementaci.

## Potřebuje upevnit

- Samostatně odvodit správné pořadí destrukce `Model`, `ShaderProgram`, `Shader`, window a OpenGL contextu.
- Prakticky dokončit `Application::~Application()`.
- Prakticky vytvořit GPU objekty až po úspěšném vytvoření contextu.
- Rozlišovat ownership od dependency i u složitějších vztahů.
- Move semantics jsou zatím záměrně neřešeny.

## Další procvičení

1. Doplnit úplné typy do `Application.cpp`.
2. Navrhnout pořadí cleanupu v `Application::~Application()` ještě před napsáním kódu.
3. Vytvořit `Model` a `ShaderProgram` až po inicializaci OpenGL.
4. Ověřit, proč mohou být jednotlivé `Shader` objekty kratšího života než výsledný `ShaderProgram`.

## Důkazy

- `972eec5` – destruktory `Model`, `Shader` a `ShaderProgram`.
- `7e3a6e9` – resource-owning třídy mají zakázané kopírování.
- `be10131` – `Application` dostává `unique_ptr<Model>`, `unique_ptr<ShaderProgram>`, forward declarations a out-of-line destruktor.
