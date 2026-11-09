# Kamera v ZPG: vlastnictví, pohled/projekce a tok aktualizací

**Původní stav:** 2026-11-05, GitHub `3b218847cb64949c33ee4575c884aaa0c904b6b2`; **aktualizováno:** 2026-11-09, GitHub `5a972baa`. Zdrojový stav byl přezkoumán, uživatel potvrdil běžný funkční test WASD + myši, nezávislý runtime test asistent neprovedl.

## Mentální model (co a proč)

Kamera je **stav pozorovatele**, ne GPU zdroj. Uchovává `position`, `direction`, `up` a parametry projekce `fov`, `aspectRatio`, `near`, `far`. Její výsledné matice se odvozují z tohoto stavu: `view = glm::lookAt(position, position + direction, up)`, `projection = glm::perspective(radians(fov), aspectRatio, near, far)`. Vektor `direction` není bod `target`; pro míření na daný světový bod platí `normalize(target-position)`. Nulový `direction` nevytváří platný směr pohledu.

Kamera jako C++ objekt nevyžaduje speciální cleanup OpenGL prostředků. To **neznamená**, že nemá vlastníka: její stav musí mít smysluplnou životnost.

## UML a C++: dvě roviny stejného rozhodnutí

```mermaid
classDiagram
    Application "1" *-- "0..*" Scene : vlastní
    Scene "1" *-- "1" Camera : vlastní hodnotou
    Application ..> Camera : používá přes aktivní scénu
    Camera ..> glm : počítá matice
    Application ..> ShaderProgram : předává uniformy
```

V aktuálním projektu:

```cpp
class Scene {
    Camera camera;             // kompozice: vlastní instance
public:
    Scene(const Camera& initial) : camera(initial) {}
    Camera& getCamera() { return camera; } // nevlastnící přístup k této instanci
};
```

- **Člen `Camera camera`** drží samostatnou identitu uvnitř `Scene`. Když zanikne scéna, zanikne kamera.
- **Parametr `const Camera&`** pouze čte počáteční zdroj; `camera(initial)` vytvoří kopii do vlastněného členu. Jediná lokální počáteční kamera proto může inicializovat tři nezávislé scény.
- **Návratový `Camera&`** nevrací kopii ani vlastnictví; klient pracuje přímo s kamerou uvnitř scény. Reference je platná pouze během života dané scény.
- **Nesprávné `Camera& camera` jako člen** v původní verzi scén odkazovalo na lokální kameru vytvořenou v `Application::createResources()`; po návratu z funkce zůstaly visící reference. Samotný `Camera&` vlastnictví nezajistí.
- **`std::unique_ptr<Camera>`** by také mohl vyjadřovat kompozici, ale není zde potřeba dynamické vytvoření, výměna, volitelná nepřítomnost nebo polymorfismus. Volbu mechanismu odvozujeme ze sémantiky a lifetime, ne obráceně.

## Kdo dělá co: od okna ke GPU

```text
GLFW framebuffer resize
  -> Application callback (zjistí nové rozměry)
      -> glViewport (OpenGL vykreslovací oblast)
      -> Camera::resize(width, height) pro všechny relevantní scény
          -> aktualizace aspectRatio (CPU stav)

Application::run() [každý snímek]
  -> zvolí aktivní Scene
  -> Scene::update(time)
  -> Camera::getViewMatrix(), getProjectionMatrix() [CPU výpočet]
  -> ShaderProgram::use() + setMat4(...) [přenos uniformů na GPU]
  -> Scene::draw() [GPU použije matice při vykreslení]
```

**Současný stav (9. 11.):** callback již používá velikost framebufferu, nastaví `glViewport` a aktualizuje `Camera::resize()` u všech scén, včetně ochrany nulových rozměrů. Matice se nahrávají před vykreslením v každém snímku. WASD a myš jsou zapojeny; detailní rozbor je v `ovladani-kamery-vstupy-cas-a-orientace.md`. Při přechodu na více osvětlovacích shader programů musí být view/projection aktuální v každém programu, který danou scénu vykresluje.

**Proč nyní ne Observer:** při jednoduchém render loop stačí pravidelně přečíst aktuální stav, bez registrace a životnosti subscriberů. Observer dává smysl, až když konkrétní potřeba ukáže výhodu notifikací.

**Výstup `glViewport` ≠ projekce:** změna viewportu sama neopraví `aspectRatio`; změna projekce sama neupraví vykreslovací oblast. Pro HiDPI zařízení preferuj framebuffer-size callback a nezapomeň na nulovou výšku minimalizovaného framebufferu.

## Přenositelné zásady

1. **Návrhový vztah je první, C++ mechanismus druhý:** UML kompozice je vlastnictví a životnost, ne povinně `unique_ptr`.
2. **Parametr vs. uložený člen vs. návratová reference** vyjadřují jiné role. Vlastnictví se rozhoduje v místě uložení, nikoli volbou `&` v signatuře konstruktoru.
3. **Změna CPU stavu ≠ změna GPU stavu.** Po změně kamery je potřeba nové uniformy nahrát ve správném shader programu před kreslením.
4. **Hlavička tvoří část závislostního grafu.** Hodnotový člen potřebuje úplnou definici typu. Nadbytečná závislost `Camera.h -> GLFW` vedla ke konfliktu `GL/gl.h` vs. GLAD v jiných překladových jednotkách.
5. **Koordinátor řídí kdy, specializovaný objekt ví jak:** `Application` řeší GLFW a pořadí render loop, `Camera` matematiku, `ShaderProgram` GL uniformy.

## Otevřené ověřovací úkoly

- Uživatel potvrdil funkční zúžení/rozšíření okna a WASD + myš, bez samostatného testu asistenta. Přesná inicializace pro HiDPI a minimalizaci zůstává nevyhodnocená.
- Dvě scény mají ve výchozím stavu stejnou kameru jako nezávislé kopie; cílený test, že si po odlišném pohybu uchovají samostatný stav, zůstává otevřený.
- Při současném diagonálním pohybu, přesné vertikální orientaci a znovuzískání fokusu ověřit hraniční chování. Samostatné odvození yaw/pitch a příslušných jednotek je dále k procvičení.
- Build a běh **asistentem** stále nejsou doložené.
