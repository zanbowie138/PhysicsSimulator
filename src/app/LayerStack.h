#pragma once
#include <vector>
#include <memory>
#include "Layer.h"

class LayerStack {
public:
    ~LayerStack();
    void PushLayer(std::unique_ptr<Layer> layer);
    void OnUpdate(float dt);
    void OnGUI();
private:
    std::vector<std::unique_ptr<Layer>> mLayers;
};
