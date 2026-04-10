#pragma once

// headers

#include "SceneNode.h"
#include "PhysicsWorld.h"
#include "UndoRedo.h"
#include "LuaRuntime.h"

// utils

#include <vector>
#include <memory>
#include <set>

class Scene {
public:
    Scene();

    void Update(float dt);

    SceneNode* AddPart(SceneNode* parent, PartShape shape = PartShape::Box);
    SceneNode* AddModel(SceneNode* parent);
    SceneNode* AddFolder(SceneNode* parent);
    SceneNode* AddScript(SceneNode* parent);
    void       RemoveNode(SceneNode* node);
    void       DuplicateNode(SceneNode* node);

    void GroupIntoModel(const std::set<SceneNode*>& nodes);
    void GroupIntoFolder(const std::set<SceneNode*>& nodes);
    void Ungroup(SceneNode* node);

    void Reparent(SceneNode* node, SceneNode* newParent, int insertIndex = -1);

    SceneNode* GetWorkspace() { return m_Workspace.get(); }
    SceneNode* GetLighting() { return m_Lighting.get(); }
    SceneNode* GetScriptService() { return m_ScriptService.get(); }
    UndoStack& GetUndo() { return m_Undo; }
    LuaRuntime& GetLua() { return m_Lua; }
    bool        IsPlaying() const { return m_Playing; }
    void        SetPlaying(bool playing);

    std::vector<SceneNode*> GetAllPartNodes();

    SceneNode* FindNode(SceneNode* target, SceneNode* searchRoot = nullptr);

private:
    void RemoveNodeFromParent(SceneNode* node);
    std::shared_ptr<SceneNode> DeepCloneNode(SceneNode* node, SceneNode* newParent);
    std::vector<std::shared_ptr<SceneNode>> DeepCloneChildren(SceneNode* node);

    std::shared_ptr<SceneNode> m_Workspace;
    std::shared_ptr<SceneNode> m_Lighting;
    std::shared_ptr<SceneNode> m_ScriptService;

    PhysicsWorld  m_Physics;
    UndoStack     m_Undo;
    LuaRuntime    m_Lua;
    bool          m_Playing = false;
    int           m_PartCounter = 0;
    std::vector<std::shared_ptr<SceneNode>> m_Snapshot;
    std::vector<std::shared_ptr<SceneNode>> m_LightingSnapshot;
};