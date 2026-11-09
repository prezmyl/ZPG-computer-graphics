# Learning checkpoint – DU03: FPS kamera, WASD, myš a tok vstupních událostí

**Datum:** 2026-11-09  
**Navazuje na:** `2026-11-05--du03-kamera-vlastnictvi-a-aktualizace-matic.md`  
**Ověřený GitHub HEAD:** `5a972baa559fa0a661e2cbec221962b229bdd34a`  
**Zdroje:** diskuse 6.–9. 11. 2026, zdrojové soubory `Application.h/.cpp`, `Camera.h/.cpp`, `Scene.h`, relevantní Git diffy (resize, WASD, rotace, myš, kurzor), současná mapa znalostí a předchozí checkpoint.  
**Ověření funkčnosti:** uživatel explicitně uvedl, že resize test funguje, po opravě zpracování mouse delta kamera reaguje správně a po aktivaci `GLFW_CURSOR_DISABLED` považuje WASD + rozhlížení myší za funkční. Asistent neprovedl vlastní kompilaci, runtime test ani automatizované testy.

## STAV – co je v aktuálním kódu

- `Application` vlastní kolekci `vector<unique_ptr<Scene>>` a vybírá `activeSceneIdx`; `Scene` vlastní `Camera` hodnotou. Nevlastnící `getActiveScene(): Scene&` vrací odkaz na existující scénu; `getCamera(): Camera&` analogicky na její kameru.
- Resize: registrován `glfwSetFramebufferSizeCallback`. Statický callback přes `glfwGetWindowUserPointer` volá `Application::onFramebufferResize(width,height)`, která nastaví `glViewport` a zavolá `Camera::resize` pro všechny scény. Kamera odmítá nulové/nekladné rozměry. Uživatel úspěšně zkoušel změnu proporcí okna; nejsou doloženy automatizované testy a přesný test minimalizace.
- `Application::run()`: `deltaTime = now-lastTime` z `glfwGetTime()` s počáteční hodnotou `lastTime` před smyčkou; `glfwGetKey` čte držené W/S/A/D, `distance = speed*deltaTime`, znaménko určuje směr. Kamera zná pouze `moveForward(distance)` a `moveRight(distance)`, nikoli klávesy GLFW.
- `Camera::moveForward()` projektuje směr do XZ (nulová Y, normalizace) pro chůzi ve stálé výšce; `moveRight()` počítá `normalize(cross(forward, up))`. `direction` zůstává směrem pohledu; není zaměňován s vektorem pohybu.
- `Camera::rotate(deltaYaw, deltaPitch)` má `direction` jako jediný uložený zdroj orientace. Z něj `atan2`/`asin` odvozuje yaw/pitch, přičítá změny **v radiánech**, omezuje pitch na ±89° a ze `sin/cos` skládá nový `direction`.
- Statický `mouse_callback` volá `Application::onMouseMove(xpos,ypos)`. `Application` vlastní mezi událostmi `lastMouseX`, `lastMouseY` a `firstMouse`; z rozdílů sousedních událostí počítá `deltaYaw = deltaX*sensitivity`, `deltaPitch = -deltaY*sensitivity`, kde `sensitivity=glm::radians(0.1f)`. Po každém pohybu aktualizuje poslední souřadnice. `glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED)` zabraňuje narážení kurzoru na okraje.
- Každý render snímek nahrává uniformy `viewMatrix` a `projectionMatrix` aktuální kamery do zatím jediného shader programu.
- Aktualizovaný UML diagram `project/diagrams/architecture.puml` zahrnuje nové metody a vstupní stav aplikace; jde o záznam struktury existujícího kódu, ne automatický důkaz správné sémantiky každé UML relace.

## Samostatně prokázáno (podle vlastních úvah a iterací)

1. **Oddělení vstupu a domény:** student bez požadavku na hotový kód výslovně zdůvodnil, že `Camera` nemá vědět o GLFW a klávesách; má pouze udržovat a měnit geometrický stav. O nové třídě či vzoru uvažoval, ale nevnucoval je bez konkrétní potřeby.
2. **Volba zdroje pravdy pro orientaci:** samostatně navrhl neukládat yaw/pitch, protože je lze odvodit z aktuálního `direction`; navrhl rozhraní `rotate(yawDelta,pitchDelta)` a převod tam/zpět. Konkrétní funkce `atan2`/`asin` a rovnice `sin/cos` byly dodány s vedením.
3. **Vlastní experimentování s hodnotami a API:** připravil `getActiveScene(): Scene&`, ptal se na variantu návratu `Scene` hodnotou a její vztah k `unique_ptr`. Rozpoznal, že návrh ownershipu je důležitý; faktické chování kopie a zákaz kopírování `Scene` s `unique_ptr` musel vysvětlit asistent. Getter později omezil na `private`.
4. **Aktivní diagnostika pozorovaného chování:** student hlásil, že po otočení o 90° vidí pouze pozadí; po vysvětlení umístění objektů ve zorném poli potvrdil, že rotace je ve skutečnosti správná. Později samostatně otestoval myš, odhalil problém při dosažení okraje okna a navrhl konkrétní `glfwSetInputMode(...GLFW_CURSOR_DISABLED)`. To je samostatná identifikace mechanismu pro vyřešení projevujícího se omezení.
5. **Pokrok implementací:** student postupně zapojil časování, W/S, A/D, `rotate`, callback myši, aktualizaci `lastMouseX/Y` a zachycení kurzoru. Jde o samostatnou práci s podporou review, nikoli důkaz, že bez nápovědy dokáže celý řetězec odvodit znovu.

