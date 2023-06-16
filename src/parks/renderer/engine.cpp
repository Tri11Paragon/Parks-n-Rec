//
// Created by brett on 6/5/23.
//
#include "parks/renderer/engine.h"

namespace parks {
    
    Engine::Engine(const Settings& settings): settings(settings) {
        vao.bind();
        vao.createVBO(
                {GL_ARRAY_BUFFER, vertices, sizeof(vertices)},
                {{0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0},
                 {1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat)},
                 {2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat)}}
        );
        vao.createVBO({GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices)});
    }
    
    void Engine::run() {
        while (!Window::isCloseRequested()) {
            Window::preUpdate();
            glClearColor(0, 0.5, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            basicCameraController.update();

#ifdef BUILD_DEV_TOOLS
            auto renderMode = settings.getProperty<int>(
                    Properties::RENDER_MODE
            );
            if (renderMode)
                glPolygonMode(GL_FRONT_AND_BACK, renderMode->getValue());
            if (Window::keyPressedLastFrame(GLFW_KEY_ESCAPE))
                Window::setMouseVisible(!Window::isMouseVisible());
#endif
            
            testShader.bind();
            glActiveTexture(GL_TEXTURE0);
            resources::getTexture("test.png")->bind();
            vao.bind();
            vao.bindEBO();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#ifdef BUILD_DEV_TOOLS
            if (renderMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
            Window::postUpdate();
        }
    }
    
    Engine::~Engine() {
    
    }
}