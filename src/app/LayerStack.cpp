#include "LayerStack.h"

LayerStack::~LayerStack() {
    for (auto& layer : mLayers)
        layer->OnDetach();
}

void LayerStack::PushLayer(std::unique_ptr<Layer> layer) {
    layer->OnAttach();
    mLayers.push_back(std::move(layer));
}

void LayerStack::OnUpdate(float dt) {
    for (auto& layer : mLayers)
        if (layer->OnUpdate(dt))
            break;
}

void LayerStack::OnGUI() {
    for (auto& layer : mLayers)
        layer->OnGUI();
}
