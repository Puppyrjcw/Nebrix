// headers

#include "UILayer.h"
#include "../Scene.h"
#include "../SceneNode.h"
#include "../Renderer.h"
#include "../Camera.h"
#include "../Part.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imguizmo/ImGuizmo.h"
#include "imguizmo/ImCurveEdit.h"
#include "imguizmo/ImGradient.h"
#include "imguizmo/ImSequencer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLFW/glfw3.h"

// utils

#include <string>
#include <vector>
#include <algorithm>

// nebrix theme

static void ApplyNebrixTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0.0f;
    s.ChildRounding = 4.0f;
    s.FrameRounding = 4.0f;
    s.GrabRounding = 4.0f;
    s.TabRounding = 4.0f;
    s.ScrollbarRounding = 4.0f;
    s.WindowPadding = ImVec2(8, 8);
    s.FramePadding = ImVec2(6, 4);
    s.ItemSpacing = ImVec2(6, 4);
    s.IndentSpacing = 16.0f;
    s.ScrollbarSize = 12.0f;
    s.TabBarBorderSize = 0.0f;

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1);
    c[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1);
    c[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.15f, 1);
    c[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1);
    c[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1);
    c[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.31f, 0.31f, 1);
    c[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1);
    c[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1);
    c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 1);
    c[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1);
    c[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1);
    c[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1);
    c[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.13f, 1);
    c[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1);
    c[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.47f, 0.91f, 1);
    c[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.38f, 0.75f, 1);
    c[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1);
    c[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.28f, 1);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1);
    c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1);
    c[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.47f, 0.91f, 1);
    c[ImGuiCol_SliderGrab] = ImVec4(0.18f, 0.47f, 0.91f, 1);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.14f, 0.38f, 0.75f, 1);
    c[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1);
    c[ImGuiCol_SeparatorHovered] = ImVec4(0.18f, 0.47f, 0.91f, 1);
    c[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.38f, 0.75f, 1);
    c[ImGuiCol_DockingPreview] = ImVec4(0.18f, 0.47f, 0.91f, 0.7f);
    c[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.10f, 1);
    c[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1);
    c[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.45f, 0.45f, 1);
    c[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1);
}

// init / shutdown

void UILayer::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ApplyNebrixTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void UILayer::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// frame begin / frame end

void UILayer::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags hostFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Host", nullptr, hostFlags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(ImGui::GetID("##Dockspace"), ImVec2(0, 0));
    ImGui::End();
}

void UILayer::End() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// draw panels

void UILayer::DrawPanels(Scene* scene, Renderer* renderer,
    Camera* camera, GLFWwindow* window, float dt) {
    DrawMenuBar(scene);
    DrawHierarchy(scene);
    DrawProperties(scene);
    DrawScriptEditor(scene);
    DrawConsole(scene);
    DrawViewport(scene, renderer, camera, window, dt);
}

// menu bar

