#pragma once
#include "Layer.h"

class Application;

class StatsLayer : public Layer {
public:
    explicit StatsLayer(Application& app);
    void OnInit() override;
    bool OnUpdate(float dt) override;
    void OnGUI() override;
private:
    Application& app;
    const char* gpuName{nullptr};
    float mspf{0.0f};
    double lastFPSTime{0.0};
    unsigned int fpsFrameCount{0};
};
