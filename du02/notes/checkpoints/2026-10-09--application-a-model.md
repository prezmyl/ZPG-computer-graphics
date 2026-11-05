# Learning checkpoint – Application a první návrh Modelu

**Datum:** 2026-10-09  
**Navazuje na code commit:** `4752c25b74db7782e020f35809672e4659472039`

## Kontext etapy

Cílem etapy bylo přestat rozšiřovat monolitický `main.cpp` z letošního `cv02` a začít první domácí úkol stavět jako malou objektovou aplikaci. Vědomě jsme odložili `Scene`, `DrawableObject`, transformace a další architekturu, která pro aktuální DU1 zatím nemá dostatečný důvod.

První praktický mezistav je:

- `main()` pouze vytvoří `Application`, zavolá inicializaci a `run()`;
- `Application` převzala inicializaci GLFW/GLAD, okno, callbacky a hlavní render loop;
- vznikla první verze `Model`, která má reprezentovat GPU geometrii pomocí VBO, VAO a počtu vrcholů;
- `ShaderProgram` je zatím pouze připravená prázdná třída a nebyl v této etapě hodnocen.

## Samostatně prokázáno

- Dokázal jsem rozlišit několik odpovědností původního monolitického programu: inicializaci aplikace, geometrii (VBO/VAO), jednotlivé shadery, shader program, vykreslení jednoho objektu, vykreslení celé scény a zpracování událostí.
- Správně jsem určil, že VAO patří ke geometrickému modelu, zatímco `modelMatrix` popisuje konkrétní instanci objektu ve scéně.
- Správně jsem odvodil, že stejnou geometrii i shader program může používat více objektů, zatímco transformační stav musí být pro jednotlivé instance oddělený.
- U `glUseProgram` jsem rozlišil dvě úrovně odpovědnosti: vyšší objekt rozhoduje, kdy se program použije, zatímco samotný `ShaderProgram` může znát mechanismus své aktivace.
- Pro první domácí úkol jsem se rozhodl nepřidávat `Scene` předčasně a nechat ji až pro další etapu, kde bude mít skutečnou odpovědnost.
- U `Model::draw()` jsem samostatně rozpoznal, že `vertexCount` je členský stav objektu a metoda jej nemusí dostávat znovu jako parametr.

## Pochopeno s vedením

- Rozdíl mezi tím, **kde se OpenGL operace volá**, a tím, **který objekt za její detail odpovídá**. Například render loop může rozhodnout, že se má model vykreslit, ale `Model` může znát vlastní VAO a počet vrcholů.
- Význam forward declaration `struct GLFWwindow;` v `Application.h`: hlavička potřebuje úplnou definici typu až tehdy, když by objekt ukládala hodnotou; pro pointer stačí deklarace typu.
- Rozdíl mezi lokální proměnnou a členskou proměnnou byl upevněn na chybě se zastíněním `GLFWwindow* window` uvnitř `Application::initialize()`.
- U návratového typu `bool` bylo potřeba vedení k tomu, že `EXIT_FAILURE` je nenulová hodnota a tedy se převádí na `true`; v metodě `initialize()` je proto vhodné explicitně vracet `false`.
- U `Model` bylo s vedením vyjasněno, že `VBO` je OpenGL handle/ID, nikoli velikost bufferu.
- Bylo potřeba vedení k pochopení, že po předání C pole jako `const float*` už `sizeof(points)` neudává velikost původního pole, ale pouze velikost pointeru.
- Konstruktor `Model(const float*, GLsizei)` a použití member initializer listu byly zavedeny s vedením.

## Vývoj pochopení

Na začátku jsem uvažoval hlavně podle pořadí OpenGL příkazů v render loopu. Postupně se mentální model změnil směrem k odpovědnostem: VBO/VAO nejsou jen příkazy, které je potřeba někam přesunout, ale stav geometrického prostředku; shader program je jiný prostředek s jiným důvodem ke změně; vyšší vrstva koordinuje jejich použití.

Důležitý posun nastal také u sdílení. Původně nebylo vlastnictví geometrie a transformací ustálené. Během diskuse jsem správně rozlišil sdílenou geometrii od per-instance transformačního stavu. Toto rozlišení bude později důležité pro vztah `DrawableObject` – `Model` – transformace.

Při implementaci `Model` se ukázalo, že koncept „počet vrcholů“ ještě není plně propojen s fyzickou velikostí dat ve VBO. `vertexCount` je správně použit pro `glDrawArrays`, ale aktuální výpočet velikosti nahrávaných dat je stále chybný.

## Potřebuje upevnit

- **Počet vrcholů vs. počet prvků vs. počet bajtů.** Aktuální `Model.cpp` počítá `dataSize = vertexCount * sizeof(float)`, přestože současný layout obsahuje 6 `float` na jeden vertex. Pro 6 vertexů tedy počet vrcholů není totéž jako počet `float` hodnot.
- **Vlastnictví a životní cyklus OpenGL prostředků.** `Model` vytváří VBO a VAO, ale zatím nemá destruktor, který by je uvolnil. RAII proto zatím není samostatně prokázáno.
- **Shader / ShaderProgram.** Odpovědnosti byly diskutovány, ale zatím nebyly implementovány ani samostatně obhájeny v kódu.
- **Uniformy.** Je pochopeno, že location se hledá vůči slinkovanému programu, ale rozdělení mezi „program umí uniform nastavit“ a „konkrétní objekt vlastní významovou hodnotu uniformu“ zatím nebylo prakticky ověřeno.
- **Vlastnictví sdílených objektů v C++.** Konceptuálně bylo rozpoznáno sdílení geometrie a shader programu, ale konkrétní reprezentace pomocí hodnot, referencí nebo smart pointerů je zatím nevyhodnocena.

## Další procvičení

1. Opravit výpočet velikosti dat pro VBO a vlastními slovy vysvětlit rozdíl mezi `vertexCount`, počtem `float` hodnot a velikostí v bajtech.
2. Dokončit `Shader` a `ShaderProgram` tak, aby bylo možné vysvětlit jejich rozdílné životní cykly a odpovědnosti.
3. Přidat destrukci VBO/VAO a vysvětlit, proč musí proběhnout v době, kdy existuje platný OpenGL context.
4. Po přidání více objektů znovu rozhodnout, kdy skutečně vzniká potřeba `Scene` a `DrawableObject`.
5. U dalšího návrhu samostatně určit, kdo objekt vlastní a kdo jej pouze používá.

## Důkazy

- `c8a819f` – oprava CMake targetu a zprovoznění samostatné `Application`.
- `157a569` – přidání první kostry `Model` a jeho členů VBO, VAO a `vertexCount`.
- `8357d8b` – přesun VBO/VAO setupu do konstruktoru `Model`, použití `const float*` a implementace `draw()`.
- `4752c25` – přechod na member initializer list a pokus nahradit chybný `sizeof(points)` explicitním `dataSize`; výpočet je však stále neúplný, protože nezahrnuje 6 hodnot na vertex.
- V diskusi jsem samostatně určil, že VAO patří geometrii, transformace konkrétnímu objektu a že geometrie i shader mohou být sdílené.