void UILayer::DrawMenuBar(Scene* scene) {
    ImGui::BeginMainMenuBar();

    // file

    if (ImGui::BeginMenu("File")) {
        ImGui::MenuItem("New");
        ImGui::MenuItem("Open");
        ImGui::MenuItem("Save");
        ImGui::Separator();
        ImGui::MenuItem("Exit");
        ImGui::EndMenu();
    }

    // edit

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, scene->GetUndo().CanUndo()))
            scene->GetUndo().DoUndo();
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, scene->GetUndo().CanRedo()))
            scene->GetUndo().DoRedo();
        ImGui::Separator();
        if (ImGui::MenuItem("Group into Model", "Ctrl+G", false, !m_Selected.empty()))
            scene->GroupIntoModel(m_Selected);
        if (ImGui::MenuItem("Group into Folder", "", false, !m_Selected.empty()))
            scene->GroupIntoFolder(m_Selected);
        if (ImGui::MenuItem("Ungroup", "", false,
            m_Primary &&
            (m_Primary->Type == NodeType::Model ||
                m_Primary->Type == NodeType::Folder)))
            scene->Ungroup(m_Primary);
        ImGui::EndMenu();
    }

    ImGui::Separator();

    // tool buttons
    const char* tools[] = { "Select","Move","Scale","Rotate" };
    for (int i = 0; i < 4; i++) {
        bool active = ((int)m_Tool == i);
        if (active)
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.47f, 0.91f, 1));
        if (ImGui::Button(tools[i]))
            m_Tool = (ToolMode)i;
        if (active)
            ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    ImGui::Separator();
    ImGui::SameLine();

    // snap inputs
    ImGui::Text("Snap:");
    ImGui::SameLine(); ImGui::SetNextItemWidth(50);
    ImGui::InputFloat("Mv##s", &m_TranslateSnap, 0, 0, "%.1f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(50);
    ImGui::InputFloat("Rt##s", &m_RotateSnap, 0, 0, "%.1f");
    ImGui::SameLine(); ImGui::SetNextItemWidth(50);
    ImGui::InputFloat("Sc##s", &m_ScaleSnap, 0, 0, "%.2f");
    ImGui::SameLine();

    ImGui::Separator();
    ImGui::SameLine();

    // play / stop
    if (!scene->IsPlaying()) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.55f, 0.25f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.65f, 0.30f, 1));
        if (ImGui::Button("  Play  ")) scene->SetPlaying(true);
        ImGui::PopStyleColor(2);
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.18f, 0.18f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.22f, 0.22f, 1));
        if (ImGui::Button("  Stop  ")) scene->SetPlaying(false);
        ImGui::PopStyleColor(2);
    }

    ImGui::EndMainMenuBar();
}

// hierarchy

