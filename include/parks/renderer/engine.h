//
// Created by brett on 6/5/23.
//

#ifndef PARKSNREC_ENGINE_H
#define PARKSNREC_ENGINE_H

#include <parks/renderer/resources.h>
#include "parks/window.h"
#include "blt/math/vectors.h"
#include "parks/config.h"
#include <parks/shader/basic_shader.vert>
#include <parks/shader/ui_shader.vert>
#include <parks/shader/basic_shader.frag>
#include <parks/shader/ui_shader.frag>
#include <parks/renderer/player.h>

namespace parks {
    struct StaticEntity {
        blt::vec3 pos;
        uint32_t modelID;
        blt::vec3 rot;
        uint32_t textureID;
    };
    
    class Renderer {
        private:
        
        public:
        
    };
    
    static GLfloat vertices[] = {
            // Positions // Colors // Texture Coords
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Top Left
    };
    static GLuint indices[] = { // Note that we start from 0!
            0, 1, 3, // First Triangle
            1, 2, 3 // Second Triangle
    };
    
    static GLfloat basicQuad_v[] = {
            // Positions // Texture Coords
            1, 1, 0.0f, 1.0f, 1.0f, // Top Right
            1, 0, 0.0f, 1.0f, 0.0f, // Bottom Right
            0, 0, 0.0f, 0.0f, 0.0f, // Bottom Left
            0, 1, 0.0f, 0.0f, 1.0f // Top Left
    };
    static GLuint basicQuad_i[] = { // Note that we start from 0!
            0, 1, 3, // First Triangle
            1, 2, 3 // Second Triangle
    };
    
    class Engine {
        private:
            Shader testShader{BasicShaderVertex, BasicShaderFragment};
            Shader uiShader{UIShaderVertex, UIShaderFragment};
            VAOStorageObject vao;
            VAOStorageObject basicQuadVAO;
            GLTexture2D geneticImageTexture;
            const Settings& settings;
            
            Player player;
            CameraController basicCameraController {player};
            
        public:
            explicit Engine(const Settings& settings);
            
            void run();
            
            ~Engine();
    };
}

#endif //PARKSNREC_ENGINE_H
