#pragma once

// utils

#include <string>
#include <vector>

struct lua_State;
class Scene;
struct SceneNode;

struct ScriptOutput {
    enum class Level { Print, Warn, Error };
    Level       level = Level::Print;
    std::string message;
};

class LuaRuntime {
public:
    LuaRuntime();
    ~LuaRuntime();

    void Init(Scene* scene);
    bool RunScript(const std::string& source,
        const std::string& name = "Script",
        SceneNode* scriptNode = nullptr);
    void RunAllScripts(Scene* scene);
    void Update(Scene* scene, float dt);

    const std::vector<ScriptOutput>& GetOutput() const { return m_Output; }
    void ClearOutput() { m_Output.clear(); }

    std::vector<ScriptOutput> m_Output;

private:
    void PushPrint();
    void BindAPI(Scene* scene);

    lua_State* m_L = nullptr;
};