void UILayer::DrawNode(Scene* scene, SceneNode* node) {
    bool isLeaf = node->Children.empty();
    bool selected = m_Selected.count(node) > 0;

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanFullWidth |
        ImGuiTreeNodeFlags_DefaultOpen;

    if (isLeaf)   flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (selected) flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushID(node);
    bool nodeOpen = ImGui::TreeNodeEx("##node", flags, "%s", node->Name.c_str());
    bool rowHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

    // selection on click

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        if (ImGui::GetIO().KeyCtrl) {
            if (selected) m_Selected.erase(node);
            else          m_Selected.insert(node);
            m_Primary = node;
        }
        else {
            m_Selected.clear();
            m_Selected.insert(node);
            m_Primary = node;
        }
    }

    // open script editor

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        if (node->Type == NodeType::Script) {
            m_EditingScript = node;
            strncpy_s(m_ScriptBuf, node->ScriptSource.c_str(), sizeof(m_ScriptBuf));
        }
    }

    if (ImGui::BeginPopupContextItem("##ctx")) {
        if (ImGui::MenuItem("Duplicate"))
            scene->DuplicateNode(node);

        if (ImGui::MenuItem("Delete")) {
            scene->RemoveNode(node);
            m_Selected.erase(node);
            if (m_Primary == node) m_Primary = nullptr;
        }

        if ((node->Type == NodeType::Model || node->Type == NodeType::Folder) &&
            ImGui::MenuItem("Ungroup"))
            scene->Ungroup(node);

        // insert option

        if (ImGui::MenuItem("Insert")) {
            m_InsertTarget = node;
            m_OpenInsert = true;
        }

        if (m_Selected.size() > 1) {
            ImGui::Separator();
            if (ImGui::MenuItem("Group into Model"))  scene->GroupIntoModel(m_Selected);
            if (ImGui::MenuItem("Group into Folder")) scene->GroupIntoFolder(m_Selected);
        }
        ImGui::EndPopup();
    }

    if (m_OpenInsert && m_InsertTarget == node) {
        ImGui::OpenPopup("##insert");
        m_OpenInsert = false;
    }

    ImGui::SetNextWindowSize(ImVec2(180, 220), ImGuiCond_Always);

    if (ImGui::BeginPopup("##insert")) {
        ImGui::TextDisabled("Insert object");
        ImGui::Separator();

        ImGui::BeginChild("##insertScroll", ImVec2(0, 0), false,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);

        auto insert = [&](PartShape shape) {
            SceneNode* added = scene->AddPart(m_InsertTarget, shape);
            m_Selected.clear();
            m_Selected.insert(added);
            m_Primary = added;
            ImGui::CloseCurrentPopup();
            };

        if (ImGui::Selectable("Part"))     insert(PartShape::Box);
        if (ImGui::Selectable("Wedge"))    insert(PartShape::Wedge);
        if (ImGui::Selectable("Sphere"))   insert(PartShape::Sphere);
        if (ImGui::Selectable("Cylinder")) insert(PartShape::Cylinder);

        ImGui::Separator();

        if (ImGui::Selectable("Model")) {
            SceneNode* added = scene->AddModel(m_InsertTarget);
            m_Selected.clear(); m_Selected.insert(added); m_Primary = added;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Selectable("Folder")) {
            SceneNode* added = scene->AddFolder(m_InsertTarget);
            m_Selected.clear(); m_Selected.insert(added); m_Primary = added;
            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();

        if (ImGui::Selectable("Script")) {
            SceneNode* added = scene->AddScript(m_InsertTarget);
            m_Selected.clear(); m_Selected.insert(added); m_Primary = added;
            m_EditingScript = added;
            strncpy_s(m_ScriptBuf, added->ScriptSource.c_str(), sizeof(m_ScriptBuf));
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }

    // recurse children

    if (nodeOpen && !isLeaf) {
        auto copy = node->Children;
        for (auto& child : copy)
            DrawNode(scene, child.get());
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void UILayer::DrawHierarchy(Scene* scene) {
    ImGui::Begin("Explorer");

    static char searchBuf[64] = "";
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##Search", "Search...", searchBuf, sizeof(searchBuf));
    ImGui::Separator();

    // services

    DrawNode(scene, scene->GetWorkspace());
    DrawNode(scene, scene->GetLighting());
    DrawNode(scene, scene->GetScriptService());

    ImGui::End();
}

// viewport

void UILayer::DrawViewport(Scene* scene, Renderer* renderer,
    Camera* camera, GLFWwindow* window, float dt) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    ImGui::PopStyleVar();

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winMin = ImGui::GetWindowContentRegionMin();
    ImVec2 origin = ImVec2(winPos.x + winMin.x, winPos.y + winMin.y);

    int w = (int)size.x, h = (int)size.y;
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    m_ViewportHovered = ImGui::IsWindowHovered();
    bool rightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    bool leftMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    camera->SetViewportSize((float)w, (float)h);
    camera->OnUpdate(window, dt, m_ViewportHovered, rightMouse);

    // render scene

    renderer->BeginScene(w, h, camera->GetView(), camera->GetProjection());
    auto partNodes = scene->GetAllPartNodes();
    for (auto* node : partNodes)
        renderer->DrawPart(*node->PartData, m_Selected.count(node) > 0);
    renderer->EndScene();

    ImTextureID texID = (ImTextureID)(intptr_t)renderer->GetFramebuffer()->GetTextureID();
    ImGui::Image(texID, size, ImVec2(0, 1), ImVec2(1, 0));

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // box selection

    if (m_ViewportHovered && m_Tool == ToolMode::Select && !ImGuizmo::IsOver()) {
        ImVec2 mouse = ImGui::GetMousePos();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_BoxSelecting = true;
            m_BoxStart = m_BoxEnd = mouse;
            if (!ImGui::GetIO().KeyCtrl) {
                m_Selected.clear();
                m_Primary = nullptr;
            }
        }

        if (m_BoxSelecting && leftMouse) {
            m_BoxEnd = mouse;
            float x0 = std::min(m_BoxStart.x, m_BoxEnd.x);
            float y0 = std::min(m_BoxStart.y, m_BoxEnd.y);
            float x1 = std::max(m_BoxStart.x, m_BoxEnd.x);
            float y1 = std::max(m_BoxStart.y, m_BoxEnd.y);
            dl->AddRectFilled({ x0,y0 }, { x1,y1 }, IM_COL32(70, 140, 255, 40));
            dl->AddRect({ x0,y0 }, { x1,y1 }, IM_COL32(70, 140, 255, 200), 0, 0, 1.5f);
        }

        if (m_BoxSelecting && !leftMouse) {
            float bx0 = std::min(m_BoxStart.x, m_BoxEnd.x) - origin.x;
            float by0 = std::min(m_BoxStart.y, m_BoxEnd.y) - origin.y;
            float bx1 = std::max(m_BoxStart.x, m_BoxEnd.x) - origin.x;
            float by1 = std::max(m_BoxStart.y, m_BoxEnd.y) - origin.y;

            bool dragged = (bx1 - bx0) > 4.0f || (by1 - by0) > 4.0f;
            if (dragged) {
                glm::mat4 vp = camera->GetProjection() * camera->GetView();
                for (auto* node : partNodes) {
                    glm::vec4 clip = vp * glm::vec4(node->PartData->Position, 1.0f);
                    if (clip.w <= 0.0f) continue;
                    float sx = (clip.x / clip.w * 0.5f + 0.5f) * (float)w;
                    float sy = (-clip.y / clip.w * 0.5f + 0.5f) * (float)h;
                    if (sx >= bx0 && sx <= bx1 && sy >= by0 && sy <= by1) {
                        m_Selected.insert(node);
                        m_Primary = node;
                    }
                }
            }
            m_BoxSelecting = false;
        }
    }

    // guizmo
    if (m_Primary && m_Primary->Type == NodeType::Part &&
        m_Tool != ToolMode::Select) {

        Part& part = *m_Primary->PartData;

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist(dl);
        ImGuizmo::SetRect(origin.x, origin.y, size.x, size.y);

        glm::mat4 view = camera->GetView();
        glm::mat4 proj = camera->GetProjection();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), part.Position);
        transform = glm::rotate(transform, glm::radians(part.Rotation.y), { 0,1,0 });
        transform = glm::rotate(transform, glm::radians(part.Rotation.x), { 1,0,0 });
        transform = glm::rotate(transform, glm::radians(part.Rotation.z), { 0,0,1 });
        transform = glm::scale(transform, part.Scale);

        ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
        if (m_Tool == ToolMode::Rotate) op = ImGuizmo::ROTATE;
        if (m_Tool == ToolMode::Scale)  op = ImGuizmo::SCALE;

        float snapT[3] = { m_TranslateSnap, m_TranslateSnap, m_TranslateSnap };
        float snapR[3] = { m_RotateSnap,    m_RotateSnap,    m_RotateSnap };
        float snapS[3] = { m_ScaleSnap,     m_ScaleSnap,     m_ScaleSnap };
        float* snap = nullptr;
        if (m_Tool == ToolMode::Move && m_TranslateSnap > 0.0f) snap = snapT;
        if (m_Tool == ToolMode::Rotate && m_RotateSnap > 0.0f) snap = snapR;
        if (m_Tool == ToolMode::Scale && m_ScaleSnap > 0.0f) snap = snapS;

        glm::vec3 prevPos = part.Position,
            prevRot = part.Rotation,
            prevScl = part.Scale;

        ImGuizmo::Manipulate(
            glm::value_ptr(view), glm::value_ptr(proj),
            op, ImGuizmo::LOCAL,
            glm::value_ptr(transform),
            nullptr, snap);

        if (ImGuizmo::IsUsing()) {
            float tr[3], ro[3], sc[3];
            ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(transform), tr, ro, sc);
            part.Position = { tr[0], tr[1], tr[2] };
            part.Rotation = { ro[0], ro[1], ro[2] };
            part.Scale = { sc[0], sc[1], sc[2] };
        }

        static bool s_WasUsing = false;
        if (s_WasUsing && !ImGuizmo::IsUsing()) {
            glm::vec3 np = part.Position, nr = part.Rotation, ns = part.Scale;
            SceneNode* t = m_Primary;
            scene->GetUndo().Push({ "Transform",
                [t, prevPos, prevRot, prevScl]() {
                    t->PartData->Position = prevPos;
                    t->PartData->Rotation = prevRot;
                    t->PartData->Scale = prevScl;
                },
                [t, np, nr, ns]() {
                    t->PartData->Position = np;
                    t->PartData->Rotation = nr;
                    t->PartData->Scale = ns;
                }
                });
        }
        s_WasUsing = ImGuizmo::IsUsing();
    }

    // keyboard shortcuts
    if (m_ViewportHovered) {
        ImGuiIO& io = ImGui::GetIO();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) scene->GetUndo().DoUndo();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) scene->GetUndo().DoRedo();
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_G)) scene->GroupIntoModel(m_Selected);
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !m_Selected.empty()) {
            for (auto* n : std::vector<SceneNode*>(m_Selected.begin(), m_Selected.end()))
                scene->RemoveNode(n);
            m_Selected.clear();
            m_Primary = nullptr;
        }
    }

    ImGui::End();
}

