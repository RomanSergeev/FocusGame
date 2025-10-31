#include "InputStorage.h"

InputStorage::KeyState::KeyState(int _lastState, bool _justPressed, bool _justReleased) : lastState(_lastState), justPressed(_justPressed), justReleased(_justReleased) {}

const InputStorage::KeyState InputStorage::KeyState::DEFAULT = InputStorage::KeyState(GLFW_RELEASE, false, false);

void InputStorage::update(GLFWwindow* window) {
        for (auto& [key, state] : keyStates) {
            int newState = glfwGetKey(window, key);
            state.justPressed = (newState == GLFW_PRESS && state.lastState == GLFW_RELEASE);
            state.justReleased = (newState == GLFW_RELEASE && state.lastState == GLFW_PRESS);
            state.lastState = newState;
        }
    }

const InputStorage::KeyState& InputStorage::getState(int key) const {
    auto iter = keyStates.find(key);
    return iter == keyStates.end() ? KeyState::DEFAULT : iter->second;
}

void InputStorage::registerKeyGroup(const std::string& groupName, std::initializer_list<int> keys) {
    keyGroups[groupName] = keys;
    for (int key : keys)
        registerKey(key);
}

bool InputStorage::isPressed(const std::string& groupName) const {
    return anyKey(groupName, [&](int key) { return isPressed(key); });
}

bool InputStorage::justPressed(const std::string& groupName) const {
    return anyKey(groupName, [&](int key) { return justPressed(key); });
}

bool InputStorage::noModifiers() const {
    return !isPressed("Alt") && !isPressed("Ctrl") && !isPressed("Shift");
}