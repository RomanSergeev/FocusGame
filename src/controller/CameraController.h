#pragma once
#include "model/Camera.h"
#include "view/GLWindow.h"

class CameraController {
public:
    struct CameraSettings {
        static constexpr float PI2 = glm::half_pi<float>();
        static constexpr std::pair<float, float> LIMIT_SENSITIVITY = { 0.005f, 1.0f };
        static constexpr std::pair<float, float> LIMIT_ZOOM_STEP = { 0.01f, 10.0f };
        static constexpr std::pair<float, float> LIMIT_ZOOM_SMOOTH = { 0.0f, 1.0f };
        static constexpr std::pair<float, float> LIMIT_ROTATE_SLOWDOWN = { 0.1f, 100.0f };
        static constexpr std::pair<float, float> LIMIT_YAW = { -PI2 + 0.1f, PI2 - 0.1f };
        static constexpr float ROTATE_STOP_EPSILON = 0.0001f;

        float sensitivity = 0.005f;
        float zoomStep = 0.5f;
        float zoomSmoothFactor = 0.1f;
        float rotateSlowdown = 6.0f;
        // smooth rotation/zoom variables:
        bool smoothRotation = false;
        bool smoothZoom = false;
        bool invertedHorizontalMouse = true;
        bool invertedVerticalMouse = true;
        bool doClampYaw = false;

        CameraSettings();

        float getZoomMinimal() const { return zoomMin; }
        float getZoomMaximal() const { return zoomMax; }

        void setZoomLimits(float zoomFrom, float zoomTo);
        void sanitize();
    private:
        float zoomMin;
        float zoomMax;
    };
    CameraController(const GLWindow& window);

    const glm::mat4& getProjectionMatrix() const { return camera.getProjectionMatrix(); }

    void updateSettings(CameraSettings&& settings);
    void updateView(float timePassed);
    glm::mat4 getCameraView() const { return camera.getView(); };
    void setZoomLimits(float zoomFrom, float zoomTo);
private:
    CameraSettings settings;
    Camera camera;
    
    float targetDistance; // target distance (for smooth zoom)
    float yawVelocity = 0.0f;
    float pitchVelocity = 0.0f;

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

    void addYaw(float yawDelta);
    void addPitch(float pitchDelta);
    void clampDistance();
};