// headers

#include "LuaRuntime.h"
#include "Scene.h"
#include "SceneNode.h"

// utils

#include <functional>
#include <string>
#include <cmath>

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

LuaRuntime::LuaRuntime() {
    m_L = luaL_newstate();
    luaL_openlibs(m_L);
}

LuaRuntime::~LuaRuntime() {
    if (m_L) lua_close(m_L);
}

static const char* k_SceneKey = "NebrixScene";
static const char* k_RuntimeKey = "NebrixRuntime";

// helpers

static float TableGetFloat(lua_State* L, int idx,
    const char* upper, const char* lower) {
    lua_getfield(L, idx, upper);
    bool has = !lua_isnil(L, -1);
    float f = (float)luaL_optnumber(L, -1, 0.0);
    lua_pop(L, 1);
    if (!has) {
        lua_getfield(L, idx, lower);
        f = (float)luaL_optnumber(L, -1, 0.0);
        lua_pop(L, 1);
    }
    return f;
}

static void PushVec3(lua_State* L, float x, float y, float z) {
    lua_newtable(L);
    lua_pushnumber(L, x); lua_setfield(L, -2, "X");
    lua_pushnumber(L, y); lua_setfield(L, -2, "Y");
    lua_pushnumber(L, z); lua_setfield(L, -2, "Z");
    lua_pushnumber(L, x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, y); lua_setfield(L, -2, "y");
    lua_pushnumber(L, z); lua_setfield(L, -2, "z");
}

static void PushVec2(lua_State* L, float x, float y) {
    lua_newtable(L);
    lua_pushnumber(L, x); lua_setfield(L, -2, "X");
    lua_pushnumber(L, y); lua_setfield(L, -2, "Y");
    lua_pushnumber(L, x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, y); lua_setfield(L, -2, "y");
}

static void PushColor3(lua_State* L, float r, float g, float b) {
    lua_newtable(L);
    lua_pushnumber(L, r); lua_setfield(L, -2, "R");
    lua_pushnumber(L, g); lua_setfield(L, -2, "G");
    lua_pushnumber(L, b); lua_setfield(L, -2, "B");
    lua_pushnumber(L, r); lua_setfield(L, -2, "r");
    lua_pushnumber(L, g); lua_setfield(L, -2, "g");
    lua_pushnumber(L, b); lua_setfield(L, -2, "b");
}

static glm::vec3 ReadVec3(lua_State* L, int idx) {
    return {
        TableGetFloat(L, idx, "X", "x"),
        TableGetFloat(L, idx, "Y", "y"),
        TableGetFloat(L, idx, "Z", "z")
    };
}

static void PushNodeWithMeta(lua_State* L, SceneNode* node) {
    lua_pushlightuserdata(L, node);
    luaL_getmetatable(L, "PartMeta");
    lua_setmetatable(L, -2);
}

// c functions

static int L_Vector3New(lua_State* L) {
    PushVec3(L,
        (float)luaL_optnumber(L, 1, 0),
        (float)luaL_optnumber(L, 2, 0),
        (float)luaL_optnumber(L, 3, 0));
    return 1;
}

static int L_Vector2New(lua_State* L) {
    PushVec2(L,
        (float)luaL_optnumber(L, 1, 0),
        (float)luaL_optnumber(L, 2, 0));
    return 1;
}

static int L_Color3New(lua_State* L) {
    PushColor3(L,
        (float)luaL_optnumber(L, 1, 1),
        (float)luaL_optnumber(L, 2, 1),
        (float)luaL_optnumber(L, 3, 1));
    return 1;
}

static int L_Color3FromRGB(lua_State* L) {
    PushColor3(L,
        (float)luaL_checknumber(L, 1) / 255.f,
        (float)luaL_checknumber(L, 2) / 255.f,
        (float)luaL_checknumber(L, 3) / 255.f);
    return 1;
}

