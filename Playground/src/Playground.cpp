#include "Playground.h"

#include <imgui.h>
#include <ImGuiFileBrowser.h>
#include <GLFW/glfw3.h>
#include <TextEditor.h>

// surface with wireframes shaders
static const char* wires_fs = R"V0G0N(#version 330 core
out vec4 FragColor;
in vec3 dist;
uniform vec3 objectColor;

const float lineWidth = 0.5;

float edgeFactor()
{
    vec3 d = fwidth(dist);
    vec3 f = step(d * lineWidth, dist);
    return min(min(f.x, f.y), f.z);
}

void main()
{
    gl_FragColor = vec4(min(vec3(edgeFactor()), objectColor), 1.0);
}
)V0G0N";

static const char* wires_vs = R"V0G0N(#version 330 core
in vec4 position;
void main()
{
    gl_Position = position;
}
)V0G0N";

static const char* wires_gs = R"V0G0N(#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
uniform mat4 view;
uniform mat4 projection;
out vec3 dist;

void main()
{
    mat4 mvp = projection * view;
    vec4 p0 = mvp * gl_in[0].gl_Position;
    vec4 p1 = mvp * gl_in[1].gl_Position;
    vec4 p2 = mvp * gl_in[2].gl_Position;

    dist = vec3(1, 0, 0);
    gl_Position = p0;
    EmitVertex();

    dist = vec3(0, 1, 0);
    gl_Position = p1;
    EmitVertex();

    dist = vec3(0, 0, 1);
    gl_Position = p2;
    EmitVertex();

    EndPrimitive();
}
)V0G0N";

static const ImVec4 BLUE(41 / 255., 74 / 255., 122 / 255., 1);

namespace Resha
{

    View3DState CreateView3D()
    {
        View3DState result;
        std::string log;
        result.program = CreateProgram(wires_gs, wires_vs, wires_fs, log);
        assert(result.program.valid);
        return result;
    }

    void FitView3D(View3DState& view)
    {
        BBox b;
        for (const MeshRenderInfo& info : view.surfacesRenderInfo)
        {
            b = Merge(b, info.box);
        }
        if (IsBBoxValid(b))
        {
            CameraFitBBox(view.camera, b);
            view.redraw = true;
        }
    }

