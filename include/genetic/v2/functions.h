//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_FUNCTIONS_H
#define PARKSNREC_FUNCTIONS_H

#include <memory>
#include <vector>
#include <genetic/util.h>
#include <genetic/v2/util.h>

namespace parks::genetic {
    
    namespace funcs {
        
        static Parameter add(const std::vector<Parameter>& params) {
            auto p1Type = params[0].getType();
            auto p2Type = params[1].getType();
            
            if (p2Type == ParameterType::IMAGE && p1Type != ParameterType::IMAGE)
                params[1].apply(std::plus(), params[0]);
            else
                params[0].apply(std::plus(), params[1]);
        }
        
        static Parameter sub(const std::vector<Parameter>& params) {
            auto p1Type = params[0].getType();
            auto p2Type = params[1].getType();
            
            if (p2Type == ParameterType::IMAGE && p1Type != ParameterType::IMAGE)
                params[1].apply(std::minus(), params[0]);
            else
                params[0].apply(std::minus(), params[1]);
        }
        
        static Parameter multiply(const std::vector<Parameter>& params) {
            auto p1Type = params[0].getType();
            auto p2Type = params[1].getType();
            
            if (p2Type == ParameterType::IMAGE && p1Type != ParameterType::IMAGE)
                params[1].apply(std::multiplies(), params[0]);
            else
                params[0].apply(std::multiplies(), params[1]);
        }
        
        static Parameter divide(const std::vector<Parameter>& params) {
            auto p1Type = params[0].getType();
            auto p2Type = params[1].getType();
            
            if (p2Type == ParameterType::IMAGE && p1Type != ParameterType::IMAGE)
                params[1].apply(std::divides(), params[0]);
            else
                params[0].apply(std::divides(), params[1]);
        }
        
    }
    
}

#endif //PARKSNREC_FUNCTIONS_H