static int L_Color3FromHSV(lua_State* L) {
    float h = (float)luaL_checknumber(L, 1);
    float s = (float)luaL_checknumber(L, 2);
    float v = (float)luaL_checknumber(L, 3);
    float r, g, b;
    if (s == 0.f) { r = g = b = v; }
    else {
        int i = (int)(h * 6); float f = h * 6.f - i;
        float p = v * (1 - s), q = v * (1 - f * s), t2 = v * (1 - (1 - f) * s);
        switch (i % 6) {
        case 0:r = v; g = t2; b = p; break; case 1:r = q; g = v; b = p; break;
        case 2:r = p; g = v; b = t2; break; case 3:r = p; g = q; b = v; break;
        case 4:r = t2; g = p; b = v; break; default:r = v; g = p; b = q; break;
        }
    }
    PushColor3(L, r, g, b);
    return 1;
}

// partmeta __index

static int L_PartIndex(lua_State* L) {
    auto* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
    const char* key = luaL_checkstring(L, 2);
    if (!node) return 0;

    // part properties

    if (node->PartData) {
        if (strcmp(key, "Position") == 0) { PushVec3(L, node->PartData->Position.x, node->PartData->Position.y, node->PartData->Position.z); return 1; }
        if (strcmp(key, "Rotation") == 0) { PushVec3(L, node->PartData->Rotation.x, node->PartData->Rotation.y, node->PartData->Rotation.z); return 1; }
        if (strcmp(key, "Size") == 0) { PushVec3(L, node->PartData->Scale.x, node->PartData->Scale.y, node->PartData->Scale.z);   return 1; }
        if (strcmp(key, "Velocity") == 0) { PushVec3(L, node->PartData->Velocity.x, node->PartData->Velocity.y, node->PartData->Velocity.z); return 1; }
        if (strcmp(key, "Color") == 0) { PushColor3(L, node->PartData->Color.r, node->PartData->Color.g, node->PartData->Color.b);   return 1; }
        if (strcmp(key, "Anchored") == 0) { lua_pushboolean(L, node->PartData->Anchored);  return 1; }
        if (strcmp(key, "CanCollide") == 0) { lua_pushboolean(L, node->PartData->CanCollide); return 1; }
        if (strcmp(key, "Mass") == 0) { lua_pushnumber(L, node->PartData->Mass);        return 1; }
        if (strcmp(key, "Transparency") == 0) { lua_pushnumber(L, 1.0f - node->PartData->Color.a); return 1; }
    }

    // lighting properties

    if (node->Lighting) {
        if (strcmp(key, "TimeOfDay") == 0) { lua_pushnumber(L, node->Lighting->TimeOfDay);  return 1; }
        if (strcmp(key, "Brightness") == 0) { lua_pushnumber(L, node->Lighting->Brightness); return 1; }
        if (strcmp(key, "FogStart") == 0) { lua_pushnumber(L, node->Lighting->FogStart);   return 1; }
        if (strcmp(key, "FogEnd") == 0) { lua_pushnumber(L, node->Lighting->FogEnd);     return 1; }
        if (strcmp(key, "FogColor") == 0) { PushColor3(L, node->Lighting->FogColor.r, node->Lighting->FogColor.g, node->Lighting->FogColor.b);   return 1; }
        if (strcmp(key, "Ambient") == 0) { PushColor3(L, node->Lighting->Ambient.r, node->Lighting->Ambient.g, node->Lighting->Ambient.b);    return 1; }
        if (strcmp(key, "OutdoorAmbient") == 0) { PushColor3(L, node->Lighting->OutdoorAmbient.r, node->Lighting->OutdoorAmbient.g, node->Lighting->OutdoorAmbient.b); return 1; }
    }

    // common

    if (strcmp(key, "Name") == 0) { lua_pushstring(L, node->Name.c_str()); return 1; }

    if (strcmp(key, "Parent") == 0) {
        if (node->Parent) PushNodeWithMeta(L, node->Parent);
        else             lua_pushnil(L);
        return 1;
    }

    for (auto& c : node->Children) {
        if (c->Name == key) {
            PushNodeWithMeta(L, c.get());
            return 1;
        }
    }

    // methods

    if (strcmp(key, "FindFirstChild") == 0) {
        lua_pushlightuserdata(L, node);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            const char* name = luaL_checkstring(L2, 1);
            for (auto& c : n->Children) if (c->Name == name) { PushNodeWithMeta(L2, c.get()); return 1; }
            lua_pushnil(L2); return 1;
            }, 1);
        return 1;
    }

    if (strcmp(key, "FindFirstChildOfClass") == 0) {
        lua_pushlightuserdata(L, node);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            const char* cls = luaL_checkstring(L2, 1);
            for (auto& c : n->Children) {
                bool match =
                    (strcmp(cls, "Part") == 0 && c->Type == NodeType::Part) ||
                    (strcmp(cls, "Model") == 0 && c->Type == NodeType::Model) ||
                    (strcmp(cls, "Folder") == 0 && c->Type == NodeType::Folder) ||
                    (strcmp(cls, "Script") == 0 && c->Type == NodeType::Script);
                if (match) { PushNodeWithMeta(L2, c.get()); return 1; }
            }
            lua_pushnil(L2); return 1;
            }, 1);
        return 1;
    }

    if (strcmp(key, "GetChildren") == 0) {
        lua_pushlightuserdata(L, node);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            lua_newtable(L2);
            for (int i = 0; i < (int)n->Children.size(); i++) {
                PushNodeWithMeta(L2, n->Children[i].get());
                lua_rawseti(L2, -2, i + 1);
            }
            return 1;
            }, 1);
        return 1;
    }

    if (strcmp(key, "GetDescendants") == 0) {
        lua_pushlightuserdata(L, node);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            lua_newtable(L2);
            int idx = 1;
            std::vector<SceneNode*> stack;
            for (auto& c : n->Children) stack.push_back(c.get());
            while (!stack.empty()) {
                SceneNode* cur = stack.back(); stack.pop_back();
                PushNodeWithMeta(L2, cur);
                lua_rawseti(L2, -2, idx++);
                for (auto& c : cur->Children) stack.push_back(c.get());
            }
            return 1;
            }, 1);
        return 1;
    }

    if (strcmp(key, "IsA") == 0) {
        lua_pushlightuserdata(L, node);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            const char* cls = luaL_checkstring(L2, 1);
            bool r = strcmp(cls, "Instance") == 0 ||
                (strcmp(cls, "Part") == 0 && n->Type == NodeType::Part) ||
                (strcmp(cls, "BasePart") == 0 && n->Type == NodeType::Part) ||
                (strcmp(cls, "Model") == 0 && n->Type == NodeType::Model) ||
                (strcmp(cls, "Folder") == 0 && n->Type == NodeType::Folder) ||
                (strcmp(cls, "Script") == 0 && n->Type == NodeType::Script);
            lua_pushboolean(L2, r); return 1;
            }, 1);
        return 1;
    }

    if (strcmp(key, "Destroy") == 0) {
        lua_getfield(L, LUA_REGISTRYINDEX, k_SceneKey);
        auto* sc = static_cast<Scene*>(lua_touserdata(L, -1)); lua_pop(L, 1);
        lua_pushlightuserdata(L, node);
        lua_pushlightuserdata(L, sc);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            auto* s = static_cast<Scene*>    (lua_touserdata(L2, lua_upvalueindex(2)));
            s->RemoveNode(n); return 0;
            }, 2);
        return 1;
    }

    if (strcmp(key, "Clone") == 0) {
        lua_getfield(L, LUA_REGISTRYINDEX, k_SceneKey);
        auto* sc = static_cast<Scene*>(lua_touserdata(L, -1)); lua_pop(L, 1);
        lua_pushlightuserdata(L, node);
        lua_pushlightuserdata(L, sc);
        lua_pushcclosure(L, [](lua_State* L2)->int {
            auto* n = static_cast<SceneNode*>(lua_touserdata(L2, lua_upvalueindex(1)));
            auto* s = static_cast<Scene*>    (lua_touserdata(L2, lua_upvalueindex(2)));
            s->DuplicateNode(n);
            PushNodeWithMeta(L2, n->Parent->Children.back().get());
            return 1;
            }, 2);
        return 1;
    }

    return 0;
}

