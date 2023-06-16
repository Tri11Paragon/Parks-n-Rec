#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <parks/app.h>
#include <blt/math/math.h>

namespace parks::Window {
    
    struct WindowSize {
        int width, height;
    };
    
    void create(const Settings &settings);
    void setupGLAD();
    void setupDearImGUI();
    void destroy();
    void preUpdate();
    void postUpdate();
    bool isCloseRequested();
    void setCloseRequested(bool shouldClose);
    void updateViewMatrix(const blt::mat4x4& view);
    void updatePerspectiveMatrix(const blt::mat4x4& perspective);
    const blt::mat4x4& getViewMatrix();
    const blt::mat4x4& getPerspectiveMatrix();
    const WindowSize& getWindowSize();
    double getFrameDeltaSeconds();
    bool isKeyDown(int key);
    bool isMouseDown(int mouse);
    bool isMouseVisible();
    void setMouseVisible(bool state);
    double getMouseX();
    double getMouseDX();
    double getMouseY();
    double getMouseDY();
    bool mousePressedLastFrame();
    bool mouseMovedLastFrame();
    bool keyPressedLastFrame(int key);
}