// properties

void UILayer::DrawProperties(Scene* scene) {
    ImGui::Begin("Properties");

    if (!m_Primary) {
        ImGui::TextDisabled("Nothing selected");
        ImGui::End();
        return;
    }

    // model / folder

    if (m_Primary->Type == NodeType::Model ||
        m_Primary->Type == NodeType::Folder) {
        ImGui::TextDisabled(m_Primary->Type == NodeType::Model ? "Model" : "Folder");
        ImGui::Separator();
        ImGui::Text("Name"); ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
        char buf[128];
        strncpy_s(buf, m_Primary->Name.c_str(), sizeof(buf));
        if (ImGui::InputText("##GName", buf, sizeof(buf)))
            m_Primary->Name = buf;
        ImGui::Text("Children: %d", (int)m_Primary->Children.size());
        ImGui::End();
        return;
    }

    // part

    if (m_Primary->Type != NodeType::Part || !m_Primary->PartData) {
        ImGui::TextDisabled("Nothing selected");
        ImGui::End();
        return;
    }

    Part& part = *m_Primary->PartData;

    // name

    ImGui::Text("Name");  ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    char nameBuf[128];
    strncpy_s(nameBuf, m_Primary->Name.c_str(), sizeof(nameBuf));
    if (ImGui::InputText("##Name", nameBuf, sizeof(nameBuf)))
        m_Primary->Name = nameBuf;

    // shape

    const char* shapes[] = { "Box","Wedge","Sphere","Cylinder" };
    ImGui::Text("Shape"); ImGui::SameLine(100);
    ImGui::TextDisabled("%s", shapes[(int)part.Shape]);

    // transform

    ImGui::Separator();
    ImGui::TextDisabled("Transform");
    ImGui::Separator();
    ImGui::Text("Position"); ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat3("##Pos", &part.Position.x, 0.1f);
    ImGui::Text("Rotation"); ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat3("##Rot", &part.Rotation.x, 0.5f);
    ImGui::Text("Scale");    ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat3("##Scl", &part.Scale.x, 0.05f, 0.05f, 100.0f);

    // appearance

    ImGui::Separator();
    ImGui::TextDisabled("Appearance");
    ImGui::Separator();
    ImGui::Text("Color");  ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    ImGui::ColorEdit4("##Color", &part.Color.x);

    // physics

    ImGui::Separator();
    ImGui::TextDisabled("Physics");
    ImGui::Separator();
    ImGui::Text("Anchored");   ImGui::SameLine(100); ImGui::Checkbox("##Anch", &part.Anchored);
    ImGui::Text("CanCollide"); ImGui::SameLine(100); ImGui::Checkbox("##CanCollide", &part.CanCollide);
    ImGui::Text("Mass");       ImGui::SameLine(100); ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Mass", &part.Mass, 0.1f, 0.1f, 100.0f);

    // runtime info

    if (scene->IsPlaying()) {
        ImGui::Separator();
        ImGui::TextDisabled("Runtime");
        ImGui::Separator();
        ImGui::Text("Velocity  X:%.2f Y:%.2f Z:%.2f",
            part.Velocity.x, part.Velocity.y, part.Velocity.z);
        ImGui::Text("OnGround  %s", part.OnGround ? "Yes" : "No");
    }

    ImGui::End();
}