// partmeta __newindex

static int L_PartNewIndex(lua_State* L) {
    auto* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
    const char* key = luaL_checkstring(L, 2);
    if (!node) return 0;

    if (node->PartData) {
        if (strcmp(key, "Position") == 0) { node->PartData->Position = ReadVec3(L, 3); return 0; }
        if (strcmp(key, "Rotation") == 0) { node->PartData->Rotation = ReadVec3(L, 3); return 0; }
        if (strcmp(key, "Size") == 0) { node->PartData->Scale = ReadVec3(L, 3); return 0; }
        if (strcmp(key, "Velocity") == 0) { node->PartData->Velocity = ReadVec3(L, 3); return 0; }
        if (strcmp(key, "Anchored") == 0) { node->PartData->Anchored = lua_toboolean(L, 3) != 0; return 0; }
        if (strcmp(key, "CanCollide") == 0) { node->PartData->CanCollide = lua_toboolean(L, 3) != 0; return 0; }
        if (strcmp(key, "Mass") == 0) { node->PartData->Mass = (float)luaL_checknumber(L, 3); return 0; }
        if (strcmp(key, "Transparency") == 0) {
            float t = (float)luaL_checknumber(L, 3);
            node->PartData->Color.a = 1.0f - t; return 0;
        }
        if (strcmp(key, "Color") == 0) {
            node->PartData->Color = {
                TableGetFloat(L,3,"R","r"),
                TableGetFloat(L,3,"G","g"),
                TableGetFloat(L,3,"B","b"),
                node->PartData->Color.a
            };
            return 0;
        }
    }

    if (node->Lighting) {
        if (strcmp(key, "TimeOfDay") == 0) { node->Lighting->TimeOfDay = (float)luaL_checknumber(L, 3); return 0; }
        if (strcmp(key, "Brightness") == 0) { node->Lighting->Brightness = (float)luaL_checknumber(L, 3); return 0; }
        if (strcmp(key, "FogStart") == 0) { node->Lighting->FogStart = (float)luaL_checknumber(L, 3); return 0; }
        if (strcmp(key, "FogEnd") == 0) { node->Lighting->FogEnd = (float)luaL_checknumber(L, 3); return 0; }
        if (strcmp(key, "FogColor") == 0) { node->Lighting->FogColor = { TableGetFloat(L,3,"R","r"),TableGetFloat(L,3,"G","g"),TableGetFloat(L,3,"B","b") }; return 0; }
        if (strcmp(key, "Ambient") == 0) { node->Lighting->Ambient = { TableGetFloat(L,3,"R","r"),TableGetFloat(L,3,"G","g"),TableGetFloat(L,3,"B","b") }; return 0; }
        if (strcmp(key, "OutdoorAmbient") == 0) { node->Lighting->OutdoorAmbient = { TableGetFloat(L,3,"R","r"),TableGetFloat(L,3,"G","g"),TableGetFloat(L,3,"B","b") }; return 0; }
    }

    if (strcmp(key, "Name") == 0) { node->Name = luaL_checkstring(L, 3); return 0; }

    if (strcmp(key, "Parent") == 0) {
        if (!lua_isnil(L, 3) && lua_isuserdata(L, 3)) {
            auto* np = static_cast<SceneNode*>(lua_touserdata(L, 3));
            if (node->Parent) {
                auto& sib = node->Parent->Children;
                std::shared_ptr<SceneNode> kept;
                for (auto& c : sib) if (c.get() == node) { kept = c; break; }
                sib.erase(std::remove_if(sib.begin(), sib.end(),
                    [node](const std::shared_ptr<SceneNode>& c) {return c.get() == node; }), sib.end());
                if (kept) { node->Parent = np; np->Children.push_back(kept); }
            }
        }
        return 0;
    }

    return 0;
}

