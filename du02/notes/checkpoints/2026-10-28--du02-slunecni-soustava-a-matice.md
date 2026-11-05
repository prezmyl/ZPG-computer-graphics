# Learning checkpoint – DU02: sluneční soustava, řetězce matic a hranice dynamických transformací

**Datum:** 2026-10-28  
**Navazuje na:** `2026-10-27--du02-animace-sceny-a-prepinani.md`  
**Zdrojový stav:** `b9a53b61f8d0825abc9ea42edd7e20186d68b567` (poslední změna k 28. 10. 2026)  
**Zadání:** `ZPG_home02.pdf`, body 1–8; dokumentace aktualizována zároveň v `project/souhrn.txt`.  
**Ověření:** aktuální GitHub zdrojové soubory, relevantní diffy, rozhovor; uživatel výslovně potvrdil funkční Slunce a Zemi a pushnul scénu s Měsícem a snímkem `pol0511_8.png`. Asistent neprováděl nezávislý lokální build ani runtime test.

## STAV – implementace DU02

- Předchozí otevřená chyba v `Application::setActiveScene(idx)` byla opravena v commitu `897793f`: validuje se **nový** vstup `idx < scenes.size()`.
- `Application` vlastní **tři** instance `Scene`: les (klávesa 1), prázdná scéna zamýšleného loginu (klávesa 2), sluneční soustava (klávesa 3). `Scene::update(time)` před vykreslením iteruje polymorfní `Animation`.
- `sphereModel` je společná geometrie Slunce, Země a Měsíce; každé těleso má svůj `DrawableObject` a vlastní `CompositeTransformation`.
- **Slunce:** pouze `Scale(0.65)`. **Země:** `Scale(0.25) → Rotation(vlastní) → Translation(2.5) → Rotation(oběh)`, obě rotace animované. **Měsíc:** `Scale(0.10) → Translation(0.55) → Rotation(oběhM) → Translation(2.5) → Rotation(oběhZ)`, dvě animované rotace.
- Pro oběh Země existují dvě různé `Rotation`, jedna v transformaci Země a druhá v transformaci Měsíce; jejich `RotationAnimation` používají stejný čas i rychlost 0,5 rad/s. **Nejde o sdílenou instanci ani o skutečný scene graph.**
- Bod 3 je splněn příkladem **dynamické rotace**. `Translation` a `Scale` dnes nemají setter ani dynamickou animaci; pro DU02 se uživatel rozhodl jejich animování nyní nerozšiřovat.
- **Bod 7 (rotující 3D login POL0511) není splněn.** Student jej vědomě vynechává, protože momentálně nemá Blender ani prostor pro instalaci. Druhá scéna je prázdná; souhrn nesmí tvrdit opak.
- U bodů 1,2,4,5,6 a 8 odpovídá popis současnému kódu a dříve doloženým testům; automatizované unit testy transformací nejsou doložené. Osvětlení a dynamický aspect ratio zůstávají úkoly DU03.

## Samostatně prokázáno

1. **Asociativita vs. nekomutativita:** student po rozboru řetězce matic správně vlastním vyjádřením uvedl, že závorkování stejného řetězce nezmění výslednou matici, pokud se nezamění pořadí faktorů. Asociativitu již probíral dříve; nyní ji spojil s konkrétním modelem Měsíce.
2. **CPU → GPU přenos:** student samostatně formuloval, že Composite vytvoří výslednou matici a ta se předává GPU pro všechny vrcholy vykreslovaného objektu. Asistent pouze upřesnil, že uniform je samostatný pro každý `DrawableObject`/draw call, nikoli univerzální pro celou scénu.
3. **Relevantní otázka o středu rotace:** student sám připomněl známý princip rotace kolem obecného bodu `T(P) * R * T(-P)` a rozpoznal napětí mezi tímto vzorcem a implementací oběhu ve Sluneční soustavě. To prokazuje aktivní hledání souvislostí, **nikoliv ještě samostatnou schopnost oba přístupy odvodit**.
4. **Rozpoznání omezení řešení:** student se sám zeptal, zda dynamická je jen rotace a co ostatní transformace. Správně tak identifikoval mezeru mezi *obecností rozhraní* a *konkrétně implementovaným chováním*.

## Pochopeno s vedením

- **Pořadí tří vrstev:** (1) přidávání transformací do `Composite`, (2) iterativní skládání `result = M_new * result`, (3) použití výsledku na sloupcový vrchol zprava doleva. Student přímo řekl, že v tom má zmatek; konkrétní tabulku mezivýsledků i příklad `R*T` vs. `T*R` vysvětlil asistent. Bez nápovědy ještě neodvodil nový řetězec matic.
- **Lokální a globální souřadnice:** obecná rotace již globálně umístěného objektu kolem bodu `P` má tvar `T(P)*R*T(-P)`; naše modely začínají ve svých lokálních počátcích a polohu získávají až transformacemi, proto `T(-P)` nemusí být explicitní.
- **Hierarchie pohybu bez scene graphu:** Měsíc má svou `R_M*T_M` kompozici v lokální soustavě Země, kterou následně transformujeme `R_Z*T_Z` do světového prostoru. Vzorec posunů a rotací a kompletní kód Měsíce dodal asistent na výslovnou žádost; úspěšný commit proto **není důkazem nezávislého návrhu** této hierarchie.
- **Dynamické chování jiných transformací:** oddělení `Transformation` (aktuální parametry a `getMatrix()`) od `Animation` (pravidlo jejich změny v čase) bylo vysvětleno na možných `TranslationAnimation` a `ScaleAnimation`, ale uživatel je neimplementoval ani jejich návrh samostatně neověřil.