void UILayer::DrawScriptEditor(Scene* scene) {
    ImGui::Begin("Script Editor");

    if (!m_EditingScript || m_EditingScript->Type != NodeType::Script) {
        ImGui::TextDisabled("Select a Script in the Explorer to edit it.");
        ImGui::End();
        return;
    }

    ImGui::Text("%s", m_EditingScript->Name.c_str());
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.55f, 0.25f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.65f, 0.30f, 1));
    bool canRun = scene->IsPlaying();
    if (!canRun) ImGui::BeginDisabled();
    if (ImGui::SmallButton("Run")) {
        scene->GetLua().RunScript(
            m_EditingScript->ScriptSource,
            m_EditingScript->Name);
    }
    if (!canRun) ImGui::EndDisabled();
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    ImGui::TextDisabled("(Run only available during Play)");
    ImGui::Separator();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float  lineH = ImGui::GetTextLineHeight();

    int lineCount = 1;
    for (char c : m_EditingScript->ScriptSource)
        if (c == '\n') lineCount++;
    float gutterW = ImGui::CalcTextSize("9999").x + 8.0f;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.10f, 1));
    ImGui::BeginChild("##gutter", ImVec2(gutterW, avail.y - 4), false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float scrollY = 0.0f; 

    for (int i = 1; i <= lineCount; i++) {
        ImGui::TextDisabled("%d", i);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 0);

    // editor area

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.12f, 1));

    struct EditorState {
        bool wantIndent = false;
        bool wantNewline = false;
        int  indentLevel = 0;
    };
    static EditorState edState;

    auto callback = [](ImGuiInputTextCallbackData* data) -> int {
        EditorState* st = static_cast<EditorState*>(data->UserData);

        if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
            return 0;
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways) {
            
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {

            // insert 4 spaces

            data->InsertChars(data->CursorPos, "    ");
            return 0;
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways) {
            if (data->EventKey == ImGuiKey_Enter) {
                int lineStart = data->CursorPos - 1;
                while (lineStart > 0 && data->Buf[lineStart - 1] != '\n')
                    lineStart--;

                // count leading spaces/tabs

                int spaces = 0;
                for (int i = lineStart; i < data->CursorPos; i++) {
                    if (data->Buf[i] == ' ')       spaces++;
                    else if (data->Buf[i] == '\t')  spaces += 4;
                    else break;
                }

                // check if line ends with 'then', 'do', 'function', 'else', '{'

                std::string line(data->Buf + lineStart, data->CursorPos - lineStart);
                bool shouldIndent = false;
                auto endsWith = [&](const char* s) {
                    std::string trimmed = line;
                    while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t'))
                        trimmed.pop_back();
                    size_t len = strlen(s);
                    return trimmed.size() >= len &&
                        trimmed.substr(trimmed.size() - len) == s;
                    };
                if (endsWith("then") || endsWith("do") ||
                    endsWith("else") || endsWith("elseif") ||
                    endsWith("{") || endsWith("function") ||
                    endsWith("repeat"))
                    shouldIndent = true;

                // insert newline + indent

                std::string indent = "\n" + std::string(spaces + (shouldIndent ? 4 : 0), ' ');
                data->InsertChars(data->CursorPos, indent.c_str());
                return 0;
            }
        }
        return 0;
        };

    if (ImGui::InputTextMultiline(
        "##src",
        m_ScriptBuf, sizeof(m_ScriptBuf),
        ImVec2(avail.x - gutterW, avail.y - 4),
        ImGuiInputTextFlags_AllowTabInput |
        ImGuiInputTextFlags_CallbackAlways,
        callback, &edState)) {
        m_EditingScript->ScriptSource = m_ScriptBuf;
    }

    ImGui::PopStyleColor();
    ImGui::End();
}

void UILayer::DrawConsole(Scene* scene) {
    ImGui::Begin("Output");

    if (ImGui::SmallButton("Clear"))
        scene->GetLua().ClearOutput();

    ImGui::SameLine();
    ImGui::TextDisabled("%d lines", (int)scene->GetLua().GetOutput().size());
    ImGui::Separator();

    ImGui::BeginChild("##consolescroll", ImVec2(0, 0), false,
        ImGuiWindowFlags_HorizontalScrollbar);

    for (auto& entry : scene->GetLua().GetOutput()) {
        ImVec4 col;
        switch (entry.level) {
        case ScriptOutput::Level::Print: col = ImVec4(0.90f, 0.90f, 0.90f, 1); break;
        case ScriptOutput::Level::Warn:  col = ImVec4(1.00f, 0.85f, 0.20f, 1); break;
        case ScriptOutput::Level::Error: col = ImVec4(1.00f, 0.35f, 0.35f, 1); break;
        }
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(entry.message.c_str());
        ImGui::PopStyleColor();
    }

    // auto scroll to bottom

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}