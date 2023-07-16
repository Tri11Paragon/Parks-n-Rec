#include <parks/window.h>
#include <blt/std/logging.h>
#include <string>
#include "parks/status.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_spectrum.h>
#include <blt/std/time.h>

namespace parks {
    
    std::string decodeGLFWError(int code) {
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
    Window::WindowSize windowSize;
    double delta;
    int64_t lastTime;
    
    struct {
        bool mouseState[128];
        bool keyState[512];
        bool keyStateLastFrame[512];
        double dx, dy;
        double lx, ly;
        bool pressedLastFrame = false;
        bool movedLastFrame = false;
    } Input;
    
    struct {
        blt::mat4x4 viewMatrix;
        blt::mat4x4 perspectiveMatrix;
        blt::mat4x4 pvMatrix;
        blt::mat4x4 orthoMatrix;
        GLuint uboID = 0;
    } Matrices;
    
    void initGLFW() {
        glfwSetErrorCallback(
                [](int code, const char* desc) -> void {
                    BLT_ERROR("%d (%s)", code, desc);
                }
        );
        
        BLT_TRACE("Complied with GLFW %i.%i.%i",
                  GLFW_VERSION_MAJOR,
                  GLFW_VERSION_MINOR,
                  GLFW_VERSION_REVISION);
        
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);
        
        BLT_DEBUG("Using GLFW %i.%i.%i", major, minor, revision);
        
        if (!glfwInit()) {
            const char* desc;
            int code = glfwGetError(&desc);
            BLT_FATAL("Unable to create GLFW window! (%s)", decodeGLFWError(code).c_str());
            BLT_FATAL("%s", desc);
            std::exit(GLFW_ERROR);
        }
    }
    
