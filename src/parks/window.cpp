#include <parks/window.h>
#include <blt/std/logging.h>
#include <string>
#include "parks/status.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace parks {

    std::string decodeGLFWError(int code){
        switch (code) {
            case GLFW_NO_ERROR:
                return "NO ERROR";
            case GLFW_NOT_INITIALIZED:
                return "glfwInit() has not been called!";
            case GLFW_NO_CURRENT_CONTEXT:
                return "GLFW context is not current in this thread!";
            case GLFW_INVALID_ENUM:
                return "Provided enum is invalid!";
            case GLFW_INVALID_VALUE:
                return "Provided value is invalid!";
            case GLFW_OUT_OF_MEMORY:
                return "Out of memory!";
            case GLFW_API_UNAVAILABLE:
                return "System does not support requested graphics API!";
            case GLFW_VERSION_UNAVAILABLE:
                return "System does not support requested API version!";
            case GLFW_PLATFORM_ERROR:
                return "Platform Error";
            case GLFW_FORMAT_UNAVAILABLE:
                return "Requested format is unavailable or not supported!";
            case GLFW_NO_WINDOW_CONTEXT:
                return "Window does not have an active graphics API context!";
            default:
                return "Unable To decode error!";
        }
    }
    GLFWwindow* window;

    GLFWwindow *Window::getWindow() {
        return window;
    }

    void initGLFW() {
        glfwSetErrorCallback([](int code, const char* desc) -> void {
            BLT_ERROR("%d (%s)", code, desc);
        });

        BLT_TRACE("Complied with GLFW %i.%i.%i",
                  GLFW_VERSION_MAJOR,
                  GLFW_VERSION_MINOR,
                  GLFW_VERSION_REVISION);

        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);

        BLT_DEBUG("Using GLFW %i.%i.%i", major, minor, revision);

        if (!glfwInit()){
            const char* desc;
            int code = glfwGetError(&desc);
            BLT_FATAL("Unable to create GLFW window! (%s)", decodeGLFWError(code).c_str());
            BLT_FATAL("%s", desc);
            std::exit(GLFW_ERROR);
        }
    }

    void createWindow(const Settings& settings){
//        if (properties.windowMode != WINDOWED){
//            const GLFWvidmode* mode = glfwGetVideoMode((GLFWmonitor*) properties.monitor);
//
//            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
//            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
//            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
//            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//        }

        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        // TODO:
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(settings.getProperty(Properties::WINDOW_WIDTH)->as<Properties::Value<int>>()->getValue(),
                                  settings.getProperty(Properties::WINDOW_HEIGHT)->as<Properties::Value<int>>()->getValue(),
                                  settings.getProperty(Properties::WINDOW_TITLE)->as<Properties::Value<std::string>>()->getValue().c_str(),
                                    nullptr, nullptr);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* _, int width, int height) -> void {
            glViewport(0, 0, width, height);
        });

        glfwMakeContextCurrent(window);
    }

    void Window::create(const Settings &settings) {
        initGLFW();
        createWindow(settings);

        glfwSwapInterval(1);
    }

    void Window::destroy() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::postUpdate() {
        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    bool Window::isCloseRequested() {
        return glfwWindowShouldClose(window);
    }

    void Window::setCloseRequested(bool shouldClose) {
        glfwSetWindowShouldClose(window, shouldClose);
    }

    void Window::setupGLAD() {
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            BLT_FATAL("Failed to initialize OpenGL context!");
            std::exit(parks::GL_ERROR);
        }
        BLT_INFO("Loaded OpenGL %d.%d", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    }

    void Window::setupDearImGUI() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 150");
    }

    void Window::preUpdate() {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

}