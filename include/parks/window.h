#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <parks/app.h>

namespace parks::Window {
    void create(const Settings &settings);
    void setupGLAD();
    void setupDearImGUI();
    void destroy();
    void preUpdate();
    void postUpdate();
    bool isCloseRequested();
    void setCloseRequested(bool shouldClose);

    GLFWwindow *getWindow();
}