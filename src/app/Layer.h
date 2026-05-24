#pragma once

class Layer {
public:
    virtual ~Layer() = default;
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    // Returns true to consume the update and stop propagation to subsequent layers
    virtual bool OnUpdate(float dt) { return false; }
    virtual void OnGUI() {}
};
