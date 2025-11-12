#include "SingleInstanceMonitor.h"

bool SingleInstance::isPrimaryInstance() const {
    return isPrimary;
}

#ifdef _WIN32
#include <windows.h>

SingleInstance::SingleInstance(const std::string& uniqueName) {
    handle = CreateMutexA(nullptr, TRUE, uniqueName.c_str());
    if (handle && GetLastError() != ERROR_ALREADY_EXISTS) {
        isPrimary = true;
    }
}

SingleInstance::~SingleInstance() {
    if (isPrimary && handle) {
        ReleaseMutex(static_cast<HANDLE>(handle));
    }
    if (handle) {
        CloseHandle(static_cast<HANDLE>(handle));
    }
}

void SingleInstance::focusExistingInstance(const std::string& uniqueName) {
    // Optional: find and activate the existing window by title/class
    HWND hwnd = FindWindowA(nullptr, "Focus Game"); // replace with your window title
    if (hwnd) {
        ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
    }
}
#endif // _WIN32

// Added a single instance block for Linux and Mac, but still need to modify CMakeLists and CLang toolchain
// as well as /usr/include and /usr/lib headers, to make it compile
#if defined(__unix__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <iostream>

SingleInstance::SingleInstance(const std::string& uniqueName) {
    std::string lockPath = "/tmp/" + uniqueName + ".lock";
    int fd = open(lockPath.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd < 0) return;

    if (flock(fd, LOCK_EX | LOCK_NB) == 0) {
        isPrimary = true;
    } else {
        close(fd);
        return;
    }
    handle = reinterpret_cast<void*>(static_cast<intptr_t>(fd));
}

SingleInstance::~SingleInstance() {
    if (isPrimary && handle) {
        int fd = static_cast<int>(reinterpret_cast<intptr_t>(handle));
        flock(fd, LOCK_UN);
        close(fd);
    }
}

void SingleInstance::focusExistingInstance(const std::string&) {
    // Not easily done portably; requires IPC (e.g. X11 message or DBus)
}
#endif // __unix__ || __APPLE__