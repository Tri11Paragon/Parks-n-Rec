//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_GENETIC_H
#define PARKSNREC_GENETIC_H

#include <genetic/util.h>
#include <genetic/v2/functions.h>
#include <blt/math/vectors.h>
#include <memory>
#include <functional>

namespace parks::genetic {
    
    struct Function {
        int paramCount;
        std::vector<ParameterType> allowedParameterTypes;
        std::function<Parameter(const std::vector<Parameter>&)> func;
    };

}

#endif //PARKSNREC_GENETIC_H