    void RenderView3D(ImVec2 area, const std::vector<SurfaceMesh>& meshes, View3DState& view)
    {
        ImGui::BeginChild("3D View", area);
        if (ImGui::IsWindowFocused())
        {
            ImGuiIO& io = ImGui::GetIO();
            // If any mouse button is pressed, trigger a redraw
            if (ImGui::IsAnyMouseDown()) view.redraw = true;

            // Handle scroll events for 3D view
            {
                const float offset = io.MouseWheel;
                if (offset)
                {
                    CameraProcessZoom(view.camera, offset);
                    view.redraw = true;
                }
            }

            // Mouse inputs
            {
                // Process drags
                const bool dragLeft = ImGui::IsMouseDragging(0);
                // left takes priority, so only one can be true
                const bool dragRight = !dragLeft && ImGui::IsMouseDragging(1);

                if (dragLeft || dragRight)
                {
                    const Vec2d dragDelta{ io.MouseDelta.x / view.width,
                                          io.MouseDelta.y / view.height };
                    // exactly one of these will be true
                    const bool isRotate = dragLeft && !io.KeyShift && !io.KeyCtrl;
                    const bool isTranslate = (dragLeft && io.KeyShift && !io.KeyCtrl) || dragRight;
                    const bool isDragZoom = dragLeft && io.KeyShift && io.KeyCtrl;

                    if (isDragZoom)
                    {
                        CameraProcessZoom(view.camera, dragDelta.y * 5);
                    }
                    if (isRotate)
                    {
                        const Vec2d currPos{
                            2 * (io.MousePos.x / view.width) - 1.0,
                            2 * ((view.height - io.MousePos.y) / view.height) - 1.0 };
                        CameraProcessRotate(view.camera, currPos - (dragDelta * 2.0f), currPos);
                    }
                    if (isTranslate)
                    {
                        CameraProcessTranslate(view.camera, dragDelta);
                    }
                }
            }

            // reset best fit zoom.
            if (ImGui::IsKeyPressed(GLFW_KEY_R))
            {
                FitView3D(view);
            }
        }

        const bool sizeChanged = view.width != size_t(area.x) || view.height != size_t(area.y);
        if (sizeChanged)
        {
            // update texture dimensions.
            view.width = area.x;
            view.height = area.y;
            ReizeRenderBuffer(view.buffer, view.width, view.height);
        }

        if (view.redraw || sizeChanged)
        {
            view.redraw = false;
            ClearRenderBuffer(view.buffer, view.backgroundColor);

            const Mat4 viewMatrix = CameraGetViewMatrix(view.camera);
            const Mat4 projectionMatrix = CameraGetProjectionMatrix(view.camera, view.width, view.height);
            float projectionMatrixData[16] = {};
            float viewMatrixData[16] = {};
            for (int i = 0; i < 16; ++i)
            {
                projectionMatrixData[i] = projectionMatrix.data[i];
                viewMatrixData[i] = viewMatrix.data[i];
            }
            // pass the parameters to the shader
            // lighting
            const Vec3f lightPos{ 1.2f, 1.0f, 2.0f };
            const Vec3f lighColour{ 1.2f, 1.0f, 2.0f };
            SetProgramUniformM4x4f(view.program, "projection", projectionMatrixData);
            SetProgramUniformM4x4f(view.program, "view", viewMatrixData);
            SetProgramUniformV3f(view.program, "lightPos", lightPos.data);
            SetProgramUniformV3f(view.program, "lightColor", lighColour.data);


            for (const MeshRenderInfo& info : view.surfacesRenderInfo)
            {
                for (const SurfaceMesh& mesh : meshes)
                {
                    if (mesh.id == info.id && mesh.visible)
                    {
                        const float meshColor[3]
                        {
                            mesh.color.r / 255.f,
                            mesh.color.g / 255.f,
                            mesh.color.b / 255.f
                        };
                        SetProgramUniformV3f(view.program, "objectColor", meshColor);
                        RenderMesh(view.buffer, view.program, info);
                    }
                }
            }
        }
        ImGui::Image((ImTextureID)(intptr_t)view.buffer.textureId, area);
        ImGui::EndChild();
    }

