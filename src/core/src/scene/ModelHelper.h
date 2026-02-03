#pragma once

#include "RenderableHelper.h"
#include "renderables/Model.h"
#include "renderer/Texture.h"

namespace SceneImporterInternal {
    class ModelHelper : public RenderableHelper {
    protected:
        // Helper to handle loading textures if present
        // This logic was previously inside FloorHelper but applies to any Model
    };
}
