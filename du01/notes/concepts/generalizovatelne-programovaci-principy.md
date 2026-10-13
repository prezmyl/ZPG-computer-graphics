# Generalizovatelné programovací a návrhové principy

Tento soubor sbírá principy, které vznikly při práci na ZPG, ale mají význam i mimo OpenGL. Nejde o seznam pouček „pro jistotu“; každý bod má být podložen konkrétní zkušeností z projektu.

## Životnost informace určuje místo uložení

Při návrhu proměnné se ptej:

> Potřebuji tuto informaci i po skončení právě běžící operace?

- Pokud ano a charakterizuje stav konkrétního objektu, patří typicky do **členské proměnné**.
- Pokud je potřeba jen uvnitř jedné operace, patří typicky do **lokální proměnné**.
- Pokud ji pouze dodává volající, může zůstat **parametrem**, aniž by ji objekt ukládal.

Příklad z `Shader`:
- `id` musí přežít konstruktor → člen objektu;
- cesta k souboru, source code, compile status a error log jsou potřeba jen při konstrukci → dočasná data.

Cíl: objekt má uchovávat pouze stav, který skutečně potřebuje pro svůj další život.

## Koordinátor rozhoduje co a kdy, specializovaný objekt zná jak

Vyšší vrstva nemá nutně znát implementační detail každé operace.

Příklad:
- `Application` může rozhodnout, že se má model vykreslit;
- `Model` ví, jak bindnout své VAO a zavolat draw call;
- vyšší vrstva může rozhodnout, že se má použít shader program;
- `ShaderProgram` bude vědět, jak aktivovat vlastní OpenGL program.

Toto rozdělení snižuje coupling a drží odpovědnosti lokální.

## Odpovědnost má následovat důvod ke změně

Při rozhodování „kam tento kód patří?“ je užitečná otázka:

> Co by způsobilo, že se tato část kódu změní?

- změna vertex layoutu nebo geometrie → `Model`;
- změna kompilace jednoho shaderu → `Shader`;
- změna linkování nebo používání programu → `ShaderProgram`;
- změna řízení běhu aplikace → `Application`.

Objekt tak seskupuje věci, které se mění ze stejného důvodu.

## Stejný datový typ neznamená stejný význam

Více hodnot může být reprezentováno číslem, ale mohou mít zcela jiné jednotky a význam:

- počet vertexů,
- počet `float` hodnot,
- počet bajtů,
- OpenGL handle/ID.

Správný návrh a API vyžadují pracovat s jejich **významem**, ne jen s C++ typem.

## Abstrakce má řešit konkrétní problém

Nová vrstva nebo třída není automaticky lepší návrh.

`Scene` a `DrawableObject` byly v DU1 odloženy, protože zatím neměly dostatečnou samostatnou odpovědnost. Přidají se až ve chvíli, kdy více objektů, transformace nebo řízení scény vytvoří skutečný důvod jejich existence.


## Převod procedurálního kódu do objektového návrhu

Při refaktoringu procedurálního kódu nevytvářej třídy mechanicky podle délky nebo pozice bloků kódu. Postupuj podle významu:

1. najdi soudržnou odpovědnost a její důvod ke změně;
2. urč vstupy, výstupy a závislosti;
3. u každé závislosti rozhodni, zda ji objekt vlastní, dlouhodobě používá, nebo ji potřebuje jen během jedné operace;
4. odděl trvalý stav od lokálních pomocných dat;
5. z aktuálních požadavků odvoď minimální veřejné rozhraní;
6. zkontroluj, kdo odpovídá za vytvoření a zánik prostředků;
7. odstraň závislosti na subsystémech, které třída ke své odpovědnosti nepotřebuje.

Příklad: původní blok `glCreateProgram → glAttachShader → glLinkProgram → glUseProgram` se stal `ShaderProgram`, protože operace společně pracují s jedním slinkovaným programem. Naopak GLFW cleanup do něj nepatří, protože se mění z jiného důvodu a program okno nevlastní.

## Použití objektu není vlastnictví

To, že objekt potřebuje jiný objekt, automaticky neznamená, že jej vlastní.

Při návrhu vztahu se ptej:

> Potřebuji objekt jen během operace, potřebuji na něj odkazovat déle, nebo řídím jeho životní cyklus?

Tato odpověď má předcházet volbě mezi hodnotou, referencí, pointerem a smart pointerem.

V `ShaderProgram` jsou `Shader` objekty pouze vstupní závislosti pro linkování, proto jsou předány jako `const&`. Program nemá právo rozhodovat o jejich zániku.

## Cleanup následuje ownership, ne pouhou dostupnost

Fakt, že třída technicky dokáže nějaký prostředek zničit, není důvod, aby to dělala.

Cleanup má provádět vlastník prostředku ve správný okamžik jeho životního cyklu. `ShaderProgram` proto nemá při vlastní chybě ničit GLFW window/context; ty patří do jiné odpovědnosti.

## Const-correctness popisuje logický stav objektu