    void RenderShaderEditor(View3DState& view3d, bool& open)
    {
        struct ShaderEditorState
        {
            TextEditor editor;
            std::string gs, vs, fs;
            int32_t activeProgram = -1;
            Program program;
            std::string log;
            int tab = -1;
        };
        static constexpr int GS_TAB = 0;
        static constexpr int VS_TAB = 1;
        static constexpr int FS_TAB = 2;

        static ShaderEditorState state;
        state.editor.SetColorizerEnable(false);
        if (state.activeProgram != view3d.program.id)
        {
            state.activeProgram = view3d.program.id;
            state.gs = view3d.program.geometryShader;
            state.vs = view3d.program.vertexShader;
            state.fs = view3d.program.fragmentShader;
            state.program = CreateProgram(state.gs.c_str(),
                                          state.vs.c_str(),
                                          state.fs.c_str(),
                                          state.log);
        }

        ImGui::Begin("Graphics editor", &open);
        const ImVec2 minPoint = ImGui::GetWindowContentRegionMin();
        const ImVec2 maxPoint = ImGui::GetWindowContentRegionMax();
        const float width = (maxPoint.x - minPoint.x);
        const float height = (maxPoint.y - minPoint.y);

        if (ImGui::BeginTabBar("Shaders"))
        {
            if (ImGui::BeginTabItem("Geometry shader"))
            {
                if (state.tab != GS_TAB)
                {
                    state.editor.SetText(state.gs);
                    state.tab = GS_TAB;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Vertex shader"))
            {
                if (state.tab != VS_TAB)
                {
                    state.editor.SetText(state.vs);
                    state.tab = VS_TAB;
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Fragment shader"))
            {
                if (state.tab != FS_TAB)
                {
                    state.editor.SetText(state.fs);
                    state.tab = FS_TAB;
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        state.editor.Render("ShaderEditor", ImVec2(width, height * 0.7));
        if (state.tab == GS_TAB)
        {
            state.gs = state.editor.GetText();
        }
        else if (state.tab == VS_TAB)
        {
            state.vs = state.editor.GetText();
        }
        else if (state.tab == FS_TAB)
        {
            state.fs = state.editor.GetText();
        }

        if (ImGui::Button("Compile"))
        {
            if (state.tab == GS_TAB)
            {
                CompileShader(state.gs.c_str(), ShaderType::GEOMETRY, state.log);
            }
            else if (state.tab == VS_TAB)
            {
                CompileShader(state.vs.c_str(), ShaderType::VERTEX, state.log);
            }
            else if (state.tab == FS_TAB)
            {
                CompileShader(state.fs.c_str(), ShaderType::FRAGMENT, state.log);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Link"))
        {
            state.program = CreateProgram(state.gs.c_str(),
                                          state.vs.c_str(),
                                          state.fs.c_str(),
                                          state.log);
            if (state.program.valid)
            {

                view3d.program = state.program;
                view3d.redraw = true;
            }
        }

        if (state.log.empty())
        {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Success");
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failure");
            ImGui::Text(state.log.c_str());
        }
        ImGui::End();
    }

    // end object list functions.
    bool LoadMesh(const char* fileName, State& state)
    {
        SurfaceMesh mesh;
        if (ReadMesh(fileName, mesh) == IOStatus::OK)
        {
            state.meshes.push_back(mesh);
            state.view3d.surfacesRenderInfo.push_back(CreateSurfaceMeshRenderInfo(mesh));
            FitView3D(state.view3d);
            state.view3d.redraw = true;
            return true;
        }
        return false;
    }

    void DrawDocumentsBoard(State& state)
    {
        const ImVec2 minPoint = ImGui::GetWindowContentRegionMin();
        const ImVec2 maxPoint = ImGui::GetWindowContentRegionMax();
        const float width = (maxPoint.x - minPoint.x);
        const float height = (maxPoint.y - minPoint.y);

        ImGui::BeginChild("Tools and Lists", ImVec2(width * 0.2, height));
        {
            ImGui::TextColored(BLUE, "Surfaces");
            for (SurfaceMesh& mesh : state.meshes)
            {
                ImGui::Spacing();
                if (ImGui::Checkbox(mesh.name.c_str(), &mesh.visible))
                {
                    state.view3d.redraw = true;
                }
                ImGui::SameLine();
                ImGui::PushID(mesh.name.c_str());
                float color[3] = { mesh.color.r / 255.0f,mesh.color.g / 255.0f,mesh.color.b / 255.0f };
                if (ImGui::ColorEdit3("Colour", color,
                                      ImGuiColorEditFlags_NoInputs |
                                      ImGuiColorEditFlags_NoLabel))
                {
                    mesh.color.r = color[0] * 255;
                    mesh.color.g = color[1] * 255;
                    mesh.color.b = color[2] * 255;
                    state.view3d.redraw = true;
                }
                ImGui::PopID();
            }
        }
        ImGui::Text("Application average: %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::EndChild();
        ImGui::SameLine();
        RenderView3D(ImVec2(width * 0.8, height), state.meshes, state.view3d);
    }

    void Startup(State& state)
    {
        state.view3d = CreateView3D();
        LoadMesh("d:/bunny.stl", state);
    }

    void Update(State& state)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = style.GrabRounding = 12;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        bool openPopup = false;
        ImGui::Begin("Viewer", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar |
                     ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::MenuItem("Open", nullptr, &openPopup);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Dark")) ImGui::StyleColorsDark();
                if (ImGui::MenuItem("Classic")) ImGui::StyleColorsClassic();
                if (ImGui::MenuItem("Light")) ImGui::StyleColorsLight();
                ImGui::EndMenu();
            }
        }

        if (openPopup)
        {
            ImGui::OpenPopup("Open STL file");
        }

        static imgui_addons::ImGuiFileBrowser fileDialog;
        if (fileDialog.showFileDialog("Open STL file", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".stl"))
        {
            LoadMesh(fileDialog.selected_path.c_str(), state);
        }
        ImGui::EndMenuBar();
        DrawDocumentsBoard(state);
        ImGui::End();

        static bool openGraphicsEditor = false;
        if (ImGui::IsKeyPressed(GLFW_KEY_X))
        {
            openGraphicsEditor = true;
        }

        if (openGraphicsEditor)
        {
            RenderShaderEditor(state.view3d, openGraphicsEditor);
        }
    }
}
