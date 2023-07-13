//
// Created by brett on 6/5/23.
//
#include "parks/renderer/engine.h"
#include <imgui.h>
#include <genetic/genetic.h>
#include <memory>

namespace parks {
    
    constexpr unsigned int gtWidth = 512;
    constexpr unsigned int gtHeight = 512;
    constexpr unsigned int gtChannels = 4;
    
    std::unique_ptr<genetic::Program> p;
    double values[gtWidth * gtHeight * gtChannels];
    unsigned char pixels[gtWidth * gtHeight * gtChannels];
    
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
    
    static void constructImage(bool outputConsole){
        double minR = std::numeric_limits<double>::max(), maxR = std::numeric_limits<double>::min();
        double minG = std::numeric_limits<double>::max(), maxG = std::numeric_limits<double>::min();
        double minB = std::numeric_limits<double>::max(), maxB = std::numeric_limits<double>::min();
        for (unsigned int i = 0; i < gtWidth; i++){
            for (unsigned int j = 0; j < gtHeight; j++){
                const auto pos = i * gtChannels + j * gtChannels * gtWidth;
                genetic::Color c = p->apply((double)i, (double)j, 0);
                auto v = c.v();
                values[pos] = c.r;
                values[pos + 1] = c.g;
                values[pos + 2] = c.b;
                values[pos + 3] = 255;
                minR = std::min(minR, c.r);
                minG = std::min(minG, c.g);
                minB = std::min(minB, c.b);
                maxR = std::max(maxR, c.r);
                maxG = std::max(maxG, c.g);
                maxB = std::max(maxB, c.b);
                if (outputConsole && i % gtWidth == 0)
                    BLT_TRACE("(%f, %f, %f) Value: %f @ %d,%d, adj: %d, %d, %d", c.r, c.g, c.b, v, i, j, pixels[i * gtChannels + j * gtChannels * gtWidth], pixels[i * gtChannels + j * gtChannels * gtWidth + 1], pixels[i * gtChannels + j * gtChannels * gtWidth + 2]);
            }
        }
        double dR = maxR - minR;
        double dG = maxG - minG;
        double dB = maxB - minB;
        for (unsigned int i = 0; i < gtWidth; i++) {
            for (unsigned int j = 0; j < gtHeight; j++) {
                const auto pos = i * gtChannels + j * gtChannels * gtWidth;
                pixels[pos] = (unsigned char)(((values[pos] - minR)/dR) * 255);
                pixels[pos + 1] = (unsigned char)(((values[pos + 1] - minG)/dG) * 255);
                pixels[pos + 2] = (unsigned char)(((values[pos + 2] - minB)/dB) * 255);
            }
        }
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
            ImGui::SetNextWindowSize({0, 256}, ImGuiCond_Once);
            ImGui::Begin("Genetic Controls");
                ImGui::Checkbox("Show Image Output?", &showImage);
                ImGui::Checkbox("Show Debug Output?", &outputConsole);
                if (ImGui::Button("Generate Image")) {
                    constructImage(outputConsole);
                }
                if (ImGui::Button("Regen Program And Run")) {
                    p = std::make_unique<genetic::Program>();
                    constructImage(outputConsole);
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
    
    }
}