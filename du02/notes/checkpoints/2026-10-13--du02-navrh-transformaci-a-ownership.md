# Learning checkpoint – DU02: návrh transformací, Composite a ownership

**Datum:** 2026-10-13  
**Typ etapy:** teoretický návrh před implementací DU02  
**Výchozí code commit:** `f76cd1538f74d08cc088a185872eb5762159cc66` (`built and running`, 2026-10-11)  
**Předchozí learning checkpoint:** `2026-10-11--funkcni-oop-render-pipeline.md`

## Kontext a hranice důkazů

Na základě letošního zadání DU02 byla porovnána cvičení 3 a 4. Rozhodnutí: nevytvářet samostatnou duplicitní aplikaci pro cvičení 4; transformace, uniformy a Composite procvičit v DU02. Cvičení 3 (rovnice transformací bez matic) využít jako kontrolu porozumění.

Dosavadní diskuse se týkala **návrhu**, nikoli nové implementace. Aktuálně ověřený kód na GitHubu stále obsahuje `Application`, `Model` a `ShaderProgram`; v `Application` jsou `unique_ptr<Model>` a `unique_ptr<ShaderProgram>`. Oproti tomu `Scene`, `DrawableObject` ani polymorfní transformační hierarchie zatím nebyly v této etapě implementací prokázány.

## Samostatně prokázáno

- Popsal datový tok: na CPU se vypočte výsledná transformační matice, předá se GPU a vertex shader transformuje vrcholy.
- Rozpoznal, že `Transformation` nemá znát `DrawableObject`: má poskytovat matici a volající zajistí její použití. Zdůvodnil to nízkou provázaností, jasnou odpovědností a znovupoužitelností.
- U rotace kolem obecného středu samostatně odvodil potřebu složené transformace (posun ke středu, rotace, posun zpět).
- Uvažoval o polymorfní dynamické kolekci, společné abstraktní třídě a virtuálních metodách pro různé typy transformací včetně vnořených kompozitů. Návrh odpovídá jádru Composite; konkrétní idea společného `getMatrix()` a seznam variant byly předtím představeny asistentem.
- U sdílené mutable `Rotation` rozpoznal, že pro nezávislé ovládání dvou stromů by bylo potřeba oddělit jejich transformační stav.
- U sdílených pointerů rozpoznal rizika změny společné instance a zániku objektu.

## Pochopeno s vedením

- Rozlišení typového vztahu (IS-A), strukturálního vztahu (HAS-A / používání) a vlastnictví/lifetime; odlišení asociace, závislosti, agregace a kompozice v UML.
- Ujasnění, že abstraktní `Transformation` nelze přímo instancovat, ale `unique_ptr<Transformation>` může vlastnit instanci odvozeného typu; nutný virtuální destruktor.
- Rozlišení výlučného `unique_ptr` (nekopírovatelný, přenos přes `std::move`) a nevlastnícího raw pointeru; samotný pointer nedokládá ownership.
- Rozlišení `Model` (sdílitelná geometrie) a `DrawableObject` (konkrétní instance se svým transformačním stavem).
- `Scene` s `vector<unique_ptr<DrawableObject>>` představuje výlučné vlastnictví DrawableObject; použití modelu automaticky neznamená jeho vlastnictví.
- Po vysvětlení se rýsuje pracovní varianta `DrawableObject` výlučně vlastní svůj `Transformation` a `CompositeTransformation` vlastní dílčí transformace. Jde o návrh k ověření, ne definitivně studentem samostatně obhájený výsledek.

## Vývoj mentálního modelu

