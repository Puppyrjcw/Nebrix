// headers

#include "Scene.h"

// utils

#include <algorithm>

Scene::Scene() {

    // Workspace

    m_Workspace = std::make_shared<SceneNode>();
    m_Workspace->Name = "Workspace";
    m_Workspace->Type = NodeType::Workspace;

    // Default baseplate

    auto bp = SceneNode::MakePart(PartShape::Box);
    bp->Name = "Baseplate";
    bp->Parent = m_Workspace.get();
    bp->PartData->Position = { 0,0,0 };
    bp->PartData->Scale = { 50,0.1f,50 };
    bp->PartData->Color = { 0.35f,0.60f,0.35f,1 };
    bp->PartData->Anchored = true;
    m_Workspace->Children.push_back(bp);

    // Lighting service

    m_Lighting = SceneNode::MakeService("Lighting", NodeType::Lighting);
    m_Lighting->Parent = nullptr;

    // ScriptService

    m_ScriptService = SceneNode::MakeService("ScriptService", NodeType::ScriptService);
    m_ScriptService->Parent = nullptr;
}

void Scene::Update(float dt) {
    if (!m_Playing) return;
    m_Lua.Update(this, dt);
    auto partNodes = GetAllPartNodes();
    for (auto* node : partNodes) {
        if (!node->PartData->Anchored) {
            std::vector<Part> single = { *node->PartData };
            m_Physics.Step(single, dt);
            *node->PartData = single[0];
        }
    }
}

std::vector<SceneNode*> Scene::GetAllPartNodes() {
    std::vector<SceneNode*> result;
    m_Workspace->CollectParts(result);
    return result;
}

void Scene::RemoveNodeFromParent(SceneNode* node) {
    if (!node->Parent) return;
    auto& siblings = node->Parent->Children;
    siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
        [node](const std::shared_ptr<SceneNode>& c) { return c.get() == node; }),
        siblings.end());
}

std::shared_ptr<SceneNode> Scene::DeepCloneNode(SceneNode* node, SceneNode* newParent) {
    auto copy = std::make_shared<SceneNode>();
    copy->Name = node->Name;
    copy->Type = node->Type;
    copy->Open = node->Open;
    copy->ScriptSource = node->ScriptSource;
    copy->ModelOffset = node->ModelOffset;
    copy->Parent = newParent;
    if (node->PartData)
        copy->PartData = std::make_shared<Part>(*node->PartData);
    if (node->Lighting)
        copy->Lighting = std::make_shared<LightingProperties>(*node->Lighting);
    for (auto& child : node->Children)
        copy->Children.push_back(DeepCloneNode(child.get(), copy.get()));
    return copy;
}

std::vector<std::shared_ptr<SceneNode>> Scene::DeepCloneChildren(SceneNode* node) {
    std::vector<std::shared_ptr<SceneNode>> result;
    for (auto& child : node->Children)
        result.push_back(DeepCloneNode(child.get(), node));
    return result;
}

SceneNode* Scene::AddPart(SceneNode* parent, PartShape shape) {
    if (!parent) parent = m_Workspace.get();
    auto node = SceneNode::MakePart(shape);
    node->Parent = parent;
    node->PartData->Position = { 0,6,0 };
    node->PartData->Color = {
        0.2f + (m_PartCounter % 5) * 0.15f,
        0.3f + (m_PartCounter % 3) * 0.20f,
        0.8f - (m_PartCounter % 4) * 0.10f,
        1.0f
    };
    m_PartCounter++;
    parent->Children.push_back(node);
    SceneNode* raw = node.get();
    m_Undo.Push({ "Add Part",
        [this, raw]() { RemoveNodeFromParent(raw); },
        [this, parent, node]() { node->Parent = parent; parent->Children.push_back(node); }
        });
    return raw;
}

SceneNode* Scene::AddModel(SceneNode* parent) {
    if (!parent) parent = m_Workspace.get();
    auto node = SceneNode::MakeModel();
    node->Parent = parent;
    parent->Children.push_back(node);
    SceneNode* raw = node.get();
    m_Undo.Push({ "Add Model",
        [this, raw]() { RemoveNodeFromParent(raw); },
        [this, parent, node]() { node->Parent = parent; parent->Children.push_back(node); }
        });
    return raw;
}

SceneNode* Scene::AddFolder(SceneNode* parent) {
    if (!parent) parent = m_Workspace.get();
    auto node = SceneNode::MakeFolder();
    node->Parent = parent;
    parent->Children.push_back(node);
    SceneNode* raw = node.get();
    m_Undo.Push({ "Add Folder",
        [this, raw]() { RemoveNodeFromParent(raw); },
        [this, parent, node]() { node->Parent = parent; parent->Children.push_back(node); }
        });
    return raw;
}

