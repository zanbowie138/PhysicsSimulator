#pragma once
#include "Layer.h"

class Application;

class StatsLayer : public Layer {
public:
    explicit StatsLayer(Application& app);
    void OnGUI() override;
private:
    Application& app;
};
