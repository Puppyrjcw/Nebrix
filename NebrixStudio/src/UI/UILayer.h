#pragma once
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <set>

struct GLFWwindow;
class Scene;
class Renderer;
class Camera;
struct SceneNode;
enum class NodeType;

enum class ToolMode { Select = 0, Move, Scale, Rotate };

class UILayer {
public:
    void Init(GLFWwindow* window);
    void Shutdown();
    void Begin();
    void End();
    void DrawPanels(Scene* scene, Renderer* renderer,
        Camera* camera, GLFWwindow* window, float dt);

private:
    void DrawMenuBar(Scene* scene);
    void DrawViewport(Scene* scene, Renderer* renderer,
        Camera* camera, GLFWwindow* window, float dt);
    void DrawHierarchy(Scene* scene);
    void DrawNode(Scene* scene, SceneNode* node);
    void DrawProperties(Scene* scene);

    void DrawScriptEditor(Scene* scene);
    void DrawConsole(Scene* scene);

    // script editor

    SceneNode* m_EditingScript = nullptr;
    char       m_ScriptBuf[8192] = {};
    bool       m_ScriptDirty = false;

    // selection

    std::set<SceneNode*> m_Selected;
    SceneNode* m_Primary = nullptr;

    // tool

    ToolMode             m_Tool = ToolMode::Select;

    // viewport state

    bool                 m_ViewportHovered = false;
    bool                 m_BoxSelecting = false;
    ImVec2               m_BoxStart = { 0, 0 };
    ImVec2               m_BoxEnd = { 0, 0 };

    // snap

    float                m_TranslateSnap = 0.0f;
    float                m_RotateSnap = 0.0f;
    float                m_ScaleSnap = 0.0f;

    // insert popup

    SceneNode* m_InsertTarget = nullptr;
    bool                 m_OpenInsert = false;
};