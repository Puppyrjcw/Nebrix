// headers

#include "SceneNode.h"

static int s_Counter = 0;

std::shared_ptr<SceneNode> SceneNode::MakePart(PartShape shape) {
    auto n = std::make_shared<SceneNode>();
    n->Type = NodeType::Part;
    n->PartData = std::make_shared<Part>();
    n->PartData->Shape = shape;
    switch (shape) {
    case PartShape::Box:      n->Name = "Part";     break;
    case PartShape::Wedge:    n->Name = "Wedge";    break;
    case PartShape::Sphere:   n->Name = "Sphere";   break;
    case PartShape::Cylinder: n->Name = "Cylinder"; break;
    }
    s_Counter++;
    n->PartData->Color = {
        0.2f + (s_Counter % 5) * 0.15f,
        0.3f + (s_Counter % 3) * 0.20f,
        0.8f - (s_Counter % 4) * 0.10f,
        1.0f
    };
    return n;
}

std::shared_ptr<SceneNode> SceneNode::MakeModel() {
    auto n = std::make_shared<SceneNode>();
    n->Type = NodeType::Model;
    n->Name = "Model";
    return n;
}

std::shared_ptr<SceneNode> SceneNode::MakeFolder() {
    auto n = std::make_shared<SceneNode>();
    n->Type = NodeType::Folder;
    n->Name = "Folder";
    return n;
}

std::shared_ptr<SceneNode> SceneNode::MakeScript() {
    auto n = std::make_shared<SceneNode>();
    n->Type = NodeType::Script;
    n->Name = "Script";
    n->ScriptSource =
        "-- Nebrix Script\n"
        "-- Use script.Parent to get the parent instance\n"
        "-- Example: local part = script.Parent\n"
        "\n"
        "print(\"Hello from Nebrix!\")\n";
    return n;
}

std::shared_ptr<SceneNode> SceneNode::MakeService(const std::string& name, NodeType type) {
    auto n = std::make_shared<SceneNode>();
    n->Type = type;
    n->Name = name;
    if (type == NodeType::Lighting)
        n->Lighting = std::make_shared<LightingProperties>();
    return n;
}

void SceneNode::CollectParts(std::vector<SceneNode*>& out) {
    if (Type == NodeType::Part) { out.push_back(this); return; }
    for (auto& c : Children) c->CollectParts(out);
}

void SceneNode::CollectPartsConst(std::vector<const SceneNode*>& out) const {
    if (Type == NodeType::Part) { out.push_back(this); return; }
    for (auto& c : Children) c->CollectPartsConst(out);
}

void SceneNode::CollectScripts(std::vector<SceneNode*>& out) {
    if (Type == NodeType::Script) { out.push_back(this); return; }
    for (auto& c : Children) c->CollectScripts(out);
}