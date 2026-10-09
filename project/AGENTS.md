# ZPG – pravidla práce a learning checkpointů

Tento soubor platí pro obsah adresáře `project/`.

Projekt slouží současně k vývoji semestrálního projektu ZPG a k průběžnému zachycování toho, jak se vyvíjí studentovo pochopení počítačové grafiky, OpenGL, GLSL, GLM a souvisejícího C++ návrhu.

## Jazyk

Veškerou learning dokumentaci, checkpointy, mapu znalostí a vysvětlující poznámky piš česky.
Názvy API, tříd, funkcí, knihoven a odborné termíny lze ponechat v originále, pokud je to přirozenější.

## Učitelský režim

Student má řešení primárně vytvářet sám.

Preferuj:
- vysvětlení principu,
- otázky a nápovědy,
- rozbor studentova kódu,
- upozornění na chybný mentální model,
- malé a pochopitelné změny.

Nepřepisuj řešení do kompletní hotové podoby, pokud o to student výslovně nepožádá.

## Git a zdrojový kód

Git root je nad tímto adresářem; lokálně odpovídá `/home/xpolas/banska/ZPG`.
Vývojový projekt je v `project/`.

Git historie má zachycovat smysluplné etapy vývoje kódu.
Learning dokumentace má být pokud možno samostatný dokumentační commit navazující na relevantní code commit.

Během learning checkpointu neměň zdrojový kód řešení, pokud o to uživatel výslovně nepožádá.

## Příkaz `checkpoint`

Když uživatel v kontextu ZPG napíše `checkpoint`, jde o dohodnutý learning checkpoint.

Checkpoint není pouze souhrn kódu. Má zachytit změnu studentova pochopení a propojit ji s relevantní Git historií.

Použij dostupné podklady:
1. diskusi v aktuálním chatu,
2. aktuální soubory v `project/`,
3. relevantní commity a diffy,
4. předchozí checkpointy,
5. mapu znalostí v `project/README.md`,
6. obecné poznámky v `project/notes/concepts/`.

## Co má checkpoint vytvořit

Pro významnou etapu vytvoř nebo aktualizuj:

1. datovaný soubor v `project/notes/checkpoints/`, typicky
   `YYYY-MM-DD--kratke-tema.md`;
2. podle potřeby obecnou poznámku v `project/notes/concepts/`;
3. `project/README.md` jako dlouhodobou mapu aktuálního stavu znalostí.

Každý checkpoint má podle dostupných důkazů rozlišit:

### Samostatně prokázáno
Co student skutečně vysvětlil, navrhl nebo implementoval bez dodání klíčové myšlenky.

### Pochopeno s vedením
Co začalo dávat smysl až po nápovědě, vysvětlení nebo opravě.

### Vývoj pochopení
Stručně zachyť:
- původní představu,
- co nebylo jasné nebo bylo chybně chápáno,
- co způsobilo změnu pohledu,
- jak student princip chápe na konci etapy.

### Potřebuje upevnit
Co je stále nejisté, opakovaně problematické nebo zatím nebylo samostatně prokázáno.

### Další procvičení
Konkrétní věci, na kterých se dá později ověřit, zda se pochopení upevnilo.

### Důkazy
Uveď relevantní commit hash, změnu kódu nebo bod z diskuse, pokud pomáhá zdůvodnit hodnocení.

## Zásady hodnocení

- Neodvozuj zvládnutí tématu pouze z toho, že správný kód existuje.
- Pokud klíčovou myšlenku dodal asistent, zařaď ji nejprve mezi „Pochopeno s vedením“, dokud ji student později samostatně neprokáže.
- Pokud nejsou dostatečné důkazy, napiš „zatím nevyhodnoceno“ místo odhadu.
- Zachovávej užitečné omyly a slepé cesty, pokud vysvětlují vývoj mentálního modelu.
- Root `project/README.md` je aktuální mapa stavu, nikoli chronologický deník.
- Datované checkpointy tvoří historii vývoje pochopení.