## Vývoj mentálního modelu

1. Při aplikaci na planetární scénu student správně poznal obecný vzorec otáčení kolem libovolného bodu, ale nedokázal jej bez pomoci sladit s rotací posunutého objektu a s řetězcem `Composite`.
2. Pojmenoval vlastní nejasnost: pořadí vložení, zápis maticového součinu a skutečné pořadí působení na bod vnímal jako nesrozumitelné.
3. Po explicitním dvoumaticovém příkladu a tabulce skládání se přesunul k abstraktnímu pravidlu asociativity a spojil jednu výslednou `modelMatrix` s přenosem do vertex shaderu.
4. Následně sám identifikoval, že z dynamických transformací jsme skutečně implementovali pouze `Rotation`. Po objasnění možnosti `TranslationAnimation` a `ScaleAnimation` vědomě zvolil nezavádět další abstrakce bez požadavku.
5. Z důvodu praktického omezení u loginu přijal **částečné dokončení DU02** a požádal o pravdivý souhrn i checkpoint.

## PRINCIP – nejvyšší přenositelná abstrakce

**Kompozice operací má dvě oddělené úrovně: vytváření složeného předpisu a jeho vyhodnocení nad daty.** V `Composite` se po vložení `T`, `R` vytvoří součin `R*T`; ten se teprve ve vertex shaderu aplikuje na bod jako `R*(T*p)`. Asociativita dovoluje měnit závorkování téhož řetězce, nikoliv zaměnit pořadí jeho prvků. Toto rozlišení se přenáší i na kompozici funkcí, transformační pipelines a hierarchické systémy souřadnic.

**Lokální souřadnicová soustava snižuje potřebu explicitních kompenzačních operací.** Pokud je geometrie vytvořená kolem vlastního počátku, lze ji nejprve lokálně otočit a následně umístit; při rotaci již umístěných světových bodů kolem `P` je nutné explicitně uvažovat `T(P)*R*T(-P)`.

**Možnost měnit stav není totéž jako implementovaná animace.** Polymorfní `Transformation::getMatrix()` funguje nad statickými i měnícími se parametry. Dynamický je až mechanismus `Animation::update(time)` spolu se setterem dané transformace. Tato hranice odpovědností se týká i GUI a simulací.

## Potřebuje upevnit / zatím nevyhodnoceno

- Samostatně spočítat `R*T` vs. `T*R` na bodu a vysvětlit tři pořadí: vložení → sestavení matice → působení na bod. Chápání asociativity je nyní prokázáno, **samostatné odvození řetězce zatím nikoli**.
- Samostatně vysvětlit, proč pro oběh modelu v lokálních souřadnicích vystačíme s `R*T`, ale pro rotaci již umístěné světové geometrie kolem `P` obecně potřebujeme `T(P)*R*T(-P)`.
- Navrhnout strukturu skutečných parent-child transformačních uzlů bez synchronizace dvou rotací pomocí shodné rychlosti a absolutního času – nyní je to jen s vedením.
- Ověřit platnost `Rotation*` při odebrání jednotlivého objektu za běhu a případně správu časového offsetu při přepínání scén.
- **Zatím nevyhodnoceno:** samostatný návrh `TranslationAnimation` / `ScaleAnimation` a nový typ animace bez dodaného rozhraní.
- Neprovedené: nezávislý build asistentem, unit testy pořadí transformací a úplnost screenshotů pro školní odevzdání.

## Další procvičení

1. **Nejbližší praktický krok:** po rozhodnutí vynechat login zkontrolovat odevzdávané screenshoty a aktualizovaný `souhrn.txt`; pro DU03 pokračovat osvětlením a správným poměrem stran projekce.
2. Později bez nápovědy spočítat jeden bod při `R*T` a `T*R`; teprve pak vnořenou transformaci Měsíce. Připravit malý CPU test `CompositeTransformation::getMatrix()`.
3. V budoucnu procvičit `setTranslation()` nebo `setFactor()` plus příslušné `Animation`, ovšem nevnucovat do nynějšího DU02.

## Důkazy

- `897793f439db049843ef37e99c5ca55eb7cbdc64` (27. 10.): opravená kontrola `idx < scenes.size()`.
- `b73cc684a7d52a3d2c65f5c452a39ffa38096087` (27. 10.): přidána `sphereModel`, oběh Země, oběh Měsíce, klávesa 3 a `pol0511_8.png`.
- `b9a53b61f8d0825abc9ea42edd7e20186d68b567` (28. 10.): poslední kontrolovaný zdrojový stav, úprava komentáře sluneční soustavy.
- Diskuse 27.–28. 10.: explicitní „ok funguje“ pro Slunce–Zemi; dotaz na obecnou rotaci, přiznaný zmatek v pořadí, vlastní shrnutí asociativity a CPU–GPU přenosu, dotaz na dynamické translace a škálování, vědomé vynechání loginu.

**Rozsah změn:** pouze `project/souhrn.txt`, learning checkpoint, konceptová dokumentace a mapa znalostí. Zdrojový kód nebyl změněn.