static int L_PartToString(lua_State* L) {
    auto* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
    lua_pushstring(L, node ? node->Name.c_str() : "nil");
    return 1;
}

static int L_InstanceNew(lua_State* L) {
    auto* sc = static_cast<Scene*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* type = luaL_checkstring(L, 1);
    SceneNode* parent = nullptr;
    if (!lua_isnil(L, 2) && lua_isuserdata(L, 2))
        parent = static_cast<SceneNode*>(lua_touserdata(L, 2));

    SceneNode* node = nullptr;
    if (strcmp(type, "Part") == 0) node = sc->AddPart(parent, PartShape::Box);
    else if (strcmp(type, "Wedge") == 0) node = sc->AddPart(parent, PartShape::Wedge);
    else if (strcmp(type, "Sphere") == 0) node = sc->AddPart(parent, PartShape::Sphere);
    else if (strcmp(type, "Cylinder") == 0) node = sc->AddPart(parent, PartShape::Cylinder);
    else if (strcmp(type, "Model") == 0) node = sc->AddModel(parent);
    else if (strcmp(type, "Folder") == 0) node = sc->AddFolder(parent);
    else if (strcmp(type, "Script") == 0) node = sc->AddScript(parent);
    else { luaL_error(L, "Unknown instance type: %s", type); return 0; }

    PushNodeWithMeta(L, node);
    return 1;
}

