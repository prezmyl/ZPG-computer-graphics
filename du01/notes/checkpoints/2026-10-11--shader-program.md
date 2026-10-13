# Learning checkpoint – ShaderProgram a převod procedurálního kódu do objektového návrhu

**Datum:** 2026-10-11  
**Navazuje na code commit:** `00a62401ea1b9cb4cde1f05861e46d1f735dfd9f`

## Kontext etapy

Po oddělení `Shader` byla další souvislá část původního procedurálního `cv02/main.cpp` převedena do třídy `ShaderProgram`.

Aktuální odpovědnost `ShaderProgram` je:

- vytvořit OpenGL program,
- použít dva již zkompilované `Shader` objekty jako vstupy,
- připojit je pomocí `glAttachShader`,
- program slinkovat,
- ověřit výsledek linkování,
- uchovat výsledné program ID,
- umět program aktivovat pomocí `use()`.

Třída neřeší načítání GLSL souborů ani kompilaci jednotlivých shaderů; to už je odpovědnost `Shader`.

## Samostatně prokázáno

- Procedurální blok pro vytvoření a linkování programu byl prakticky přesunut do samostatné implementace `ShaderProgram.cpp`.
- Po upozornění na hranice odpovědností nebylo do `ShaderProgram` přeneseno ničení GLFW okna ani ukončování GLFW.
- Poslední úprava odstranila zbytečné `this->`, přidala kontrolu selhání `glCreateProgram`, odstranila nepotřebnou závislost na GLFW a nastavila `use()` jako `const`.

Poznámka k hodnocení: klíčový návrh rozhraní `ShaderProgram(const Shader&, const Shader&)` a principy ownershipu byly dodány s vedením, proto je nepovažuji zatím za samostatně prokázaný návrh.

## Pochopeno s vedením

- Rozdíl mezi **kompilací jednoho shaderu** a **linkováním více shaderů do shader programu**.
- `ShaderProgram` potřebuje při konstrukci existující `Shader` objekty, ale nemusí je vlastnit ani kopírovat; proto jsou vhodné `const Shader&`.
- Program ID je trvalý stav objektu, zatímco `linkSuccess` a `infoLog` jsou pouze dočasná data linkování.
- `use() const` může měnit OpenGL stav, přesto je metoda z pohledu C++ objektu `const`, protože nemění jeho členský stav.
- `this->id` i `id` jsou zde korektní; `this->` není nutné bez kolize jmen.
- `ShaderProgram` nemá ničit GLFW window/context, protože je nevlastní.
- Při neúspěšném linkování může uklidit program, který sám vytvořil; životní cyklus ostatních prostředků patří jejich vlastníkům.

## Vývoj pochopení

Výchozí procedurální kód měl kompilaci shaderů, linkování programu, práci s oknem i cleanup na jednom místě. Při převodu do OOP nebyl kód rozdělen pouze mechanicky podle skupin řádků, ale podle odpovědností a životních cyklů.

`Shader` nyní reprezentuje jeden zkompilovaný shader. `ShaderProgram` reprezentuje jiný OpenGL prostředek vznikající linkováním shaderů. `Application` zůstává vlastníkem/koordinátorem okna a contextu.

Důležité bylo rozlišit, že skutečnost, že `ShaderProgram` nějaký prostředek **používá**, ještě neznamená, že ho **vlastní**.

## Generalizovatelné poučení / nejvyšší abstrakce

### Jak převádět existující procedurální kód do objektového návrhu

Při refaktoringu není dobrý postup „vezmu blok řádků a udělám z něj třídu“. Užitečnější je postupovat od významu:

1. **Najdi soudržnou odpovědnost.**  
   Ptej se, které operace řeší jeden problém a mění se ze stejného důvodu.

2. **Urči vstupy a výstup odpovědnosti.**  
   Zde jsou vstupy dva zkompilované `Shader` objekty a výsledkem je slinkovaný OpenGL program.

3. **Odděl závislosti od vlastnictví.**  
   Pokud objekt jiný objekt pouze používá během operace, nemusí jej vlastnit. To se může v C++ projevit např. referencí.

