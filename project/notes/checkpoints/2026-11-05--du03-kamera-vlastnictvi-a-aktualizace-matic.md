# Learning checkpoint – DU03: kamera, kompozice se scénou a aktualizace matic

**Datum:** 2026-11-05  
**Navazuje na:** `2026-10-28--du02-slunecni-soustava-a-matice.md`  
**Kontrolovaný GitHub commit:** `3b218847cb64949c33ee4575c884aaa0c904b6b2`  
**Rozsah:** rozhovor 1.–5. 11. 2026, letošní požadavky cvičení 4–6 (Camera, procházení lesa, perspektiva, osvětlovací modely), zdrojové soubory `Camera.h/.cpp`, `Scene.h/.cpp`, `Application.cpp`, relevantní Git diffy.  
**Ověření:** pouze code review GitHubu a uživatelem zaslaný screenshot překladače. Asistent nekompiloval ani nespouštěl aktuální verzi; poslední úspěšný build či runtime test uživatel výslovně nepotvrdil.

## STAV – ověřená implementace

- `Camera` drží primární hodnotový stav: `position`, `direction`, `up`, `fov`, `aspectRatio`, `near`, `far`. `getCenter() const` vrací `position + direction`; `getViewMatrix() const` volá `glm::lookAt`, `getProjectionMatrix() const` používá `glm::perspective(glm::radians(fov), aspectRatio, near, far)`.
- `Scene` nově **vlastní** `Camera camera;` (hodnotový člen). `Scene(const Camera& source) : camera(source)` vytvoří samostatnou kopii; `getCamera()` vrací `Camera&` jako nevlastnící přístup k existující kameře. Každá ze tří scén má vlastní instanci, byť nyní se shodným počátečním nastavením.
- V `Application::createResources()` se směr odvozuje z bodů `glm::normalize(target - position)`. Původní nulový `direction` byl odstraněn.
- `Application::run()` získává v každém snímku aktivní scénu, volá `update(time)`, potom `shaderProgram->use()`, nastaví `viewMatrix` a `projectionMatrix` přes `ShaderProgram::setMat4()` a nakonec `draw()`. Nejde již o jednorázové nastavení matic před smyčkou.
- `Camera::resize(width,height)` pouze přepočítá `aspectRatio`; `glViewport()` zůstává v `Application` callbacku. Nepotřebný `#include <GLFW/glfw3.h>` byl z `Camera.h` odstraněn po diagnostice konfliktu GLFW/OpenGL hlaviček s GLAD.
- **Nedokončeno:** resize callback dosud nevolá `Camera::resize()` pro scény; současný `aspectRatio` tedy po změně rozměru zůstává starý. `Camera::resize` nekontroluje nulovou výšku. Kamera zatím nemá API pro posun a změnu orientace; WASD ani rozhlížení myší nejsou napojené. Lambert/Phong/Blinn osvětlení dosud nezačalo. Samostatné kamery mají zatím stejný výchozí stav.

## Samostatně prokázáno

1. **Identifikace chybějící schopnosti a změna priority:** student sám namítl, že pevná kamera a perspektiva neumožňují procházet scénou, a na základě letošního cvičení i ukázky jiného studenta navrhl začít kamerou před osvětlením. Dokládá orientaci na požadované chování místo mechanického pokračování v další funkci.
2. **První návrh stavu a rozhraní:** samostatně navrhl pozici `x,y,z`, `up`, `lookAt`, přístupové metody a notifikaci typu Observer. Volbu mezi pevným cílem a směrem u FPS kamery zdůvodnil očekávaným WASD a ovládáním myší (pohyb bez změny orientace).
3. **Rozdělení odpovědností:** bez převzetí hotového kódu formuloval „Application detekuje změnu, Camera počítá projekční matici, ShaderProgram předává výsledek GPU“. Následně samostatně shrnul, že volající řeší viewport, volá `resize()` a teprve pak `getProjectionMatrix()`.
4. **Návrh vazby Scene–Camera:** sám zvolil nezávislou kameru pro každou scénu, aby měla vlastní stav a zanikala se scénou. Navázal to na UML kompozici a formuloval otázku, zda použít hodnotový člen či `unique_ptr`.
5. **Rozpoznání nesouladu v implementaci:** po prvním pushi s `Camera& camera` v `Scene` sám upozornil, že předává jednu instanci všem scénám. Následně vlastním vyjádřením správně dovodil, že při hodnotovém členu může lokální počáteční kameru kopírovat do tří nezávislých scén a původní lokální objekt může zaniknout. Přesné znění konstruktoru a getteru bylo rozebráno s vedením.
6. **Implementační pokus:** student ve více commitech sám vytvořil `Camera`, `Scene` s hodnotovým členem, GLM výpočty a přesun uniformů do smyčky. Dokladem je reálná iterace kódu, nikoli automaticky důkaz samostatného porozumění všem API detailům.