    void createWindow(const Settings& settings) {
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
        
        window = glfwCreateWindow(
                settings.getProperty<int>(Properties::WINDOW_WIDTH)->getValue(),
                settings.getProperty<int>(Properties::WINDOW_HEIGHT)->getValue(),
                settings.getProperty<std::string>(Properties::WINDOW_TITLE)->getValue().c_str(),
                nullptr, nullptr
        );
        
        glfwSetFramebufferSizeCallback(
                window, [](GLFWwindow* _, int width, int height) -> void {
                    windowSize.width = width;
                    windowSize.height = height;
                    glViewport(0, 0, width, height);
                    auto pers = blt::perspective(90, (float) width / (float) height, 0.1f, 500.0f);
                    auto ortho = blt::ortho(0, (float)width, 0, (float)height, -1, 1);
                    Window::updatePerspectiveMatrix(pers);
                    Window::updateOrthograhpicMatrix(ortho);
                }
        );
        
        glfwSetKeyCallback(
                window, [](GLFWwindow* _, int key, int scancode, int action, int mods) -> void {
                    Input.keyStateLastFrame[key] = action == GLFW_PRESS;
                    Input.keyState[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
                }
        );
        
        glfwSetMouseButtonCallback(
                window, [](GLFWwindow* _, int button, int action, int mods) -> void {
                    Input.mouseState[button] = action == GLFW_PRESS || action == GLFW_REPEAT;
                    Input.pressedLastFrame = true;
                }
        );
        
        glfwSetCursorPosCallback(
                window, [](GLFWwindow* _, double x, double y) -> void {
                    Input.dx = x - Input.lx;
                    Input.dy = y - Input.ly;
                    Input.lx = x;
                    Input.ly = y;
                    Input.movedLastFrame = true;
                }
        );
        
        glfwMakeContextCurrent(window);
        glfwGetWindowSize(window, &windowSize.width, &windowSize.height);
    }
    
    void setupSharedWindowMatrices() {
        glGenBuffers(1, &Matrices.uboID);
        glBindBuffer(GL_UNIFORM_BUFFER, Matrices.uboID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(blt::mat4x4) * parks::Window::UBO_MATRICES_COUNT, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, Matrices.uboID);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    void Window::create(const Settings& settings) {
        initGLFW();
        createWindow(settings);
        Window::setupGLAD();
        Window::setupDearImGUI();
        setupSharedWindowMatrices();
        
        glfwSwapInterval(1);
    }
    
    void Window::destroy() {
        glfwDestroyWindow(window);
        glfwTerminate();
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
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        
        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        ImGui::Spectrum::StyleColorsSpectrum();
        ImGui::Spectrum::LoadFont(16);
        
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
    
    void Window::postUpdate() {
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        auto currentTime = blt::system::getCurrentTimeNanoseconds();
        delta = (double) (currentTime - lastTime) / 1000000000.0;
        lastTime = currentTime;
        Input.pressedLastFrame = false;
        Input.movedLastFrame = false;
        for (int i = 0; i < 512; i++)
            Input.keyStateLastFrame[i] = false;
    }
    
    void Window::updateViewMatrix(const blt::mat4x4& view) {
        Matrices.viewMatrix = view;
        Matrices.pvMatrix = Matrices.perspectiveMatrix * Matrices.viewMatrix;
        glBindBuffer(GL_UNIFORM_BUFFER, Matrices.uboID);
        glBufferSubData(
                GL_UNIFORM_BUFFER, sizeof(blt::mat4x4), sizeof(blt::mat4x4),
                Matrices.viewMatrix.ptr());
        glBufferSubData(
                GL_UNIFORM_BUFFER, sizeof(blt::mat4x4) * 2, sizeof(blt::mat4x4),
                Matrices.pvMatrix.ptr());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    void Window::updateOrthograhpicMatrix(const blt::mat4x4& perspective) {
        Matrices.orthoMatrix = perspective;
        glBindBuffer(GL_UNIFORM_BUFFER, Matrices.uboID);
        glBufferSubData(
                GL_UNIFORM_BUFFER, sizeof(blt::mat4x4) * 3, sizeof(blt::mat4x4), Matrices.orthoMatrix.ptr());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    void Window::updatePerspectiveMatrix(const blt::mat4x4& perspective) {
        Matrices.perspectiveMatrix = perspective;
        Matrices.pvMatrix = Matrices.perspectiveMatrix * Matrices.viewMatrix;
        glBindBuffer(GL_UNIFORM_BUFFER, Matrices.uboID);
        glBufferSubData(
                GL_UNIFORM_BUFFER, 0, sizeof(blt::mat4x4), Matrices.perspectiveMatrix.ptr());
        glBufferSubData(
                GL_UNIFORM_BUFFER, sizeof(blt::mat4x4) * 2, sizeof(blt::mat4x4),
                Matrices.pvMatrix.ptr());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    const blt::mat4x4& Window::getViewMatrix() {
        return Matrices.viewMatrix;
    }
    
    const blt::mat4x4& Window::getPerspectiveMatrix() {
        return Matrices.perspectiveMatrix;
    }
    
    const Window::WindowSize& Window::getWindowSize() {
        return windowSize;
    }
    
    double Window::getFrameDeltaSeconds() {
        return delta;
    }
    
    bool Window::isKeyDown(int key) {
        return Input.keyState[key];
    }
    
    bool Window::isMouseDown(int mouse) {
        return Input.mouseState[mouse];
    }
    
    bool Window::isMouseVisible() {
        return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
    }
    
    void Window::setMouseVisible(bool state) {
        glfwSetInputMode(window, GLFW_CURSOR, state ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    
    double Window::getMouseX() {
        return Input.lx;
    }
    
    double Window::getMouseY() {
        return Input.ly;
    }
    
    double Window::getMouseDX() {
        return Input.dx;
    }
    
    double Window::getMouseDY() {
        return Input.dy;
    }
    
    bool Window::mousePressedLastFrame() {
        return Input.pressedLastFrame;
    }
    
    bool Window::mouseMovedLastFrame() {
        return Input.movedLastFrame;
    }
    
    bool Window::keyPressedLastFrame(int key) {
        return Input.keyStateLastFrame[key];
    }
    
    
}