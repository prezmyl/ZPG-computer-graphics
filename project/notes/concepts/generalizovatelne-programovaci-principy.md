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