## Pochopeno s vedením

- **Vektor směru vs. cílový bod:** původně předal `direction=(0,0,0)`; asistent vysvětlil, že `lookAt(position, position + direction, up)` potřebuje nenulový směr a ukázal `normalize(target-position)`. Student změnu následně implementoval (`3b218847`). Samostatné odvození z nového příkladu zatím neověřeno.
- **Primární stav vs. odvozené matice:** rozhodnutí počítat `view` a `projection` z parametrů kamery, zachovat `get*() const` a oddělit `glViewport` přišlo po dílčím vysvětlení. Student metody implementoval sám, jejich plný kontrakt dále ověříme.
- **Reference, vlastnictví a lifetime:** první `Scene(Camera&)` s členem `Camera&` odkazovala na lokální kameru `createResources()`, což vytvořilo dangling reference. Chybu společně identifikovali; výklad, že parametr `const Camera&` může iniciovat hodnotovou kopii a `getCamera(): Camera&` je nevlastnící přístup, poskytl asistent. Student toto poté správně shrnul a implementoval.
- **Konstruktor a const-correctness:** `Scene(const Camera&)` místo mezikopie `Scene(const Camera)`, `const` gettery, rozdíl přímého vlastnictví a `unique_ptr` byly korigovány v review.
- **OpenGL include řetězec:** screenshot překladu ukázal konflikt systémového `GL/gl.h` s `glad/gl.h`. Asistent ukázal, že `Scene.h -> Camera.h -> GLFW/glfw3.h` přeneslo transitive include; student poté `GLFW` z `Camera.h` odstranil (`d7361a18`). Samostatné nalezení příčiny nebylo prokázáno.
- **Aktualizace uniformů:** asistent upozornil, že nahrání matic před `while` nestačí, a navrhl pravidelné aktualizace před `draw()`. Student přesun implementoval (`3b218847`). Observer byl diskutován, ale vědomě odložen jako zatím nepotřebná abstrakce.

## Vývoj mentálního modelu

1. **Pevné view/projection v Application** student zpochybnil jako překážku pohybu ve 3D. Začal návrhem polohy a `lookAt`, uvažoval o Observeru.
2. Po vysvětlení FPS chování rozlišil **polohu** a **směr pohledu**; matematické převádění bodu na směr musel ještě doplnit s nápovědou.
3. V architektuře rozpoznal, že třída může být popsána UML vazbami dříve, než vybereme konkrétní C++ mechanismus; samostatně rozhodl, že kamera logicky náleží scéně.
4. První `Camera&` jako člen `Scene` omylem sdílel lokální objekt. Student chybu v samotném sdílení poznal, s vedením rozlišil **referenci jako člen** od **reference jako návratové hodnoty** a přešel k hodnotovému vlastnictví.
5. Přechod z kopírování parametru hodnotou na `const Camera&` snížil zbytečné kopírování; referenci v getteru ponechal, protože nepřevádí vlastnictví.
6. Nečekaný překladový konflikt po přidání hodnotového členu ukázal, že úplná definice typu v hlavičce může propagovat další `#include` a narušit další překladové jednotky.
7. V aktuální etapě je kamera oddělena jako matematický objekt a její uniformy jsou obnovovány v render loop; vstupy, resize a pozorovatelný test tří různých kamer teprve následují.

## PRINCIP – nejvyšší přenositelná abstrakce

**Nejdříve stanov význam, identitu a životnost objektů; způsob reprezentace v C++ je až důsledek.** UML kompozice `Scene ◆— Camera` znamená, že scéna vlastní svou instanci. Hodnotový člen `Camera camera` tuto sémantiku vyjadřuje bez heap alokace. Parametr `const Camera&` se může použít ke kopírování do vlastního členu a `getCamera(): Camera&` umožňuje další vrstvě pracovat s **tou samou instancí bez změny vlastnictví**. Samotný pointer/reference není mechanismus péče o životnost.

