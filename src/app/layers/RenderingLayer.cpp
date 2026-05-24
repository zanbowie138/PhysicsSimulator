#include "RenderingLayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "core/World.h"
#include "renderer/RenderSystem.h"
#include "components/Components.h"
#include "utils/Logger.h"

extern World world;

RenderingLayer::RenderingLayer(Application& app) : app(app) {}

bool RenderingLayer::OnUpdate(float dt) {
    glm::vec3 lightPos(0.0f, 1.0f, 0.0f);
    try {
        Entity light = app.GetLightEntity();
        if (world.GetEntitySignature(light).test(
                world.GetComponentType<Components::Transform>()))
            lightPos = world.GetComponent<Components::Transform>(light).worldPos;
    } catch (const std::exception& e) {
        LOG(LOG_ERROR) << "Error getting light transform: " << e.what() << "\n";
    }

    app.GetUBO().UpdateData(app.GetCamera(), lightPos);

    static constexpr float kShadowSize = 20.0f;
    glm::mat4 lightProj = glm::ortho(-kShadowSize, kShadowSize,
                                      -kShadowSize, kShadowSize, 0.1f, 100.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    auto renderSystem = app.GetRenderSystem();
    renderSystem->SetLightSpaceMatrix(lightProj * lightView);
    renderSystem->ShadowPass();
    renderSystem->Update();

    return false;
}