static int L_GameGetService(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, k_SceneKey);
    auto* sc = static_cast<Scene*>(lua_touserdata(L, -1)); lua_pop(L, 1);
    const char* svc = luaL_checkstring(L, 2);
    if (strcmp(svc, "Workspace") == 0) { PushNodeWithMeta(L, sc->GetWorkspace());    return 1; }
    if (strcmp(svc, "Lighting") == 0) { PushNodeWithMeta(L, sc->GetLighting());     return 1; }
    if (strcmp(svc, "ScriptService") == 0) { PushNodeWithMeta(L, sc->GetScriptService()); return 1; }
    lua_pushnil(L);
    return 1;
}

// math helpers

static int L_MathClamp(lua_State* L) { double v = luaL_checknumber(L, 1), lo = luaL_checknumber(L, 2), hi = luaL_checknumber(L, 3); lua_pushnumber(L, v<lo ? lo : v>hi ? hi : v); return 1; }
static int L_MathLerp(lua_State* L) { double a = luaL_checknumber(L, 1), b = luaL_checknumber(L, 2), t = luaL_checknumber(L, 3); lua_pushnumber(L, a + (b - a) * t); return 1; }
static int L_MathSign(lua_State* L) { double n = luaL_checknumber(L, 1); lua_pushnumber(L, n < 0 ? -1 : n>0 ? 1 : 0); return 1; }
static int L_MathRound(lua_State* L) { double n = luaL_checknumber(L, 1); lua_pushnumber(L, floor(n + 0.5)); return 1; }

// table helpers

static int L_TableFind(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_Integer len = luaL_len(L, 1);
    for (lua_Integer i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        if (lua_rawequal(L, -1, 2)) { lua_pop(L, 1); lua_pushinteger(L, i); return 1; }
        lua_pop(L, 1);
    }
    lua_pushnil(L); return 1;
}
static int L_TableGetn(lua_State* L) { luaL_checktype(L, 1, LUA_TTABLE); lua_pushinteger(L, (lua_Integer)luaL_len(L, 1)); return 1; }
static int L_TableClear(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);
    while (lua_next(L, 1)) { lua_pop(L, 1); lua_pushvalue(L, -1); lua_pushnil(L); lua_settable(L, 1); }
    return 0;
}

// string helpers

static int L_StringSplit(lua_State* L) {
    std::string s = luaL_checkstring(L, 1), sep = luaL_checkstring(L, 2);
    lua_newtable(L); int idx = 1; size_t pos = 0, found;
    while ((found = s.find(sep, pos)) != std::string::npos) {
        lua_pushstring(L, s.substr(pos, found - pos).c_str()); lua_rawseti(L, -2, idx++); pos = found + sep.size();
    }
    lua_pushstring(L, s.substr(pos).c_str()); lua_rawseti(L, -2, idx); return 1;
}
static int L_StringTrim(lua_State* L) {
    std::string s = luaL_checkstring(L, 1);
    size_t a = s.find_first_not_of(" \t\n\r"), b = s.find_last_not_of(" \t\n\r");
    if (a == std::string::npos) { lua_pushstring(L, ""); return 1; }
    lua_pushstring(L, s.substr(a, b - a + 1).c_str()); return 1;
}

static int L_Wait(lua_State* L) { return 0; }

//  pushprint