SceneNode* Scene::AddScript(SceneNode* parent) {
    if (!parent) parent = m_Workspace.get();
    auto node = SceneNode::MakeScript();
    node->Parent = parent;
    parent->Children.push_back(node);
    SceneNode* raw = node.get();
    m_Undo.Push({ "Add Script",
        [this, raw]() { RemoveNodeFromParent(raw); },
        [this, parent, node]() { node->Parent = parent; parent->Children.push_back(node); }
        });
    return raw;
}

void Scene::RemoveNode(SceneNode* node) {
    if (!node || node == m_Workspace.get()) return;
    if (!node->Parent) return;
    std::shared_ptr<SceneNode> kept;
    for (auto& c : node->Parent->Children)
        if (c.get() == node) { kept = c; break; }
    SceneNode* parent = node->Parent;
    RemoveNodeFromParent(node);
    m_Undo.Push({ "Delete",
        [this, parent, kept]() { kept->Parent = parent; parent->Children.push_back(kept); },
        [this, node]() { RemoveNodeFromParent(node); }
        });
}

void Scene::DuplicateNode(SceneNode* node) {
    if (!node || !node->Parent) return;
    auto copy = DeepCloneNode(node, node->Parent);
    node->Parent->Children.push_back(copy);
    SceneNode* raw = copy.get();
    std::shared_ptr<SceneNode> kept = copy;
    m_Undo.Push({ "Duplicate",
        [this, raw]() { RemoveNodeFromParent(raw); },
        [this, node, kept]() { kept->Parent = node->Parent; node->Parent->Children.push_back(kept); }
        });
}

void Scene::Reparent(SceneNode* node, SceneNode* newParent, int insertIndex) {
    if (!node || !newParent || node == newParent) return;
    SceneNode* check = newParent;
    while (check) { if (check == node) return; check = check->Parent; }

    std::shared_ptr<SceneNode> kept;
    if (node->Parent) {
        for (auto& c : node->Parent->Children)
            if (c.get() == node) { kept = c; break; }
        RemoveNodeFromParent(node);
    }
    if (!kept) return;

    node->Parent = newParent;
    if (insertIndex < 0 || insertIndex >= (int)newParent->Children.size()) {
        newParent->Children.push_back(kept);
    }
    else {
        newParent->Children.insert(
            newParent->Children.begin() + insertIndex, kept);
    }
}

void Scene::GroupIntoModel(const std::set<SceneNode*>& nodes) {
    if (nodes.empty()) return;
    SceneNode* parent = (*nodes.begin())->Parent;
    if (!parent) parent = m_Workspace.get();
    auto model = SceneNode::MakeModel();
    model->Parent = parent;
    std::vector<std::shared_ptr<SceneNode>> moved;
    for (auto& child : parent->Children)
        if (nodes.count(child.get())) moved.push_back(child);
    for (auto& n : moved) {
        RemoveNodeFromParent(n.get());
        n->Parent = model.get();
        model->Children.push_back(n);
    }
    parent->Children.push_back(model);
}

void Scene::GroupIntoFolder(const std::set<SceneNode*>& nodes) {
    if (nodes.empty()) return;
    SceneNode* parent = (*nodes.begin())->Parent;
    if (!parent) parent = m_Workspace.get();
    auto folder = SceneNode::MakeFolder();
    folder->Parent = parent;
    std::vector<std::shared_ptr<SceneNode>> moved;
    for (auto& child : parent->Children)
        if (nodes.count(child.get())) moved.push_back(child);
    for (auto& n : moved) {
        RemoveNodeFromParent(n.get());
        n->Parent = folder.get();
        folder->Children.push_back(n);
    }
    parent->Children.push_back(folder);
}

void Scene::Ungroup(SceneNode* node) {
    if (!node || !node->Parent) return;
    if (node->Type != NodeType::Model && node->Type != NodeType::Folder) return;
    SceneNode* parent = node->Parent;
    std::vector<std::shared_ptr<SceneNode>> children = node->Children;
    RemoveNodeFromParent(node);
    for (auto& c : children) { c->Parent = parent; parent->Children.push_back(c); }
}

void Scene::SetPlaying(bool playing) {
    if (playing && !m_Playing) {
        m_Snapshot = DeepCloneChildren(m_Workspace.get());
        if (m_Lighting->Lighting)
            m_LightingSnapshot = { DeepCloneNode(m_Lighting.get(), nullptr) };
        m_Lua.Init(this);
        m_Lua.RunAllScripts(this);
    }
    if (!playing && m_Playing) {
        m_Workspace->Children = m_Snapshot;
        for (auto& c : m_Workspace->Children) c->Parent = m_Workspace.get();
        m_Lua.ClearOutput();
    }
    m_Playing = playing;
}

SceneNode* Scene::FindNode(SceneNode* target, SceneNode* searchRoot) {
    if (!searchRoot) searchRoot = m_Workspace.get();
    if (searchRoot == target) return searchRoot;
    for (auto& c : searchRoot->Children) {
        auto* found = FindNode(target, c.get());
        if (found) return found;
    }
    return nullptr;
}