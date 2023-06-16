//
// Created by brett on 6/14/23.
//
#include <parks/renderer/player.h>
#include <parks/window.h>
#include <imgui.h>
#include <parks/config.h>

namespace parks {
    
    blt::mat4x4 CameraController::generateViewMatrix() {
        blt::mat4x4 view;
        view.rotateX((float)degreeToRad(player.rot.x()));
        view.rotateY((float)degreeToRad(player.rot.y()));
        //view.rotateZ((float)player.rot.z());
        view.translate((float)player.pos.x(), (float)player.pos.y(), (float)player.pos.z());
        return view;
    }
    
    void CameraController::update() {
        blt::vec3d move;
        if (Window::isKeyDown(GLFW_KEY_W)){
            move[2] = player.baseMovementSpeed;
        } else if (Window::isKeyDown(GLFW_KEY_S))
            move[2] = -player.baseMovementSpeed;
        else
            move[2] = 0;
        
        if (Window::isKeyDown(GLFW_KEY_E)){
            move[1] = -player.baseMovementSpeed;
        } else if (Window::isKeyDown(GLFW_KEY_Q))
            move[1] = player.baseMovementSpeed;
        else
            move[1] = 0;
        
        if (Window::isKeyDown(GLFW_KEY_A)){
            move[0] = -player.baseMovementSpeed;
        } else if (Window::isKeyDown(GLFW_KEY_D))
            move[0] = player.baseMovementSpeed;
        else
            move[0] = 0;
        
        auto yawRads = degreeToRad(player.rot.y());
        
        blt::vec3d dir;
        dir[0] = -move.z() * std::sin(yawRads) + -move.x() * std::cos(yawRads);
        dir[1] = move.y();
        dir[2] = move.z() * std::cos(yawRads) + -move.x() * std::sin(yawRads);
        
        player.pos = player.pos + dir * Window::getFrameDeltaSeconds();
        
        if (Window::mouseMovedLastFrame() && !Window::isMouseVisible()) {
            auto dYaw = Window::getMouseDX() * player.sensitivity * Window::getFrameDeltaSeconds();
            auto dPitch = Window::getMouseDY() * player.sensitivity * Window::getFrameDeltaSeconds();
            
            player.rot[1] += dYaw;
            player.rot[0] += dPitch;
        }
        
        const float sSpeed = 100;
        const float vertFact = 0.75;
        if (Window::isKeyDown(GLFW_KEY_RIGHT))
            player.rot[1] += sSpeed * Window::getFrameDeltaSeconds();
        if (Window::isKeyDown(GLFW_KEY_LEFT))
            player.rot[1] -= sSpeed * Window::getFrameDeltaSeconds();
        if (Window::isKeyDown(GLFW_KEY_UP))
            player.rot[0] -= sSpeed * Window::getFrameDeltaSeconds() * vertFact;
        if (Window::isKeyDown(GLFW_KEY_DOWN))
            player.rot[0] += sSpeed * Window::getFrameDeltaSeconds() * vertFact;
        
        if(player.rot[0] > 89.0f)
            player.rot[0] = 89.0f;
        if(player.rot[0] < -89.0f)
            player.rot[0] = -89.0f;
        
        if (player.rot[1] < 0)
            player.rot[1] = 360;
        if (player.rot[1] > 360)
            player.rot[1] = 0;
        
#ifdef BUILD_DEV_TOOLS
        ImGui::Begin("Player Stats");
        if (ImGui::CollapsingHeader("Player Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text(
                    "Player Pos (X/Y/Z): %f / %f / %f", player.pos.x(), player.pos.y(),
                    player.pos.z());
            ImGui::Text(
                    "Player Rot (Pitch/Yaw/Roll): %f / %f / %f", player.rot.x(), player.rot.y(),
                    player.rot.z());
        }
        if (ImGui::CollapsingHeader("Debug Info")) {
            ImGui::Text("(DEBUG) Dir (X/Y/Z): %f / %f / %f", dir.x(), dir.y(), dir.z());
            ImGui::Text("(DEBUG) Move (X/Y/Z): %f / %f / %f", move.x(), move.y(), move.z());
        }
        ImGui::End();
#endif
        
        Window::updateViewMatrix(generateViewMatrix());
    }
}