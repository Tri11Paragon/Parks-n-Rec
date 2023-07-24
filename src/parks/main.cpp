#include <iostream>

#include <parks/window.h>
#include <blt/std/logging.h>
#include "parks/renderer/engine.h"
#include <genetic/util.h>
#include <blt/profiling/profiler.h>

using namespace parks;

enum class test : int {
        dush,
        mush,
        push,
        fund,
        mund,
        pund,
        cunt,
        euur,
        sexy,
        fucks,
};

int dushf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int mushf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int pushf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int fundf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int mundf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int pundf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int cuntf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int eurrf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int sexyf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}
int fucksf(int i){
    int acc = 1;
    for (int p = 0; p < i; p++)
        acc += p * i;
    return acc;
}

int (*funcs[10])(int);

int main(){
    
    funcs[0] = &dushf;
    funcs[1] = &mushf;
    funcs[2] = &pushf;
    funcs[3] = &fundf;
    funcs[4] = &mundf;
    funcs[5] = &pundf;
    funcs[6] = &cuntf;
    funcs[7] = &eurrf;
    funcs[8] = &sexyf;
    funcs[9] = &fucksf;
    
    int numsToRun = 100000;
    
    BLT_START_INTERVAL("Funcs", "Index");
    int64_t acc1 = 0;
    for (int i = 0; i < numsToRun; i++){
        int index = i % 10;
        acc1 += funcs[index](i);
    }
    BLT_END_INTERVAL("Funcs", "Index");
    BLT_TRACE("%Ld", acc1);
    
    BLT_START_INTERVAL("Funcs", "Switch");
    int64_t acc2 = 0;
    for (int i = 0; i < numsToRun; i++){
        int index = i % 10;
        switch ((test)index){
            case test::dush:
                acc2 += dushf(i);
                break;
            case test::mush:
                acc2 += mushf(i);
                break;
            case test::push:
                acc2 += pushf(i);
                break;
            case test::fund:
                acc2 += fundf(i);
                break;
            case test::mund:
                acc2 += mundf(i);
                break;
            case test::pund:
                acc2 += pundf(i);
                break;
            case test::cunt:
                acc2 += cuntf(i);
                break;
            case test::euur:
                acc2 += eurrf(i);
                break;
            case test::sexy:
                acc2 += sexyf(i);
                break;
            case test::fucks:
                acc2 += fucksf(i);
                break;
        }
    }
    BLT_END_INTERVAL("Funcs", "Switch");
    BLT_TRACE("%Ld", acc2);
    
    BLT_PRINT_PROFILE("Funcs");
    return 0;
    
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