void LuaRuntime::PushPrint() {

    // print

    lua_pushlightuserdata(m_L, this);
    lua_pushcclosure(m_L, [](lua_State* L)->int {
        auto* rt = static_cast<LuaRuntime*>(lua_touserdata(L, lua_upvalueindex(1)));
        int n = lua_gettop(L); std::string msg;
        for (int i = 1; i <= n; i++) { if (i > 1)msg += "\t"; msg += luaL_tolstring(L, i, nullptr); lua_pop(L, 1); }
        rt->m_Output.push_back({ ScriptOutput::Level::Print,msg }); return 0;
        }, 1);
    lua_setglobal(m_L, "print");

    // warn

    lua_pushlightuserdata(m_L, this);
    lua_pushcclosure(m_L, [](lua_State* L)->int {
        auto* rt = static_cast<LuaRuntime*>(lua_touserdata(L, lua_upvalueindex(1)));
        std::string msg = luaL_checkstring(L, 1);
        rt->m_Output.push_back({ ScriptOutput::Level::Warn,msg }); return 0;
        }, 1);
    lua_setglobal(m_L, "warn");

    // error

    lua_pushlightuserdata(m_L, this);
    lua_pushcclosure(m_L, [](lua_State* L)->int {
        auto* rt = static_cast<LuaRuntime*>(lua_touserdata(L, lua_upvalueindex(1)));
        std::string msg = luaL_optstring(L, 1, "error");
        rt->m_Output.push_back({ ScriptOutput::Level::Error,msg });
        return lua_error(L);
        }, 1);
    lua_setglobal(m_L, "error");
}

//  BindAPI

void LuaRuntime::BindAPI(Scene* scene) {
    lua_pushlightuserdata(m_L, scene);
    lua_setfield(m_L, LUA_REGISTRYINDEX, k_SceneKey);

    lua_pushlightuserdata(m_L, this);
    lua_setfield(m_L, LUA_REGISTRYINDEX, k_RuntimeKey);

    // Vector3

    lua_newtable(m_L);
    lua_pushcfunction(m_L, L_Vector3New); lua_setfield(m_L, -2, "new");
    PushVec3(m_L, 0, 0, 0); lua_setfield(m_L, -2, "zero");
    PushVec3(m_L, 1, 1, 1); lua_setfield(m_L, -2, "one");
    lua_setglobal(m_L, "Vector3");

    // Vector2

    lua_newtable(m_L);
    lua_pushcfunction(m_L, L_Vector2New); lua_setfield(m_L, -2, "new");
    PushVec2(m_L, 0, 0); lua_setfield(m_L, -2, "zero");
    PushVec2(m_L, 1, 1); lua_setfield(m_L, -2, "one");
    lua_setglobal(m_L, "Vector2");

    // Color3

    lua_newtable(m_L);
    lua_pushcfunction(m_L, L_Color3New);     lua_setfield(m_L, -2, "new");
    lua_pushcfunction(m_L, L_Color3FromRGB); lua_setfield(m_L, -2, "fromRGB");
    lua_pushcfunction(m_L, L_Color3FromHSV); lua_setfield(m_L, -2, "fromHSV");
    lua_setglobal(m_L, "Color3");

    // Part metatable

    luaL_newmetatable(m_L, "PartMeta");
    lua_pushstring(m_L, "__index");    lua_pushcfunction(m_L, L_PartIndex);    lua_settable(m_L, -3);
    lua_pushstring(m_L, "__newindex"); lua_pushcfunction(m_L, L_PartNewIndex); lua_settable(m_L, -3);
    lua_pushstring(m_L, "__tostring"); lua_pushcfunction(m_L, L_PartToString); lua_settable(m_L, -3);
    lua_pop(m_L, 1);

    // Instance

    lua_newtable(m_L);
    lua_pushlightuserdata(m_L, scene);
    lua_pushcclosure(m_L, L_InstanceNew, 1);
    lua_setfield(m_L, -2, "new");
    lua_setglobal(m_L, "Instance");

    // workspace

    PushNodeWithMeta(m_L, scene->GetWorkspace());
    lua_setglobal(m_L, "workspace");

    // game

    lua_newtable(m_L);
    PushNodeWithMeta(m_L, scene->GetWorkspace());    lua_setfield(m_L, -2, "Workspace");
    PushNodeWithMeta(m_L, scene->GetLighting());     lua_setfield(m_L, -2, "Lighting");
    PushNodeWithMeta(m_L, scene->GetScriptService()); lua_setfield(m_L, -2, "ScriptService");
    lua_pushcfunction(m_L, L_GameGetService);        lua_setfield(m_L, -2, "GetService");
    lua_setglobal(m_L, "game");

    // task

    lua_newtable(m_L);
    lua_pushcfunction(m_L, L_Wait); lua_setfield(m_L, -2, "wait");
    lua_pushcfunction(m_L, L_Wait); lua_setfield(m_L, -2, "spawn");
    lua_pushcfunction(m_L, L_Wait); lua_setfield(m_L, -2, "defer");
    lua_pushcfunction(m_L, L_Wait); lua_setfield(m_L, -2, "delay");
    lua_setglobal(m_L, "task");

    lua_pushcfunction(m_L, L_Wait);
    lua_setglobal(m_L, "wait");

    // math extensions

    lua_getglobal(m_L, "math");
    lua_pushcfunction(m_L, L_MathClamp); lua_setfield(m_L, -2, "clamp");
    lua_pushcfunction(m_L, L_MathLerp);  lua_setfield(m_L, -2, "lerp");
    lua_pushcfunction(m_L, L_MathSign);  lua_setfield(m_L, -2, "sign");
    lua_pushcfunction(m_L, L_MathRound); lua_setfield(m_L, -2, "round");
    lua_pop(m_L, 1);

    // table extensions

    lua_getglobal(m_L, "table");
    lua_pushcfunction(m_L, L_TableFind);  lua_setfield(m_L, -2, "find");
    lua_pushcfunction(m_L, L_TableGetn);  lua_setfield(m_L, -2, "getn");
    lua_pushcfunction(m_L, L_TableClear); lua_setfield(m_L, -2, "clear");
    lua_pop(m_L, 1);

    // string extensions

    lua_getglobal(m_L, "string");
    lua_pushcfunction(m_L, L_StringSplit); lua_setfield(m_L, -2, "split");
    lua_pushcfunction(m_L, L_StringTrim);  lua_setfield(m_L, -2, "trim");
    lua_pop(m_L, 1);
}

