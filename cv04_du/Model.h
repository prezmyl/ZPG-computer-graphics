#pragma once
class Model {
public:
    virtual ~Model() = default;
    virtual void draw() const = 0;
};