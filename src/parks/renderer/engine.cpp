//
// Created by brett on 6/5/23.
//
#include "parks/renderer/engine.h"
#include <imgui.h>
#include <memory>
#include <blt/profiling/profiler.h>
#include <thread>
#include <mutex>
#include <barrier>
#include <genetic/v3/program_v3.h>

namespace parks {
    
    genetic::Program* p;
    
    Engine::Engine(const Settings& settings): settings(settings) {
        vao.bind();
        vao.createVBO(
                {GL_ARRAY_BUFFER, vertices, sizeof(vertices)},
                {{0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0},
                 {1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat)},
                 {2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat)}}
        );
        vao.createVBO({GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(indices)});
        
        basicQuadVAO.bind();
        basicQuadVAO.createVBO(
                {GL_ARRAY_BUFFER, basicQuad_v, sizeof(basicQuad_v)},
                {{0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0},
                 {1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat)}}
        );
        basicQuadVAO.createVBO({GL_ELEMENT_ARRAY_BUFFER, basicQuad_i, sizeof(basicQuad_i)});
        
        geneticImageTexture.allocate(GL_UNSIGNED_BYTE, WIDTH, HEIGHT, CHANNELS);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        p = new genetic::Program();
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
            
            static bool showImage = true;
            
            ImGui::ShowDemoWindow();
            ImGui::SetNextWindowSize({0, 512}, ImGuiCond_Once);
            ImGui::Begin("Genetic Controls");
                ImGui::Checkbox("Show Image Output?", &showImage);
                p->run();
            ImGui::End();
            geneticImageTexture.upload(p->getPixels(), GL_UNSIGNED_BYTE, WIDTH, HEIGHT, CHANNELS);
            
            if (showImage) {
                geneticImageTexture.bind();
                
                auto windowSize = Window::getWindowSize();
                
                blt::mat4x4 trans;
                trans.translate(((float)(windowSize.width - WIDTH))/2.0f, ((float)(windowSize.height - HEIGHT))/2.0f, 0);
                trans.scale(WIDTH, HEIGHT, 1);
                
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                uiShader.bind();
                uiShader.setMatrix("transform", trans);
                basicQuadVAO.bind();
                basicQuadVAO.bindEBO();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glEnable(GL_DEPTH_TEST);
            }
            
            Window::postUpdate();
        }
    }
    
    Engine::~Engine() {
        delete p;
        BLT_PRINT_PROFILE("Genetic", blt::logging::BLT_NONE, true);
    }
}