//  init

void LuaRuntime::Init(Scene* scene) {
    PushPrint();
    BindAPI(scene);
}

// runscript

bool LuaRuntime::RunScript(const std::string& source,
    const std::string& name,
    SceneNode* scriptNode) {
    if (scriptNode) {
        PushNodeWithMeta(m_L, scriptNode);
        lua_setglobal(m_L, "script");
    }

    int result = luaL_loadbuffer(m_L, source.c_str(), source.size(), name.c_str());
    if (result != LUA_OK) {
        std::string err = lua_tostring(m_L, -1); lua_pop(m_L, 1);
        m_Output.push_back({ ScriptOutput::Level::Error,err });
        return false;
    }
    result = lua_pcall(m_L, 0, 0, 0);
    if (result != LUA_OK) {
        std::string err = lua_tostring(m_L, -1); lua_pop(m_L, 1);
        m_Output.push_back({ ScriptOutput::Level::Error,err });
        return false;
    }
    return true;
}

// runallscripts

void LuaRuntime::RunAllScripts(Scene* scene) {
    std::vector<SceneNode*> scripts;
    scene->GetWorkspace()->CollectScripts(scripts);
    scene->GetScriptService()->CollectScripts(scripts);
    for (auto* node : scripts)
        RunScript(node->ScriptSource, node->Name, node);
}

// update

void LuaRuntime::Update(Scene* scene, float dt) {
    lua_getglobal(m_L, "onUpdate");
    if (lua_isfunction(m_L, -1)) {
        lua_pushnumber(m_L, (double)dt);
        if (lua_pcall(m_L, 1, 0, 0) != LUA_OK) {
            std::string err = lua_tostring(m_L, -1); lua_pop(m_L, 1);
            m_Output.push_back({ ScriptOutput::Level::Error,err });
        }
    }
    else {
        lua_pop(m_L, 1);
    }
}