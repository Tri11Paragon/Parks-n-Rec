//
// Created by brett on 6/14/23.
//

#ifndef PARKSNREC_PLAYER_H
#define PARKSNREC_PLAYER_H

#include <blt/math/vectors.h>
#include "blt/math/matrix.h"

namespace parks {
    
    class Controller;
    class CameraController;
    class NetworkedController;
    
    class Player {
            friend Controller;
            friend CameraController;
            friend NetworkedController;
        private:
            blt::vec3d pos;
            blt::vec3d rot;
            const float baseMovementSpeed = 10.0f;
            const float sprintingMovementSpeed = 20.0f;
            const float flyingMovementSpeed = 50.0f;
            const float sensitivity = 5.0f;
        public:
        
    };
    
    class Controller {
        protected:
            Player& player;
        public:
            explicit Controller(Player& player): player(player) {}
            virtual void update() = 0;
    };
    
    class CameraController : public Controller {
        private:
            blt::mat4x4 generateViewMatrix();
        public:
            explicit CameraController(Player& player): Controller(player) {
                player.pos;
            }
            void update() final;
    };
    
    class NetworkedController : public Controller {
        private:
        
        public:
            explicit NetworkedController(Player& player): Controller(player) {
                    player.pos;
            }
    };

}

#endif //PARKSNREC_PLAYER_H