`const` metoda slibuje, že nemění pozorovatelný/logický stav daného C++ objektu. Nemusí nutně znamenat „nemá žádný vedlejší účinek nikde v systému“.

`ShaderProgram::use() const` mění globální OpenGL stav, ale nemění vlastní program ID ani jiný členský stav objektu.


## RAII je návrh ownershipu, ne pouze destruktor

RAII svazuje vlastnictví resource s životním cyklem C++ objektu. Samotný destruktor ale nestačí. U resource-owning typu je třeba řešit také kopírování, přesun, závislosti a pořadí destrukce.

Praktické otázky:

1. Kdo resource vytváří?
2. Kdo jej vlastní?
3. Smí být vlastník kopírován?
4. Na čem resource závisí?
5. Co musí při jeho destrukci ještě existovat?

OpenGL příklad: `Model` vlastní VAO/VBO, ale jejich cleanup vyžaduje stále platný OpenGL context.

## Resource-owning typ a Rule of Three/Five/Zero

Pokud typ vlastní externí resource a definuje vlastní destruktor, je nutné vědomě posoudit také copy/move operace.

Prosté zkopírování handle obvykle neznamená vytvoření nového resource. Pokud není definována korektní deep copy nebo transfer ownershipu, je bezpečnější kopírování zakázat.

Aktuální `Model`, `Shader` a `ShaderProgram` proto používají deleted copy constructor a copy assignment.

## Typ pointeru je důsledek ownershipu a životnosti

Pointer mechanismus se nemá volit podle zvyku.

- reference: typicky dočasné použití bez ownershipu;
- `unique_ptr`: právě jeden vlastník, možnost řídit okamžik vzniku a zániku;
- shared ownership má smysl pouze tehdy, pokud skutečně existuje více vlastníků.

`Application` používá `unique_ptr<Model>` a `unique_ptr<ShaderProgram>`, protože tyto objekty vlastní, ale mohou vzniknout až po vytvoření OpenGL contextu.

## Ownership a dependency jsou různé vztahy

Objekt může resource vlastnit, ale jeho použití nebo destrukce může záviset na jiném živém systému.

`Model` vlastní VBO/VAO, ale jejich OpenGL cleanup závisí na existujícím contextu.

Proto je při návrhu životního cyklu nutné sledovat obojí:

```text
kdo koho vlastní?
na kom je kdo funkčně závislý?
```

Z těchto dvou vztahů se často odvozuje správné pořadí konstrukce a destrukce.

## Forward declaration odděluje rozhraní od potřeby úplného typu

Forward declaration umožňuje v hlavičce pracovat s pointerem nebo referencí bez načtení celé definice typu. Implementace ale musí mít úplný typ ve chvíli, kdy potřebuje znát jeho velikost, členy nebo provést destrukci přes `std::unique_ptr`.

To je obecná technika pro omezení závislostí mezi hlavičkami: rozhraní ví jen tolik, kolik skutečně potřebuje; implementace si načte úplné definice.


## Pořadí konstrukce a destrukce odvozuj ze závislostí

Pokud objekt `B` potřebuje `A` při konstrukci nebo při cleanupu, musí tomu odpovídat jejich životní cyklus.

Obecně:

```text
A vznikne
↓
B může vzniknout
...
B zanikne
↓
A může zaniknout
```

V ZPG:

- `ShaderProgram` vzniká až po jednotlivých shaderech;
- OpenGL resources vznikají až po contextu a musí zaniknout před ním.

## Scope je součást návrhu životnosti

Lokální proměnná, členská proměnná a dynamicky vlastněný objekt nejsou jen syntaktické varianty. Vyjadřují, jak dlouho má daná věc existovat.

Pokud objekt potřebujeme pouze během jedné fáze, je lokální scope často nejpřesnější řešení.

Příklad: jednotlivé `Shader` objekty jsou lokální v `createResources()`, protože po linkování programu už nejsou potřeba. `ShaderProgram` a `Model` musí přežít render loop, proto jsou stavem `Application`.

## Odděluj fáze, které se mění z jiných důvodů

Nová odpovědnost nemusí automaticky znamenat novou třídu.

Nejdříve může stačit samostatná metoda, pokud pouze odděluje fázi práce se samostatným důvodem ke změně.

V `Application`:

- `initialize()` řeší infrastrukturu a OpenGL context;
- `createResources()` řeší konkrétní GPU obsah aplikace.

## Debugging compiler chyby sleduje řetězec závislostí

U komplikované compiler chyby nezačínej opravou posledního viditelného řádku. Sleduj:

1. který typ nebo symbol je skutečně problémový;
2. odkud byl includován nebo instancován;
3. jaký compile-time mechanismus je aktivní;
4. která předchozí návrhová volba tuto situaci vytvořila.

Příklady z projektu:

- redefinition GLAD symbolů vznikla kvůli přetrvávajícímu preprocesorovému makru v jednom translation unit;
- `sizeof` na incomplete type uvnitř `unique_ptr` vznikl při implicitním generování speciální členské funkce v místě, kde byl pointee typ pouze forward-declared.
