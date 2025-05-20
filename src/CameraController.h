#include "OpenGLhub.h"

struct CameraSettings {
    float sensitivity = 0.005f;
    float zoomStep = 0.5f;
    float zoomMin = 2.0f;
    float zoomMax = 20.0f;
    float zoomSmooth = 0.1f;
    float rotateSlowdown = 0.9f;
    float rotateEpsilon = 0.0001f;
    float pitchMax =  glm::half_pi<float>() - 0.1f;
};

class CameraController {
    CameraSettings settings;
    float yaw   = 0.0f;  // horizontal angle (around Y)
    float pitch = 0.0f;  // vertical angle (around X)
    float radius; // distance from origin
    
    // smooth rotation/zoom variables:
    bool smoothRotation = false;
    bool smoothZoom = false;
    float targetRadius; // target distance (for smooth zoom)
    float yawVelocity = 0.0f;
    float pitchVelocity = 0.0f;

    bool rotating = false;
    double lastX = 0.0, lastY = 0.0;

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    void handleMouseButton(int button, int action, int mods);
    void handleMousePosition(double xpos, double ypos);
    void handleMouseScroll(double xOffset, double yOffset);

    void clampPitch();
public:
    CameraController(GLFWwindow* window, float radius = 5.0f);

    void updateView();
    glm::mat4 getView() const;
    inline void setSmoothRotation(bool smooth) { smoothRotation = smooth; }
    inline void setSmoothZoom(bool smooth) { smoothZoom = smooth; }
};