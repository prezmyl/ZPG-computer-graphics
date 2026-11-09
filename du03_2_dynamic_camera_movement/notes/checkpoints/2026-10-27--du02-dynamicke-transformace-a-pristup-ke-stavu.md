# Learning checkpoint – DU02: dynamické transformace a přístup k vlastněnému stavu

**Datum:** 2026-10-27  
**Etapa:** od statického lesa k jednomu animovanému stromu, pochopení toku změny stavu, ownershipu a pořadí transformací  
**Navazuje na:** `2026-10-15--du02-transformace-a-composite-implementace.md` a `2026-10-15--du02-drawableobject-a-prenos-matice-do-gpu.md`  
**Ověřený code commit:** `da67ac4395a34d3009f5aea5639ece44beececef` (oprava animace po chybném pokusu `57dea2621c635437afd31e0801339219675e04de`)  
**Metoda ověření:** aktuální GitHub zdrojový kód, rozdíly commitů a diskuse v tomto chatu; student výslovně potvrdil „funguje to“. Asistent aplikaci lokálně nesestavil ani nespouštěl.

## Stav řešení po etapě

- `Application::createResources()` vytváří geometrii stromů a keřů, `ShaderProgram`, `Scene`, několik `DrawableObject` a jejich vlastní `CompositeTransformation`. Využívá jednoho sdíleného modelu pro stromy, jednoho pro keře a společného shader programu.
- V pomocné lokální lambdě `addInstance(..., bool animate = false)` je pro první strom (`i == 0`) vytvořena `Rotation` kolem osy Y. `Application::animatedRotation` získá nevlastnící adresu přes `rotation.get()` **před** předáním `std::unique_ptr` do Composite pomocí `std::move(rotation)`.
- Transformace animovaného stromu se přidávají v pořadí **Scale → Rotation → Translation**; vzhledem k algoritmu Composite (nová matice vlevo) je výsledkem **`M = T * R * S`**. Rotace se aplikuje v místních souřadnicích před posunem, takže strom zůstává na své pozici.
- V každé iteraci `Application::run()` se před `scene->draw()` kontroluje `if (animatedRotation)` a volá `setAngle(static_cast<float>(glfwGetTime()))`; zvolená rychlost je 1 rad/s. Ostatní stromy a keře jsou statické.
- `Scene::draw()` volá `DrawableObject::draw()`, které znovu získá `transformation->getMatrix()`, nahraje výsledek jako `modelMatrix` přes `ShaderProgram::setMat4()` a vykreslí `Model::draw()` / `glDrawArrays()`. Není nutný žádný druhý setter transformace v `DrawableObject`.
- Ponechány shaderové pokusy ze cvičení: `normal.vert` používá `gl_VertexID > 36443` a `normal.frag` intenzitu `0.3 + 0.7 * abs(normalize(surfaceNormal).y)`. Jsou sdílené pro oba modely, takže dělení podle indexu je specifické pro strom a není obecnou materiálovou logikou.
- Současná `Scene` je pouze kolekce objektů s `draw()`, bez `update()`, přepínání scén či samostatné scény loginu a planet. Pokus s `animatedRotation` v `Application` je **dočasné ověření mechanismu**, nikoli cílové rozdělení odpovědností.

## Samostatně prokázáno (podle konkrétních odpovědí v chatu)

1. **Vlastnický graf.** Student bez dodání řešení správně vyjmenoval, že `Scene` vlastní `DrawableObject`, ten vlastní transformaci, zatímco `DrawableObject` pracuje s odkazy na `Model` a `ShaderProgram`. Rozpoznal možnost nevlastnícího přístupu k rotaci a alternativu s gettery napříč více objekty; sám formuloval návrhový problém.
2. **Tok dat při vykreslování.** Student vlastními slovy uvedl `scene->draw() → object->draw() → získání transformační matice → shaderProgram->setMat4() → model->draw() → glDrawArrays()`. Z toho správně odvodil, že není třeba znovu volat `setTransformation()`: aktuální matice se vyžaduje a přenáší při každém kreslení.
3. **Rozlišení výpočetní a vykreslovací odpovědnosti.** Student rozeznal, že `Rotation` tvoří matici podle svého stavu a `DrawableObject` zajišťuje její přenos do GPU. Formulace, že ji Rotation „předá“ do `DrawableObject`, byla upřesněna na aktivní vyžádání přes `getMatrix()`.
4. **Praktická integrace:** student změnu implementoval, pushnul a potvrdil vykreslení animace. Samotné vložení správného algoritmu **není důkazem jeho nezávislého navržení**, protože poslední korekce byla poskytnuta asistentem.

## Pochopeno až s vedením

- **Životní cyklus vs. místo aktualizace:** student původně přirozeně navrhoval měnit rotaci poblíž inicializace v `createResources()`. Po vysvětlení rozdílu mezi jednorázovou inicializací a opakovaným render loop pochopil, že změna úhlu musí nastat průběžně. Koordinátor rozhoduje *kdy*, rotace zná *jak* vypočítat matici.
- **Nevlastnící alias k `unique_ptr`-vlastněné instanci:** použití `Rotation* animatedRotation = rotation.get()` před `std::move(rotation)` jako konkrétní implementační techniku dodal asistent. Ukazatel se při přesunu vlastnictví nemění, ale nezajišťuje životnost objektu.
- **Pořadí matic a osy:** první commit vložil rotaci kolem **Z** až za `Translation`; fakticky vznikalo `R * T * S`, tedy rotace již posunutého modelu kolem světového počátku. Asistent upozornil na chybu a doporučil **Y** a `Scale → Rotation → Translation` (`T * R * S`); student opravil.
- **Diagnostika neplatného ukazatele:** v prvním commitu bylo `animatedRotation = nullptr` a následné bezpodmínečné `animatedRotation->setAngle()`, tedy dereference null pointeru. Asistent chybu identifikoval; student následně doplnil uloženou adresu i kontrolu před použitím.
- **Zvolená míra abstrakce:** pro jedno ověření používáme `Rotation*` v `Application`. Cílové rozhodnutí, kam patří aktualizace více nezávislých animací (konkrétní scéna / aktualizační objekt), jsme probrali, ale zatím neimplementovali.

