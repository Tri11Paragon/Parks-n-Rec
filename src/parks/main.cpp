#include <iostream>

#include <parks/window.h>
#include <blt/std/logging.h>
#include "parks/renderer/engine.h"

using namespace parks;

int main(){

    Settings settings;
    settings.setProperty(Properties::WINDOW_WIDTH, new Properties::Value<int>(1440));
    settings.setProperty(Properties::WINDOW_HEIGHT, new Properties::Value<int>(720));
    settings.setProperty(Properties::WINDOW_RESIZABLE, new Properties::Value<bool>(true));
    settings.setProperty(Properties::WINDOW_TITLE, new Properties::Value<std::string>("So You Think You Can Park?"));
    
    // TODO: poll hardware
    resources::init(16);
    // TODO local
    resources::loadTexture("/home/brett/git/parksandrec/resources/textures/test.png", "test.png");

    Window::create(settings);
    
    resources::beginLoading();

    parks::Engine gameEngine(settings);
    //Window::setMouseVisible(false);
    gameEngine.run();
    
    resources::cleanup();

    BLT_INFO("Engine has stopped running. Have a nice day!");

}