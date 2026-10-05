classDiagram
  %% Modely
  class Model
  class Triangle
  Model <|-- Triangle

  %% Shadery
  class Shader
  class VertexShader
  class FragmentShader
  Shader <|-- VertexShader
  Shader <|-- FragmentShader

  class ShaderProgram
  ShaderProgram "1" *-- "2" Shader : composes

  %% Transformace
  class AbstractTransformation
  class Rotate
  class Translate
  class Scale
  AbstractTransformation <|-- Rotate
  AbstractTransformation <|-- Translate
  AbstractTransformation <|-- Scale

  class TransformChain
  TransformChain "1" o-- "*" AbstractTransformation : holds (ordered)

  %% Drawable
  class DrawableObject
  DrawableObject "1" o-- "1" Model
  DrawableObject "1" o-- "1" ShaderProgram
  DrawableObject "1" o-- "1" TransformChain