4. **Odděl trvalý stav od dočasných dat algoritmu.**  
   Program ID musí přežít konstruktor → člen objektu.  
   Link status a error log existují pouze během linkování → lokální proměnné.

5. **Odvoď minimální veřejné rozhraní z toho, co okolí skutečně potřebuje.**  
   Prozatím stačí konstrukce programu a `use()`. Uniformy nebo reload se nepřidávají dříve, než vznikne požadavek.

6. **Zkontroluj hranice cleanupu.**  
   Třída nemá uklízet prostředky jen proto, že je používá. Cleanup má následovat ownership a životní cyklus.

7. **Odstraň nepotřebné závislosti.**  
   Pokud `ShaderProgram` nepotřebuje GLFW, nemá mít důvod includovat GLFW ani znát okno.

Obecný mentální postup:

```text
procedurální kód
      ↓
co je jeden problém / důvod ke změně?
      ↓
odpovědnost
      ↓
vstupy a závislosti
      ↓
vlastním je, nebo jen používám?
      ↓
co musí přežít operaci?
      ↓
minimální veřejné chování
      ↓
hranice životního cyklu a cleanupu
      ↓
třída / objekt
```

Tento postup je použitelný i mimo OpenGL při refaktoringu databázového, síťového, souborového nebo aplikačního procedurálního kódu.

### Použití není vlastnictví

Závislost na jiném objektu sama o sobě neříká nic o ownershipu.

`ShaderProgram` během konstrukce používá `Shader`, ale neznamená to, že má právo tyto objekty mazat. Naopak vlastní své program ID, takže cleanup tohoto prostředku je jeho odpovědnost.

Obecná otázka u každé závislosti:

> Potřebuji tento objekt pouze použít, potřebuji na něj dlouhodobě odkazovat, nebo vlastním jeho životní cyklus?

Teprve z odpovědi má vycházet hodnota, reference, pointer nebo smart pointer.

### Const-correctness popisuje stav C++ objektu

Metoda může mít vedlejší účinek mimo objekt a přesto být `const`, pokud nemění jeho logický členský stav.

`ShaderProgram::use() const` mění aktivní OpenGL program, ale nemění `ShaderProgram::id`.

## Potřebuje upevnit

- Životní cyklus úspěšně vytvořeného `ShaderProgram`: zatím chybí destruktor s `glDeleteProgram`.
- Životní cyklus jednotlivých `Shader` objektů po úspěšném linkování programu.
- RAII a správné pořadí destrukce OpenGL prostředků vůči existenci platného contextu.
- Samostatné odvození vhodného typu vazby (hodnota/reference/pointer/smart pointer) z ownershipu a životnosti zatím není plně prokázáno.
- Robustnější propagace chyb místo `exit(EXIT_FAILURE)` zatím nebyla řešena.
- V `ShaderProgram.h` by bylo možné snížit závislost na `Shader.h` pomocí forward declaration, protože jsou používány pouze reference; to je nyní pouze možné zlepšení, ne chyba.

## Další procvičení

1. Rozebrat životní cyklus `Shader`, `ShaderProgram`, `Model`, okna a OpenGL contextu a určit správné pořadí destrukce.
2. Doplnit RAII cleanup tak, aby prostředek uvolňoval jeho vlastník.
3. Při další třídě nejdřív samostatně projít: odpovědnost → závislosti → ownership → stav → rozhraní.
4. Při dalším refaktoringu procedurálního bloku nejprve popsat jeho vstupy, výstupy, trvalý stav a hranice odpovědnosti ještě před psaním hlavičky.

## Důkazy

- Commit `d974064` – první návrh rozhraní `ShaderProgram`.
- Commit `f5be93d` – první implementace vytvoření, attach, link a kontroly programu.
- Commit `00a6240` – odstranění závislosti na GLFW, kontrola `glCreateProgram`, cleanup vlastního programu při chybě linkování a `use() const`.
- V diskusi byla explicitně řešena otázka, proč `ShaderProgram` nemá volat `glfwDestroyWindow` ani `glfwTerminate`: tyto prostředky nevlastní.