**Odděl událost, trvalý stav, odvozená data a jejich konzumenta.** GLFW callback reprezentuje událost, `Camera` drží geometrický stav, GLM vypočítá matice a `ShaderProgram` je nahraje do GPU. Aktualizace CPU stavu není automaticky aktualizace GPU uniformu. Pro nynější projekt je jednoduchý *pull* aktuálních matic při každém snímku přiměřenější než Observer s registrací/odregistrováním posluchačů.

**Transitivní závislosti hlaviček jsou součástí návrhu.** Jakmile má `Scene` hodnotový člen `Camera`, musí znát kompletní definici třídy. Nadbytečné `#include <GLFW/glfw3.h>` se tím propaguje a může kolidovat s GLAD. Třída by měla v hlavičce záviset jen na skutečně potřebných typech.

## Potřebuje upevnit / zatím nevyhodnoceno

- **Životnost:** bez nápovědy vysvětlit, proč lokální `Camera` v `createResources()` může zaniknout po kopírování do členů scén; naopak proč `Camera&` jako člen při stejném uspořádání vedla k dangling reference. Odpověď na hypotetické použití reference z `getCamera()` při změně stavu nebyla explicitně ověřena.
- **Směr pohledu:** bez nápovědy odvodit `direction = target - position`, předvést rozdíl pohybu vlevo/vpravo a otočení; ověřit jednotkovost a vhodné použití `normalize`.
- **Resize:** `window_size_callback` pouze volá `glViewport`; doplnit `glfwSetFramebufferSizeCallback`, předat rozměry do `Camera::resize` u všech scén nebo při aktivaci scény, ošetřit `height == 0`, ověřit zúžení okna bez eliptických koulí.
- **Vstupy:** WASD, delta time, yaw/pitch, mouse callback a ortogonální základna směru/right/up chybějí. Samostatný návrh rozhraní a implementace **zatím nevyhodnoceny**.
- **Více shader programů:** nyní jediný `shaderProgram`; u DU03 osvětlení zajistit matice ve všech používaných programech. Zvažovat Observer až tehdy, bude-li jasný praktický přínos.
- **Ověření:** nezávislá kompilace posledního stavu, běh, prokazatelná změna výchozího pohledu mezi scénami a korektní minimizace/resize nebyly doloženy. Poslední nahlášený build screenshotem byl chybový; související include oprava je v commitu, ale bez potvrzeného nového buildu.

## Další procvičení – konkrétní návaznost

1. **Nejbližší krok:** samostatně navrhnout veřejnou metodu `Application` pro událost změny framebufferu (např. `onFramebufferResize(width,height)`), kterou zavolá GLFW callback; uvnitř aktualizovat `glViewport` a poměry stran všech scén bez GPU operací v `Camera`.
2. Otestovat dvě scény s prokazatelně odlišnou polohou nebo směrem kamery; po přepnutí se musí změnit pohled, aniž by se měnily cizí kamery.
3. Následně navrhnout malé API kamery pro posun a rozhlížení, implementovat WASD + myš a nezávislost na snímkové frekvenci.
4. Teprve poté zavést osvětlení (Ambient/Lambert/Phong/Blinn-Phong) se správnou soustavou normál a světla.

## Důkazy

- `5f878cb763` (1. 11.): první návrh `Camera.h/.cpp` a začlenění do CMake.
- `95e45752fc` (4. 11.): první zapojení kamery do `Scene` jako nevlastnící `Camera&` odkazující na lokální instanci – chybný lifetime.
- `2de3c784fb` (5. 11.): hodnotový `Camera camera` v `Scene`, konstruktor původně kopírující parametr hodnotou.
- `d7361a184c` (5. 11.): `Scene(const Camera&)` a odstranění `GLFW` z `Camera.h`.
- `3b218847cb` (5. 11.): `normalize(target-position)` a předávání `viewMatrix`/`projectionMatrix` v každém snímku.
- Diskuse 1.–5. 11.: vlastní návrh kamery před osvětlením, FPS chování, rozdělení Application/Camera/ShaderProgram, UML/kompozice, rozpoznání chybného sdílení `Camera&`, slovně vysvětlené kopírování do scén, screenshot chyby překladače.

**Rozsah checkpointu:** pouze dokumentace `project/notes/` a `project/README.md`; žádné zdrojové soubory, modely, shadery ani CMake se tímto checkpointem nemění.
