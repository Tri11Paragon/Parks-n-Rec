//
// Created by brett on 7/17/23.
//
#include <genetic/v3/v3_functions.h>

namespace parks::genetic {
    
    template<typename T>
    inline Color applyFunc(T op, Color left, Color right) {
        return Color(op(left.r, right.r), op(left.g, right.g), op(left.b, right.b));
    }
    
    Color add(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::plus(), args.left, args.right);
    }
    
    Color subtract(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::minus(), args.left, args.right);
    }
    
    Color multiply(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::multiplies(), args.left, args.right);
    }
    
    Color divide(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::divides(), args.left, args.right);
    }
    
    Color mod(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(floatMod(), args.left, args.right);
    }
    
    Color round(OperatorArguments args, const ParameterSet& params) {
        return Color(std::round(args.left.r), std::round(args.left.g), std::round(args.left.b));
    }
    
    Color min(OperatorArguments args, const ParameterSet& params) {
        return Color(std::min(args.left.r, args.right.r), std::min(args.left.g, args.right.g), std::min(args.left.b, args.right.b));
    }
    
    Color max(OperatorArguments args, const ParameterSet& params) {
        return Color(std::max(args.left.r, args.right.r), std::max(args.left.g, args.right.g), std::max(args.left.b, args.right.b));
    }
    
    Color abs(OperatorArguments args, const ParameterSet& params) {
        return Color(std::abs(args.left.r), std::abs(args.left.g), std::abs(args.left.b));
    }
    
    Color log(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
    
    Color sin(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
    
    Color cos(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
    
    Color atan(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
    
    Color noise(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
    
    Color colorNoise(OperatorArguments args, const ParameterSet& params) {
        return Color(0, 0, 0);
    }
}