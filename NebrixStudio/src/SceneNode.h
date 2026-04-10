#pragma once

// headers

#include "Part.h"

// utils

#include <string>
#include <vector>
#include <memory>

enum class NodeType {
    Workspace,
    Part,
    Model,
    Folder,
    Script,
    ScriptService,
    Lighting
};

struct LightingProperties {
    float       TimeOfDay = 14.0f;
    float       Brightness = 1.0f;
    float       FogStart = 100.0f;
    float       FogEnd = 300.0f;
    glm::vec3   FogColor = { 0.75f, 0.75f, 0.75f };
    glm::vec3   Ambient = { 0.5f,  0.5f,  0.5f };
    glm::vec3   OutdoorAmbient = { 0.5f, 0.5f, 0.5f };
    glm::vec3   SkyColor = { 0.53f, 0.81f, 0.98f };
};

struct SceneNode {
    std::string                             Name;
    NodeType                                Type = NodeType::Part;
    std::shared_ptr<Part>                   PartData;
    std::string                             ScriptSource = "-- Script\n";
    std::shared_ptr<LightingProperties>     Lighting;
    std::vector<std::shared_ptr<SceneNode>> Children;
    SceneNode* Parent = nullptr;
    bool                                    Open = true;
    glm::vec3                               ModelOffset = { 0,0,0 };

    static std::shared_ptr<SceneNode> MakePart(PartShape shape = PartShape::Box);
    static std::shared_ptr<SceneNode> MakeModel();
    static std::shared_ptr<SceneNode> MakeFolder();
    static std::shared_ptr<SceneNode> MakeScript();
    static std::shared_ptr<SceneNode> MakeService(const std::string& name, NodeType type);

    void CollectParts(std::vector<SceneNode*>& out);
    void CollectScripts(std::vector<SceneNode*>& out);
    void CollectPartsConst(std::vector<const SceneNode*>& out) const;
};