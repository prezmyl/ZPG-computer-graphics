classDiagram
  %% ===== Render vrstva =====
  class ShaderProgram {
    +use()
    +setUniform(name, mat4)
    +setUniform(name, vec3)
    +setUniform(name, float)
    +setUniform(name, int)
    +static fromSources(vsSrc, fsSrc)
  }

  class Model {
    <<interface>>
    +draw()*
  }
  class TriangleModel {
    +draw()
  }
  Model <|-- TriangleModel

  class AbstractTransformation {
    <<interface>>
    +matrix()* : mat4
  }
  class Translate { +matrix() : mat4 }
  class Rotate { +matrix() : mat4 }
  class Scale { +matrix() : mat4 }
  AbstractTransformation <|-- Translate
  AbstractTransformation <|-- Rotate
  AbstractTransformation <|-- Scale

  class TransformChain {
    +add(t: shared_ptr<AbstractTransformation>)
    +combined() : mat4
  }
  TransformChain "1" o-- "*" AbstractTransformation : agregace

  class DrawableObject {
    +model : Model*
    +program : ShaderProgram*
    +transform : shared_ptr<TransformChain>
    +draw(V:mat4, P:mat4)
  }
  DrawableObject "1" o-- "1" Model : agregace
  DrawableObject "1" o-- "1" ShaderProgram : agregace
  DrawableObject "1" o-- "1" TransformChain : agregace

  %% ===== Scény =====
  class Scene {
    <<abstract>>
    +update(dt:float)
    +render(V:mat4, P:mat4)
  }
  class SimpleScene {
    +add(obj: unique_ptr<DrawableObject>)
    +render(V,P)
  }
  class RotatingTriangleScene {
    +update(dt)
    +render(V,P)
  }
  class EmptyScene {
    +render(V,P)
  }
  Scene <|-- SimpleScene
  SimpleScene <|-- RotatingTriangleScene
  Scene <|-- EmptyScene
  SimpleScene "1" *-- "*" DrawableObject : kompozice

  class SceneManager {
    +add(scene: unique_ptr<Scene>)
    +switchTo(index:int)
    +active() : Scene
  }
  SceneManager "1" *-- "*" Scene : kompozice

  %% vztahy navíc ve specializované scéně
  RotatingTriangleScene o-- ShaderProgram : agregace
  RotatingTriangleScene *-- TriangleModel : kompozice

