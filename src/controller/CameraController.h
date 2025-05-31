#include "view/GLWindow.h"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/ext/matrix_transform.hpp"

class CameraController {
public:
    struct CameraSettings {
        float sensitivity = 0.005f;
        float zoomStep = 0.5f;
        float zoomSmoothFactor = 0.1f;
        float rotateSlowdown = 6.0f;
        float rotateEpsilon = 0.0001f;
        float yawMax =  glm::half_pi<float>() - 0.1f;
        float pitchMax =  glm::half_pi<float>() - 0.1f;
        // smooth rotation/zoom variables:
        bool smoothRotation = false;
        bool smoothZoom = false;
        bool invertedHorizontalMouse = true;
        bool invertedVerticalMouse = true;
        bool doClampYaw = false;
        bool doClampPitch = true;
    private:
        float zoomMin = 2.0f;
        float zoomMax = 20.0f;

        friend class CameraController;
    };
    CameraController(const GLWindow& window, const CameraSettings& settings, float radius = 5.0f);

    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }

    void updateView(float timePassed);
    glm::mat4 getView() const;
    void setZoomLimits(float zoomMin, float zoomMax);
    void setAspectRatio(float ratio);
private:
    static constexpr float DEFAULT_ZOOM = 1.0f;

    CameraSettings settings;
    float yaw   = 0.0f;  // horizontal angle (around Y)
    float pitch = 0.0f;  // vertical angle (around X)
    float radius; // distance from origin
    
    float targetRadius; // target distance (for smooth zoom)
    float yawVelocity = 0.0f;
    float pitchVelocity = 0.0f;
    glm::mat4 projectionMatrix;

    bool rotating = false;
    double lastX = 0.0, lastY = 0.0;

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    void handleMouseButton(int button, int action, int mods);
    void handleMousePosition(GLFWwindow* window, double xpos, double ypos);
    void handleMouseScroll(double xOffset, double yOffset);
    void handleWindowResize(int width, int height);

    void clampYaw();
    void clampPitch();
    void clampRadius();
};