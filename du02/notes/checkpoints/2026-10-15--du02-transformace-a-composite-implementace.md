# Learning checkpoint – DU02: implementovaná hierarchie transformací a Composite

**Datum:** 2026-10-15  
**Etapa:** od teoretického návrhu ke konkrétním C++ třídám; statická kontrola na GitHubu  
**Navazuje na:** `2026-10-13--du02-navrh-transformaci-a-ownership.md`  
**Relevantní poslední code commit:** `e492c754e1098753e0d5dd824f4ed6099e83a5a5`  
**Rozsah:** `Transformation`, `Translation`, `Rotation`, `Scale`, `CompositeTransformation` a jejich CMake integrace; **bez změny zdrojového kódu během checkpointu**.

## Kontext a skutečný stav kódu

Od návrhu z 13. 10. vznikly třídy:

- `Transformation`: abstraktní rozhraní `virtual glm::mat4 getMatrix() const = 0` a virtuální destruktor.
- `Translation`: ukládá `glm::vec3 translation`; `getMatrix()` používá `glm::translate(glm::mat4(1.0f), translation)`.
- `Rotation`: ukládá osu `glm::vec3` a úhel `float` v radiánech; `getMatrix()` volá `glm::rotate`; `setAngle()` umožňuje změnu stavu.
- `Scale`: ukládá tři měřítkové faktory v `glm::vec3`; `getMatrix()` volá `glm::scale`.
- `CompositeTransformation`: dědí ze `Transformation`, vlastní uspořádanou kolekci `std::vector<std::unique_ptr<Transformation>>`, přebírá vlastnictví prostřednictvím `addTransformation(std::unique_ptr<Transformation>)` a počítá složenou matici.
- `CMakeLists.txt`: zdrojové soubory nových tříd jsou přidané do targetu.

Aktuální `CompositeTransformation::getMatrix()`: začne jednotkovou maticí, prochází kolekci v pořadí vložení, pro každou položku provádí `result = transformation->getMatrix() * result` a vrací `result`. Stejné rozhraní podporuje i vnořený Composite.

**Hranice ověření:** soubory a commity byly staticky zkontrolovány na GitHubu. V rámci tohoto checkpointu nebyl proveden build ani běhový či jednotkový test. Třídy zatím nejsou v aktuálním kódu zapojené do `DrawableObject` / shader uniform `modelMatrix` ani do funkční animované scény. „Implementováno“ neznamená „otestováno v renderu“.

## Samostatně prokázáno

- Student správně pojmenoval **asociativitu** násobení matic: součin lze přezávorkovat při zachování pořadí faktorů. V návazné odpovědi ovšem ještě směšoval pořadí aplikace s prioritou operátorů; porozumění **nekomutativitě** bylo následně upevněno s vedením.
- Na otázku po dvou způsobech skládání zvolil `result = transformation->getMatrix() * result`, čímž zachoval požadované pořadí aplikace od začátku kolekce. Alternativy a vodítko pro rozpis iterací poskytl asistent; vlastní samostatné odvození bez nabídky možností zatím nebylo prokázáno.
- U vnořeného Composite sám rozpoznal, že pro volajícího není potřeba speciální větev: stačí zavolat stejné virtuální rozhraní. Upřesnění, že se vrací **matice**, nikoli instance transformací, dodal asistent.
- Sám vybral model animace, kde `Rotation` dostává **úhel aktualizovaný zvenčí** a pouze z něj sestavuje matici.
- Po vysvětlení `glm::vec3`, `glm::mat4` a `glm::translate` správně vysvětlil, proč se výpočet elementární matice začíná jednotkovou maticí: je neutrálním prvkem násobení; zároveň odlišil vytvoření transformační matice od její aplikace na konkrétní bod.
- Student vlastními commity doplnil konkrétní C++ definice `Translation`, `Rotation`, `Scale` a `CompositeTransformation`, jejich datové členy, konstruktory a integraci do CMake. Míra samostatnosti návrhu těchto rozhraní je omezená předchozími příklady/nápovědami, viz níže.

## Pochopeno s vedením

- Rozlišení **parametrů operace** (posun/osa + úhel/faktory) a odvozené reprezentace (`glm::mat4`): první verze `Translation.h` ukládala matici a deklarovala `getTranslation()`. Po rozboru přešla na `glm::vec3` a správně překrytou `getMatrix() const override`.
- Konkrétní použití `glm::translate`, `glm::rotate`, `glm::scale`, jednotkové `glm::mat4(1.0f)`, `member initializer list`, `override`, GLM includes a radiánů bylo vysvětleno nebo přímo doporučeno asistentem. Následné správné použití v kódu je důkaz praktického následování, **ne nezávislé objevení API**.
- Architektura `CompositeTransformation` (`vector<unique_ptr<Transformation>>`, signatura `addTransformation`, `std::move`) a jádro výpočetního cyklu byly již v diskusi explicitně navrženy asistentem. Student je správně převzal a implementoval; samostatný návrh z nulového zadání tím zatím prokázán není.
- S vedením byl rozebrán vztah **ownership – nevlastnící přístup – lifetime**: Composite může výlučně vlastnit rotaci, kdežto `Scene` by na ni mohla nevlastnicky odkazovat pro animaci. `shared_ptr` by řešil jiný, skutečně sdílený lifetime; v aktuálním návrhu jej zatím nepotřebujeme.
- Student v diskusi upozornil na riziko směšování vykreslování a animačních odpovědností v `DrawableObject` a na režii/hrozbu coupling při vyhledávání v Composite; rozlišení coupling daného znalostí interní struktury vs. pouhé existence ukazatele bylo dotaženo s vedením.

## Vývoj pochopení: užitečné omyly a opravy