1. **Výchozí představa:** transformační třída by měla umět nejen vypočítat a složit matici, ale i aplikovat ji na konkrétní `DrawableObject`.
2. **Změna:** student sám zvolil variantu, kde transformace poskytuje matici bez znalosti vykreslovaného objektu; převzal použití do `DrawableObject` či příslušné vykreslovací vrstvy.
3. **První zobecnění:** rotace kolem libovolného bodu ukázala, proč nestačí jedna pevná sekvence parametrů translation/rotation/scale a proč potřebujeme volné skládání.
4. **Composite:** student odvodil polymorfní kolekci přes abstraktního předka; konkrétní rozhraní a vzor byly nejprve nabídnuty asistentem.
5. **Ownership – slepá cesta:** zprvu preferoval nevlastnící pole pointerů a domníval se, že abstraktní třída nemůže vlastnit konkrétní objekty. Vysvětlen rozdíl mezi typem ukazatele, skutečným dynamickým typem a správcem životnosti.
6. **Sdílení:** student si všiml, že více scén může využívat stejný obsah; po rozlišení `Model` vs. `DrawableObject` se ukázalo, že sdílet geometrii neznamená sdílet transformační stav.
7. **Stav na konci:** jsou rozlišeny otázky *co je objekt za typ*, *na jaké objekty odkazuje* a *kdo je vlastní*. Konečné lifetime vztahy i implementace stále čekají na vlastní ověření.

## Generalizovatelné poučení / nejvyšší abstrakce

**Odpovědnost, vazba a vlastnictví jsou tři samostatná návrhová rozhodnutí.** Typová hierarchie určuje substituovatelnost, strukturální vztah vyjadřuje spolupráci a vlastnictví určuje životní cyklus. Volba reference, raw pointeru, `unique_ptr` či `shared_ptr` má následovat skutečné požadavky na lifetime a sdílení, nikoliv je nahrazovat. Odvozený objekt lze vlastnit přes ukazatel na abstraktní základ.

**Sdílení identity není totéž jako shodné parametry.** Dvě samostatné rotace o stejný úhel lze měnit nezávisle; dvě reference na tutéž mutable rotaci sdílejí změny. Tento princip platí i mimo OpenGL.

## Potřebuje upevnit / zatím nevyhodnoceno

- Samostatná obhajoba konečného ownershipu ve vztazích `Scene`–`DrawableObject`–`Transformation`–`CompositeTransformation`; otázka vnořených kompozitů a dynamického stavu.
- Návrh `CompositeTransformation::getMatrix()`, pořadí násobení matic (pravostranné působení na sloupcový vektor) a ověření rozdílu mezi `T*R` a `R*T`: **zatím nevyhodnoceno**.
- Reprezentace statické vs. dynamické transformace a její aktualizace v čase: **zatím nevyhodnoceno**.
- Převod návrhu do funkčního C++, testy, cleanup a ownership zdrojů: **zatím nevyhodnoceno**.
- Pozor na záměnu `IS-A Transformable` a `IS-A Transformation`; první může být rozumné rozhraní, druhé pro `DrawableObject` významově nesedí.

## Další procvičení

1. Popsat ownership stromu `DrawableObject → Composite → Rotation/Translation` i co se stane při zániku jednotlivých objektů.
2. Slovně nebo pseudokódem samostatně navrhnout skládání matic v `getMatrix()` a otestovat pořadí na jednoduchém vrcholu.
3. Rozhodnout, kde se drží čas/rychlost dynamické rotace a kdo vyvolává aktualizaci.
4. Navrhnout minimální rozhraní tříd teprve po ověření požadavků a vztahů.
5. Implementací potvrdit (ne pouze převzít z diskuse) životní cyklus objektů a polymorfní destrukci.

## Důkazy

- Diskuse v tomto chatu dne 2026-10-13: explicitní zdůvodnění oddělení `Transformation` od `DrawableObject`, rozpoznání skládání pro rotaci kolem bodu, vlastní návrh polymorfní kolekce, reakce na sdílení transformací.
- `f76cd1538f74d08cc088a185872eb5762159cc66`: poslední ověřený code commit; teoretická etapa na něj navazuje bez úprav zdrojového kódu.
- `6a4308e2e0f17f2812ce46b0f794931a2032e827`: předchozí dokumentační checkpoint funkční OOP render pipeline.
