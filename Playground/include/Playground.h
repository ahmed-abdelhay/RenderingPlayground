#pragma once

#include "Resha.h"
#include <imgui.h>

namespace Resha
{
    struct View3DState
    {
        static constexpr size_t TEXTURE_WIDTH = 1024;
        static constexpr size_t TEXTURE_HEIGHT = 1024;
        size_t width = TEXTURE_WIDTH;
        size_t height = TEXTURE_HEIGHT;
        RenderBuffer buffer = CreateRenderBuffer(width, height);
        Color backgroundColor = Color{ 125, 125, 125, 255 };
        Program program;
        bool redraw = true;
        Camera camera;
        std::vector<MeshRenderInfo> surfacesRenderInfo;
    };

    struct State
    {
        std::vector<SurfaceMesh> meshes;
        View3DState view3d;
    };

    View3DState CreateView3D();
    void FitView3D(View3DState& view);
    void RenderView3D(ImVec2 area, const std::vector<SurfaceMesh>& meshes, View3DState& view);
    void RenderShaderEditor(View3DState& state, bool& open);

    // called every frame.
    void Startup(State& state);
    void Update(State& state);
}