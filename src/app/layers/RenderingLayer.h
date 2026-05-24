#pragma once
#include "Layer.h"

class Application;

class RenderingLayer : public Layer {
public:
    explicit RenderingLayer(Application& app);
    bool OnUpdate(float dt) override;

private:
    Application& app;
};
