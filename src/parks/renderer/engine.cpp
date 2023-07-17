//
// Created by brett on 6/5/23.
//
#include "parks/renderer/engine.h"
#include <imgui.h>
#include <genetic/old/genetic.h>
#include <memory>
#include <blt/profiling/profiler.h>
#include <thread>
#include <mutex>
#include <barrier>
#include <genetic/old/genetic.h>

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
        
        basicQuadVAO.bind();
        basicQuadVAO.createVBO(
                {GL_ARRAY_BUFFER, basicQuad_v, sizeof(basicQuad_v)},
                {{0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0},
                 {1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat)}}
        );
        basicQuadVAO.createVBO({GL_ELEMENT_ARRAY_BUFFER, basicQuad_i, sizeof(basicQuad_i)});
        
        geneticImageTexture.allocate(GL_UNSIGNED_BYTE, gtWidth, gtHeight, gtChannels);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        p = std::make_unique<genetic::Program>();
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
            static bool outputConsole = false;
            p->drawTree();
            ImGui::ShowDemoWindow();
            ImGui::SetNextWindowSize({0, 512}, ImGuiCond_Once);
            ImGui::Begin("Genetic Controls");
                ImGui::Checkbox("Show Image Output?", &showImage);
                ImGui::Checkbox("Show Debug Output?", &outputConsole);
                if (ImGui::Button("Generate Image")) {
                    constructImage(outputConsole);
                }
                if (ImGui::Button("Regen Program And Run")) {
                    old = std::move(p);
                    p = std::make_unique<genetic::Program>();
                    constructImage(outputConsole);
                }
                if (ImGui::Button("Crossover")){
                    auto np = p->crossover(old.get());
                    old = std::move(p);
                    p = std::unique_ptr<genetic::Program>(np);
                    BLT_TRACE("cross| np: %d, old: %d, p: %d, save: %d", np, old.get(), p.get(), save.get());
                }
                if (ImGui::Button("Mutate")){
                    p->mutate();
                }
                if (ImGui::Button("Save")){
                    save = std::move(p);
                    p = std::make_unique<genetic::Program>();
                    BLT_TRACE("save| old: %d, p: %d, save: %d", old.get(), p.get(), save.get());
                }
                if (ImGui::Button("Revert")){
                    old = std::move(p);
                    p = std::move(save);
                    BLT_TRACE("revert| old: %d, p: %d, save: %d", old.get(), p.get(), save.get());
                }
                if (ImGui::Button("Rescale Color")){
                    double dR = g_maxR - g_minR;
                    double dG = g_maxG - g_minG;
                    double dB = g_maxB - g_minB;
                    for (unsigned int i = 0; i < gtWidth; i++) {
                        for (unsigned int j = 0; j < gtHeight; j++) {
                            const auto pos = i * gtChannels + j * gtChannels * gtWidth;
                            displayProgress = (float) (j * gtChannels +
                                                       i * gtChannels * gtWidth) /
                                              (float) (gtWidth * gtHeight * gtChannels);
                            pixels[pos] = (unsigned char) (((values[pos] - g_minR) / dR) *
                                                           255);
                            pixels[pos + 1] = (unsigned char) (
                                    ((values[pos + 1] - g_minG) / dG) * 255);
                            pixels[pos + 2] = (unsigned char) (
                                    ((values[pos + 2] - g_minB) / dB) * 255);
                        }
                    }
                }
                if (ImGui::CollapsingHeader("Progress")) {
                    ImGui::Text("Render Progress: ");
                    for (const auto progress : renderingProgress)
                        ImGui::ProgressBar(progress);
                    ImGui::Text("Display Progress: ");
                    ImGui::ProgressBar(displayProgress);
                }
                
                int count = 0;
                for (auto b : completedThreads)
                    if (b)
                        count++;
                if (count == threads){
                    double dR = g_maxR - g_minR;
                    double dG = g_maxG - g_minG;
                    double dB = g_maxB - g_minB;
                    for (unsigned int i = 0; i < gtWidth; i++) {
                        for (unsigned int j = 0; j < gtHeight; j++) {
                            const auto pos = i * gtChannels + j * gtChannels * gtWidth;
                            displayProgress = (float) (j * gtChannels +
                                                                 i * gtChannels * gtWidth) /
                                                        (float) (gtWidth * gtHeight * gtChannels);
                            pixels[pos] = (unsigned char) (((values[pos] - g_minR) / dR) *
                                                           255);
                            pixels[pos + 1] = (unsigned char) (
                                    ((values[pos + 1] - g_minG) / dG) * 255);
                            pixels[pos + 2] = (unsigned char) (
                                    ((values[pos + 2] - g_minB) / dB) * 255);
                        }
                    }
                }
                geneticImageTexture.upload(pixels, GL_UNSIGNED_BYTE, gtWidth, gtHeight, gtChannels);
            ImGui::End();
            
            if (showImage) {
                geneticImageTexture.bind();
                
                auto windowSize = Window::getWindowSize();
                
                blt::mat4x4 trans;
                trans.translate(((float)(windowSize.width - gtWidth))/2.0f, ((float)(windowSize.height - gtHeight))/2.0f, 0);
                trans.scale(gtWidth, gtHeight, 1);
                
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
        running = false;
        for (auto*& t : runningThread) {
            if (t == nullptr)
                continue;
            t->join();
            delete t;
        }
        BLT_PRINT_PROFILE("Genetic", blt::logging::BLT_NONE, true);
    }
}