## Vývoj mentálního modelu

1. **Původní představa:** transformace se vytváří v `createResources()`; proto by mohla být aktualizována tam. Student zároveň viděl, že přístup k hluboko uložené transformaci může znamenat řetězec getterů.
2. **První změna:** díky srovnání `createResources()` a `run()` rozlišil konstrukci stavu od jeho časové změny. Začal uvažovat o nevlastnících referencích a jejich umístění.
3. **Druhá změna:** sám zrekonstruoval pull-model vykreslování: `DrawableObject` žádá čerstvou matici při každém `draw()`. Tím odpadl mylný požadavek „znovu ji předat“ po `setAngle()`.
4. **Funkční omyl:** první implementace neměla inicializovaný nevlastnící pointer a zaměnila lokální rotaci za rotaci kolem počátku. Přímý rozbor `nullptr`, osy a násobení matic vedl k opravě.
5. **Výsledek:** funkčně ověřen jeden lokálně rotující strom, zatímco zbytek scény zůstává statický. Student potvrdil běh; nezávislé odvození pořadí `T * R * S` při novém příkladu **zatím neprokázal**.

## Generalizovatelné poučení / nejvyšší abstrakce

**Odděl zdroj pravdy, jeho změnu a odvozený výsledek.** `Rotation` uchovává úhel; čas řídí jeho změnu; `getMatrix()` na vyžádání přepočítá aktuální matici. Není potřeba ručně synchronizovat více kopií téhož odvozeného stavu. Tento princip platí obecně pro animace, GUI i aplikační modely.

**Vlastnictví, přístup a plánování aktualizací jsou tři nezávislá rozhodnutí.** `unique_ptr` určuje, kdo objekt zničí; `Rotation*` umožní změnit jeho stav bez převodu vlastnictví; koordinující objekt určuje, kdy jej změnit. Platnost pozorujícího pointeru musí být doložena skutečným životním cyklem vlastníka.

**Pořadí operací je sémantika programu.** `T * R * S` a `R * T * S` jsou obě platné matice, ale znamenají jiný pohyb. U skládání se musí stanovit, zda chceme místní rotaci na pozici, nebo obíhání světového počátku.

## Potřebuje upevnit / zatím nevyhodnoceno

- **Potřebuje upevnit:** bez nápovědy zvolit správné pořadí transformací pro rotaci kolem vlastní osy, oběh kolem jiného objektu a rotaci kolem obecného bodu.
- **Potřebuje upevnit:** správné zajištění lifetime nevlastnícího `Rotation*` při výměně/odstranění scény. Současná `if (animatedRotation)` chrání před **null**, nikoli před **dangling pointerem**.
- **Zatím nevyhodnoceno:** samostatný návrh `Scene::update()` nebo vhodné alternativy a rozdělení řízení času mezi `Application` a jednotlivé scény.
- **Zatím nevyhodnoceno:** dvě či více nezávislých animací, přepínání scén, rotující 3D login a hierarchický pohyb Slunce–Země–Měsíc.
- **Zatím nevyhodnoceno:** vlastní CPU test matic pro lokální rotaci a oběh; GPU build/diagnostika nebyly asistentem samostatně provedeny.
- **Poznámka k údržbě:** komentáře v `Application.cpp` obsahují češtinu navzdory školnímu pravidlu anglického kódu; kód při checkpointu vědomě neměníme.

## Další procvičení

1. Pro zadaný bod ručně odvodit účinky `T * R * S` a `R * T * S`.
2. Bez hotového návrhu rozhodnout, kdo vlastní a kdo jen sleduje konkrétní animovanou rotaci u více scén.
3. Implementovat aktualizaci jedné scény tak, aby `Application` nemusela znát její konkrétní objekty.
4. Zobrazit rotující login a poté Slunce–Země–Měsíc s hierarchií oběžných transformací; odůvodnit pořadí operací.
5. Ověřit, co se stane s nevlastnícím `Rotation*` po odstranění `DrawableObject` a proč nestačí otestovat pouze nenulovost pointeru.

## Důkazy

- `57dea2621c635437afd31e0801339219675e04de` (2026-10-27): první pokus o dynamickou transformaci, `Rotation(Z, 45°)` po translaci, nevlastnící `animatedRotation` nenastavený, přesto použit v render loop.
- `da67ac4395a34d3009f5aea5639ece44beececef` (2026-10-27): `Rotation(Y)` mezi Scale a Translation, získání `rotation.get()`, přesun vlastnictví, animován první strom a kontrola `if (animatedRotation)`.
- Diskuse v tomto chatu 2026-10-27: vlastní výklad vztahů objektů, návrh nevlastnícího odkazu vs. getterů, vlastní rekonstrukce toku `Scene::draw() → DrawableObject::draw() → ShaderProgram → Model`; následně potvrzení „funguje to“.
- Předchozí checkpointy z 2026-10-15 zachycují samostatnou hierarchii `CompositeTransformation` a první funkční přenos `modelMatrix` do GPU.

**Rozsah:** pouze learning dokumentace, bez změn `.cpp`, `.h`, shaderů nebo modelů.
