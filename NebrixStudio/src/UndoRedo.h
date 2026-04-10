#pragma once

// utils

#include <vector>
#include <functional>
#include <string>

struct UndoAction {
    std::string         Description;
    std::function<void()> Undo;
    std::function<void()> Redo;
};

class UndoStack {
public:
    void Push(UndoAction action) {

        // clear history

        m_Stack.erase(m_Stack.begin() + m_Index, m_Stack.end());
        m_Stack.push_back(action);
        m_Index = (int)m_Stack.size();
    }

    void DoUndo() {
        if (m_Index <= 0) return;
        m_Index--;
        m_Stack[m_Index].Undo();
    }

    void DoRedo() {
        if (m_Index >= (int)m_Stack.size()) return;
        m_Stack[m_Index].Redo();
        m_Index++;
    }

    bool CanUndo() const { return m_Index > 0; }
    bool CanRedo() const { return m_Index < (int)m_Stack.size(); }

private:
    std::vector<UndoAction> m_Stack;
    int                     m_Index = 0;
};