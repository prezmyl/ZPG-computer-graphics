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
