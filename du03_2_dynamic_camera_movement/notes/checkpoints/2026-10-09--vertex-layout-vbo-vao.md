# Learning checkpoint – vertex layout, stride a offset

**Datum:** 2026-10-09  
**Navazuje na code commit:** `30605002520ec4aaaf987dacdf8c18add114bf6b`

## Stručný mentální model

Aktuální vertex má pevný layout:

```text
x y z | r g b
pozice | barva
```

Jeden vertex tedy obsahuje **6 hodnot typu `float`**.

Je potřeba rozlišovat tři různé jednotky:

```text
vertexCount                         = počet vrcholů
vertexCount * 6                     = počet float hodnot
vertexCount * 6 * sizeof(float)     = velikost dat v bajtech
```

Proto:

- `glBufferData(..., dataSize, ...)` dostává **velikost v bajtech**;
- `glDrawArrays(..., vertexCount)` dostává **počet vrcholů**.

## Jak VAO popisuje jeden vertex

Pro pozici:

```cpp
glVertexAttribPointer(
    0,                  // location 0
    3,                  // x, y, z
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(float),  // stride: celý vertex
    (GLvoid*)0          // offset: pozice začíná na začátku
);
```

Pro barvu:

```cpp
glVertexAttribPointer(
    1,                         // location 1
    3,                         // r, g, b
    GL_FLOAT,
    GL_FALSE,
    6 * sizeof(float),         // další vertex je o 6 floatů dál
    (GLvoid*)(3 * sizeof(float)) // barva začíná po x,y,z
);
```

Zapamatovat si:

```text
stride = vzdálenost mezi začátky stejného atributu ve dvou sousedních vertexech
offset = vzdálenost od začátku vertexu k začátku konkrétního atributu
```

V našem layoutu:

```text
| x y z r g b | x y z r g b |
^       ^       ^
0       color   další vertex

stride pozice = 6 * sizeof(float)
stride barvy  = 6 * sizeof(float)
offset pozice = 0
offset barvy  = 3 * sizeof(float)
```

## Samostatně prokázáno

- Rozpoznal jsem, že `vertexCount` je počet vrcholů modelu a `Model::draw()` jej může použít jako svůj členský stav.
- Po předchozí opravě jsem správně formuloval, že **6 floatů patří jednomu vrcholu**.

## Pochopeno s vedením

- Rozdíl mezi počtem vertexů, počtem `float` hodnot a počtem bajtů.
- Význam `stride` a `offset` v `glVertexAttribPointer`.
- Proč jsou všechna tři aktuální použití `sizeof(float)` správná:
  - `vertexCount * 6 * sizeof(float)` = celé VBO v bajtech,
  - `6 * sizeof(float)` = stride jednoho vertexu,
  - `3 * sizeof(float)` = offset barvy.

## Vývoj pochopení

Původně byl `vertexCount` zaměněn s množstvím dat nahrávaných do VBO. Nyní je odděleno, že jeden vertex je logická jednotka pro kreslení, ale fyzicky je v bufferu reprezentován několika hodnotami a `glBufferData` pracuje s bajty.

## Potřebuje upevnit

- Samostatně odvodit stride a offset u jiného layoutu, například `position + normal + texture coordinates`.
- Upevnit vztah mezi atributy VAO a odpovídajícími `layout(location = ...)` vstupy ve vertex shaderu.

## Další procvičení

Při příštím jiném vertex layoutu bez nápovědy určit:

1. počet hodnot na vertex,
2. velikost jednoho vertexu v bajtech,
3. stride,
4. offset každého atributu.

## Důkazy

- Commit `3060500`: oprava `dataSize` z `vertexCount * sizeof(float)` na `vertexCount * 6 * sizeof(float)`.
- V diskusi bylo následně správně formulováno: „6 floatů patří jednomu vrcholu“.