## Pochopeno s vedením / chyby a jejich opravy

- **`deltaTime`:** první verze použila `long` a neinicializované `lastTime`; asistent identifikoval ztrátu zlomku sekundy a nedefinované chování. Student opravil na `double now`, inicializované `double lastTime` a `float deltaTime`. Doložená implementace, samostatné odvození znovu neověřeno.
- **Znaménko W/S, A/D:** původně W i S předávaly kladnou vzdálenost; po review student opravil pro S záporné znaménko. Vektor `right` získal pomocí vektorového součinu; správné pořadí `cross(forward,up)` i vodorovnou projekci už v dialogu asistent vysvětlil.
- **Rotace a jednotky:** první `clamp(pitch,-89,89)` zaměňoval stupně a radiány. Asistent vysvětlil rozsah, oprava používá `glm::radians`. Test `rotate(90°)` byl původně uvnitř `while`, a tedy prováděn každý snímek; po upozornění přemístěn mimo smyčku a později odstraněn.
- **Mouse delta a životnost:** první `lastMouseX/Y` a `firstMouse` byly lokální v `onMouseMove()`, takže se `firstMouse=true` opakovalo při každém callbacku. S vedením přesunuto do členů `Application`. Následný první funkční callback zapomínal po výpočtu změn aktualizovat `lastMouseX/Y`, což způsobovalo akumulaci odchylky a rychlé zmizení scény z pohledu. Asistent určil chybu a student opravil.
- **GLFW callback, převod pixelů:** mechanismus `glfwSetCursorPosCallback`, `glfwGetWindowUserPointer`, inicializační přeskočení první události a `sensitivity` v radiánech na pixel byly vysvětleny, student je zapojil.
- **Prázdná scéna po rotaci:** uživatel správně potvrdil, že černé/jednobarevné pozadí nebylo automaticky chybou OpenGL, ale pohledem mimo objekty. Rozpoznání vzniklo po vysvětlení.
- **Zachycení kurzoru:** asistent v předchozích krocích možnost `GLFW_CURSOR_DISABLED` zmínil; uživatel si ji při konkrétním projevu sám vybavil a požádal o potvrzení. Proto je doložené praktické použití, nikoli zcela nezávislé objevení nového API.

## Vývoj mentálního modelu

1. Od kamery bez vstupů student správně vymezil `Application` jako překladatel GLFW událostí a `Camera` jako geometrický stav. Vyhnul se předčasnému zavedení `CameraController` či Observeru.
2. První experiment s WASD odhalil význam výběru časového typu a inicializace. Rozlišení pohledu od vodorovného směru chůze umožnilo stabilní výšku při W/S a vektorový součin určil pravolevý pohyb.
3. Při orientaci student navrhl jediný zdroj pravdy `direction`; inverzní trigonometrie a omezení pitch se doplnily s výkladem. Ověření 90° ukázalo rozdíl mezi chybným rendererem a prázdným zorným polem.
4. Přidání callbacku myši odkrylo rozdíl mezi **stavem přetrvávajícím mezi událostmi** (členská proměnná) a **jednorázovým výsledkem** (lokální delta). První opravou se vyřešil scope, druhou referenční bod následné změny.
5. Po odstranění limitu fyzického kurzoru přes GLFW disabled mode uživatel potvrzuje funkčnost celé základní navigace; znalost dalších hraničních stavů zůstává neověřena.

## PRINCIP – nejvyšší přenositelná abstrakce

**Ovládání je transformace událostí a uplynulého času na změnu doménového stavu, nikoli přímá vazba kláves na objekty.** Externí rozhraní (GLFW) dodává události a aktuální stav kláves, `Application` je interpretuje (rychlost × delta času; delta pixelů × úhlová citlivost) a `Camera` zná pouze operace `moveForward`, `moveRight` a `rotate`. Vzniká čitelné oddělení odpovědností, které lze využít i mimo grafiku pro GUI, ovládání robota, simulace či hry.

