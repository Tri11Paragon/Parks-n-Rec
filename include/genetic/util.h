//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_UTIL_H
#define PARKSNREC_UTIL_H

#include <random>
#include <blt/std/time.h>
#include <variant>
#include "blt/math/vectors.h"
#include "blt/std/logging.h"

namespace parks {
    
    constexpr unsigned int WIDTH = 512;
    constexpr unsigned int HEIGHT = 512;
    constexpr unsigned int CHANNELS = 3;
    
    inline static double randomDouble(double min, double max) {
        std::mt19937 rng(blt::system::getCurrentTimeNanoseconds());
        static std::uniform_real_distribution<double> gen(0, 1);
        
        return gen(rng) * (max - min) + min;
    }
    
    inline static int randomInt(int min, int max) {
        return (int)randomDouble(min, max);
    }
    
    inline static bool chance(double bound = 0.5){
        return randomDouble(0, 1) < bound;
    }
}

#endif //PARKSNREC_UTIL_H
