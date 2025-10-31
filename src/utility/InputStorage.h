#pragma once
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>
#include "glfw3.h"

class InputStorage {
    struct KeyState {
        static const KeyState DEFAULT;

        int lastState;
        bool justPressed;
        bool justReleased;
        KeyState(int _lastState, bool _justPressed, bool _justReleased);
    };
    std::unordered_map<int, KeyState> keyStates;
    std::unordered_map<std::string, std::vector<int>> keyGroups;

    template <typename Predicate>
    bool anyKey(const std::string& group, Predicate pred) const {
        auto it = keyGroups.find(group);
        if (it == keyGroups.end()) return false;
        for (int key : it->second)
            if (pred(key)) return true;
        return false;
    }

public:
    void update(GLFWwindow* window);
    const KeyState& getState(int key) const;
    void registerKey(int key) { keyStates.try_emplace(key, KeyState::DEFAULT ); }
    void registerKeyGroup(const std::string& groupName, std::initializer_list<int> keys);
    bool isPressed(int key) const { return getState(key).lastState == GLFW_PRESS; }
    bool isPressed(const std::string& groupName) const;
    bool justPressed(int key) const { return getState(key).justPressed; }
    bool justPressed(const std::string& groupName) const;
    bool justReleased(int key) const { return getState(key).justReleased; }
    bool noModifiers() const;
};