**Přírůstková operace musí mít přesný časový či prostorový referenční bod.** Pro snímky platí `deltaTime = now-lastTime; lastTime=now`. Pro myš platí `deltaX=x-lastX; lastX=x`. Pokud je `lastX` lokální, reference na minulý stav se ztratí; pokud se neaktualizuje, změna se stále měří od původního bodu a opakovaným přičítáním se chybně akumuluje. To je přenositelné pravidlo pro event-stream zpracování a numerickou integraci.

**Reprezentace stavu a jednotky musí mít stabilní kontrakt.** `direction` je vektor prostoru, `yaw/pitch` dočasné odvozené úhly, `deltaX/deltaY` rozdíl pixelů a `deltaYaw/deltaPitch` změna radiánů. Nezaměňovat významově rozdílné hodnoty, byť jsou všechny `float/double`.

## Potřebuje upevnit / zatím nevyhodnoceno

- **Diagonální rychlost:** W+D znamená dva nezávislé posuny, takže rychlost je v ideálním případě `sqrt(2) * speed`. Před rozhodnutím o opravě ověřit, zda je pro zadání nutná konstantní rychlost ve všech směrech; případná normalizace výsledného pohybu zatím není implementovaná.
- **Extrémní orientace:** `Camera::moveForward/right` normalizuje XZ projekci `direction`. Pitch ±89° omezí dosažení přesně svislého směru při používání `rotate`, ale konstruktor stále přijímá libovolný `direction`; nulový/svislý směr a nejednotková délka nejsou obecně ošetřené. Samostatné vysvětlení cross-product orientace a počátečního yaw/pitch je vhodné procvičit.
- **Počáteční rozměry:** kamera se vytváří s `800/600`; inicializace z reálného `glfwGetFramebufferSize` dosud není doložena. Callback při resize aktualizuje všechny scény; test přesné inicializace na HiDPI zatím nevyhodnocen.
- **Reaktivace a focus:** `GLFW_CURSOR_DISABLED` nyní funguje, ale chybí záměrné uvolnění kurzoru, reset `firstMouse` při návratu fokusu a případné bezpečné zacházení s dlouhým `deltaTime` po pauze. Nejde o nutně povinný požadavek pro aktuální úkol.
- **Ovládání a vlastnictví:** znovu bez vedení zdůvodnit, proč `getActiveScene(): Scene&` objekt nekopíruje ani nepředává vlastnictví, zatímco `Scene` vracená hodnotou by se musela kopírovat a v dnešní podobě je nekopírovatelná kvůli členům `vector<unique_ptr<...>>`.
- **Nekonzistence popisů s věcnou správností:** komentáře v kódu obsahují pozůstatky pokusů a překlepy, ale nepředstavují nyní blokující bug.
- **Běhové testy:** student potvrdil základní chování; nebyl proveden nezávislý build, opakovatelný test první události po refocusu, současný stisk WASD ve více scénách ani unit test úhlových hranic.

## DALŠÍ KROK

Za funkční uzavřenou etapu považovat **základní FPS navigaci** (WASD + myš + resize) na základě uživatelského testu. Před dalším větším rozšířením lze nejvýše krátce ověřit chování při diagonálním pohybu / ztrátě fokusu; nezačínat nyní abstrakcemi bez potřeby.

**Následující hlavní etapa DU03:** znovu vymezit požadavky osvětlení podle letošního zadání a nejdříve nechat studenta navrhnout tok světelných údajů (co vlastní `Scene`, co dostává `DrawableObject`, co přenáší `ShaderProgram`, co počítá GLSL). Teprve poté navrhnout minimální změny vertex layoutu/normál a jednotlivých osvětlovacích shaderů, bez předčasného zavádění speciálního design patternu.

## Důkazy – vybrané commity

- `c3e44058`, `acf05fb1`, `eaebd31b` (6. 11.): framebuffer resize, guard nulových rozměrů, oprava registrace callbacku.
- `4fbd6439`, `cb3f5b68`, `33a74c91` (7. 11.): pokusy a opravy W/S, A/D, `deltaTime`, vodorovná projekce pohybu a `cross(forward,up)`.
- `a792ae35`, `76d589b0` (7. 11.): `Camera::rotate`, omezení pitch v radiánech, jednorázový test rotace.
- `2e3e686b`, `a61a097e`, `d91151d6` (8. 11.): `onMouseMove`, chybné lokální `firstMouse` a následně chybná referenční pozice, jejich opravy.
- `3aba55b7` (9. 11.): povolen `GLFW_CURSOR_DISABLED`.
- `5a972baa` (9. 11.): aktualizace UML `project/diagrams/architecture.puml`.
- Diskuse 7.–9. 11.: samostatná hranice Camera/GLFW, návrh jednoho `direction` jako zdroje pravdy, otázka hodnotového getteru, ověření prázdného zorného pole při 90°, diagnostika hran okna, finální potvrzení fungujícího ovládání.

**Rozsah checkpointu:** pouze learning dokumentace. Žádný zdrojový kód, shader, model, CMake ani UML soubor se tímto checkpointem nemění.
