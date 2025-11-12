#pragma once
#include <string>

class SingleInstance {
public:
    // Create and try to acquire an instance lock.
    // `uniqueName` must be the same for all launches of your app.
    explicit SingleInstance(const std::string& uniqueName);
    ~SingleInstance();

    // True if this process owns the lock
    bool isPrimaryInstance() const;

    // Bring an existing instance window to the front (optional, Windows only)
    static void focusExistingInstance(const std::string& uniqueName);

private:
    bool isPrimary = false;
    void* handle = nullptr; // OS-specific handle
};