1. **`Translation` jako uložená `mat4`, `getTranslation()`:** nejistota, zda se uchovává hotová matice či parametry. Oprava po vysvětlení kontraktu `Transformation::getMatrix()` a oddělení stavu od výpočtu.
2. **Matematika skládání:** znalost asociativity byla přítomna; pořadí transformací bylo zprvu zaměněno s prioritou operátorů. Po konkrétním příkladu `Translation → Rotation → Scale` student vybral správné násobení zleva: `S * R * T` pro sloupcový vektor.
3. **Vnořování:** student pochopil, že `Composite` je pro klienta další `Transformation` a může se vyhodnocovat rekurzivně přes stejný kontrakt.
4. **Dynamika:** student zvolil minimální odpovědnost `Rotation` („dostanu nový úhel, vrátím aktuální matici“), bez zabudování času a GLFW do transformační třídy.
5. **Implementace Composite:** první verze `getMatrix()` správně iterovala, ale **nevracela** `glm::mat4`. Na chybu upozornil asistent; student ji opravil v commitu `9663e9e`, včetně explicitního `<utility>`. Zde jde o korekci s vedením, nikoli samostatně nalezenou chybu.
6. **Ukončení nadbytečné návrhové debaty:** student výslovně požádal o přechod od opakované debaty o ownership/coupling k implementaci. To ukazuje schopnost rozpoznat, kdy už pracovní rozhodnutí stačí; definitivní architekturu to však samo nepotvrzuje.

## Generalizovatelné poučení / nejvyšší abstrakce

**Uchovávej minimální významový stav, odvozený výsledek poskytuj přes kontrakt; kompozice nemusí znát konkrétní typ části.** Elementární transformace uchovává parametry a vypočítá `mat4`, Composite pouze používá jednotné `getMatrix()` a skládá výsledek. Oddělení nezávislých důvodů změny umožní později aktualizovat úhel bez změn shaderové či renderovací vrstvy.

**Pořadí v matematickém výrazu je součást významu API.** Asociativita dovoluje přesouvat závorky, ale ne prohazovat členy; `R*T` a `T*R` mají obecně jiný výsledek. API Composite musí explicitně stanovit, zda pořadí vložení znamená pořadí aplikace na sloupcový vektor.

**Vlastnictví a možnost měnit stav jsou různé otázky.** `unique_ptr` určuje vlastníka; nevlastnící ukazatel může sloužit k aktualizaci, ale musí respektovat životní cyklus vlastníka. Při návrhu nelze nahrazovat tuto úvahu automaticky `shared_ptr`.

## Potřebuje upevnit / zatím nevyhodnoceno

- **Zatím nevyhodnoceno:** samostatné napsání `CompositeTransformation::getMatrix()` bez předloženého algoritmu; matematické ověření prohození `T*R` a `R*T` konkrétními vrcholy.
- **Zatím nevyhodnoceno:** build a test výsledných matic, včetně **prázdného Composite**, vnořeného Composite, různých pořadí a hodnot parametrů.
- **Zatím nevyhodnoceno:** dynamická scéna s časově aktualizovaným úhlem, bezpečné přístupy ke konkrétním transformacím a lifetime po odebrání objektu.
- **Zatím nevyhodnoceno:** propojení `DrawableObject`, `Model`, `ShaderProgram` a shader uniform `modelMatrix`; modelové transformace na GPU.
- Upevnit, proč `override` musí odpovídat podpisu základní třídy, jak funguje polymorfní destrukce a `unique_ptr` transfer.
- Drobnosti pro pozdější kontrolu: konzistentní explicitní includes v hlavičkách, normalizovaná nenulová osa rotace, kontrakt příjmu `nullptr` do `addTransformation`. Nejde o důvod překreslovat architekturu před dalším krokem.
- Do kódu zatím nezavádět nové vzory nebo složitý lifecycle mechanismus, pokud se neobjeví reálný požadavek.

## Další procvičení

1. Jednoduchý CPU test: `Translation(2,3,0)` aplikovaná na `glm::vec4(1,1,0,1)` musí vrátit `(3,4,0,1)`.
2. Na jednom bodu porovnat `T*R` a `R*T`; vysvětlit pořadí na sloupcovém vektoru.
3. Ověřit `CompositeTransformation` s nulou, jednou, několika a vnořenou transformací.
4. Zavést minimální propojení `DrawableObject` → `ShaderProgram` uniform `modelMatrix` → vertex shader, bez předčasné komplexní scény.
5. Až po funkčním vykreslení doplnit animaci `Rotation::setAngle()` a ověřit přístup/lifetime.

## Důkazy a Git historie

- `6af2c793` (2026-10-14): abstraktní základ a první hlavička `Translation`, již po opravě původního návrhu.
- `3f9e7313` (2026-10-14): `Translation.cpp` s maticí posunutí.
- `ffe532b8` (2026-10-15): doplnění transformačních souborů a diagramů.
- `cfcc8bc7` (2026-10-15): `Rotation.h/.cpp` s `setAngle`.
- `709e3c86` (2026-10-15): `Scale.h/.cpp`.
- `615a8619` (2026-10-15): první Composite bez `return`.
- `9663e9e4` (2026-10-15): oprava `return result;` a `#include <utility>`.
- `e492c754` (2026-10-15): aktualizované diagramy zachycující transformační hierarchii.
- Diskuse v tomto chatu 2026-10-13 až 2026-10-15: jednotlivé odpovědi k asociativitě, pořadí matic, rekurzi, živému stavu, GLM a ownershipu.

**Poznámka k diagramům:** autogenerovaný UML zápis s agregací nad kontejnery není sám o sobě důkazem UML vlastnictví; to je doloženo konkrétními `unique_ptr` v implementaci.
