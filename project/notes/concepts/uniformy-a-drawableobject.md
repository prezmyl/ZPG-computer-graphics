# Uniformy, modelMatrix a DrawableObject (DU02)

Tato poznámka rozšiřuje starší `opengl-objekty-a-odpovednosti.md`. Stav odpovídá 15. 10. 2026: jedna vykreslovaná instance čtverce s `Translation`; student potvrdil funkční běh.

## Co je zde konkrétně vyřešeno

### Parametr transformace → uniform → vertex shader

`Translation` uchovává `glm::vec3(0.25f, 0, 0)`. `getMatrix()` vrací `glm::mat4`. `DrawableObject::draw()` předá matici přes obecné `ShaderProgram::setMat4("modelMatrix", matrix)`.

V `setMat4`:

1. `glGetUniformLocation(id, name)` vrátí lokaci v **konkrétním slinkovaném programu**, ne adresu či vlastní hodnotu matice. `-1` může znamenat chybějící nebo neaktivní uniform.
2. `glm::value_ptr(matrix)` poskytne `const float*` na 16 elementů matice.
3. `glUniformMatrix4fv(location, 1, GL_FALSE, ptr)` nastaví jednu matici v **právě aktivním programu**; `use()` musí být zavoláno před tímto voláním. `GL_FALSE` ponechává obvyklou GLM/OpenGL orientaci bez transpozice.

Ve vertex shaderu:

```glsl
uniform mat4 modelMatrix;
gl_Position = modelMatrix * vec4(position, 1.0);
```

`in vec3 position` se liší vrchol od vrcholu; uniform je pro dané použití programu při draw call společná hodnota. Zatím neexistuje view/projection řetězec; `gl_Position` dostává transformované souřadnice přímo do clip space.

### Proč ShaderProgram přijímá jméno uniformu

`ShaderProgram` vlastní OpenGL program ID a provádí obecnou API operaci `setMat4(name, value)`. Nepatří do něj trvalá modelová transformace jedné instance – dva `DrawableObject` mohou použít stejný program a přesto mu před jednotlivými draw cally poslat odlišné matice.

## Rozdělení odpovědností

- `Application`: vytváří prostředky a řídí běh; aktuálně vlastní `unique_ptr<Model>`, `unique_ptr<ShaderProgram>` a `unique_ptr<DrawableObject>`.
- `DrawableObject`: vlastní `unique_ptr<Transformation>`; pouze používá `Model*` a `ShaderProgram*`. Získá matici, aktivuje shader, nastaví uniform, deleguje geometrický draw.
- `Model`: vlastní VAO/VBO a počet vertexů; pouze jeho `draw()` ví, jak je vykreslit.
- `CompositeTransformation`: stále umí spojit mnoho transformací, ale `DrawableObject` o konkrétním typu transformace nerozhoduje.

Pracovní způsob vzniku:

```cpp
auto move = std::make_unique<Translation>(glm::vec3(0.25f, 0.0f, 0.0f));
drawableObject = std::make_unique<DrawableObject>(
    model.get(), shaderProgram.get(), std::move(move)
);
```

`.get()` pouze půjčí adresu a **nepřenese vlastnictví** `Model` nebo `ShaderProgram`; `std::move(move)` předá vlastnictví `Translation`. Vnitřní `unique_ptr<Transformation>` pak zajišťuje polymorfní životní cyklus.

**Chybný aktuální komentář v destruktoru:** text, že `DrawableObject` vlastní model a program, neodpovídá návrhu; vlastní pouze transformaci. Správné pořadí zániku je `DrawableObject → ShaderProgram/Model → OpenGL context` kvůli platnosti nevlastnících odkazů a dostupnému GPU contextu, nikoli proto, že by `DrawableObject` vlastnila GPU objekty.

## Původní slepé cesty

- Přímé `glBindVertexArray` a `glDrawArrays` z `DrawableObject` zbytečně obešly `Model::draw()` a vedly k vystavování VAO/VBO getterů. Toto bylo vráceno.
- `vector<unique_ptr<Transformation>>` v `DrawableObject` duplikoval odpovědnost `CompositeTransformation`. Nahrazeno jedním `unique_ptr<Transformation>`.
- První konstrukce `DrawableObject` v `Application` se pokusila předávat `unique_ptr<Model>` a `unique_ptr<ShaderProgram>` tam, kde byly očekávány nevlastnící `Model*` a `ShaderProgram*`.
- Při testování `Translation` nebyl problém v matici. `file(COPY ...)` v CMake kopíruje GLSL při konfiguraci; starý shader v build adresáři se přestal používat až po `Reload CMake`. Při ladění externích dat rozlišovat source/build/runtime soubory.

## Otevřené otázky

Bez nových závěrů: dosud nebyly otestovány dvě instance sdílející geometrii, Composite jako vlastněná transformace v renderu, změna úhlu za běhu, délka života nevlastnících pointerů ve složitější scéně ani M/V/P řetězec. Není důvod zavádět `Scene` jen podle vzoru, dokud nevznikne skutečná potřeba koordinace více instancí.
