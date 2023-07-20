//
// Created by brett on 7/17/23.
//
#include <genetic/v3/functions_v3.h>
#include <stb/stb_perlin.h>

namespace parks::genetic {
    
    template<typename T>
    inline Color applyFunc(T op, Color left, Color right) {
        return Color(op(left.r, right.r), op(left.g, right.g), op(left.b, right.b));
    }
    
    Color add(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::plus(), args.left, args.right);
    }
    
    Color subtract(OperatorArguments args, const ParameterSet& params) {
        return applyFunc(std::minus(), args.left, args.right);;
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
        return Color(std::log(args.left.r), std::log(args.left.g), std::log(args.left.b));
    }
    
    Color sin(OperatorArguments args, const ParameterSet& params) {
        return Color(std::sin(args.left.r), std::sin(args.left.g), std::sin(args.left.b));
    }
    
    Color cos(OperatorArguments args, const ParameterSet& params) {
        return Color(std::cos(args.left.r), std::cos(args.left.g), std::cos(args.left.b));
    }
    
    Color atan(OperatorArguments args, const ParameterSet& params) {
        return Color(std::atan(args.left.r), std::atan(args.left.g), std::atan(args.left.b));
    }
    
    const float lacunarity = 6;
    const float octaves = 8;
    const float gain = 2;
    const float scale = 128;
    
    Color noise(OperatorArguments args, const ParameterSet& params) {
        float scaleX = (float)args.left.r * (float)params[3].r * scale;
        float scaleY = (float)args.right.r * (float)params[3].r * scale;
        
        return Color(stb_perlin_turbulence_noise3(scaleX, scaleY, 0.52342, (float)params[0].r * lacunarity, (float)params[1].r * gain, (int)std::max(2.0, params[2].r * octaves)));
    }
    
    Color colorNoise(OperatorArguments args, const ParameterSet& params) {
        float scaleX = (float)args.left.r * (float)params[3].r * scale;
        float scaleY = (float)args.right.r * (float)params[4].r * scale;
        
        float r = stb_perlin_turbulence_noise3(scaleX, scaleY, 0.52342, (float)params[0].r * lacunarity, (float)params[1].r * gain, (int)std::max(2.0, params[2].r * octaves));
        float g = stb_perlin_turbulence_noise3(scaleX, 0.21045, scaleY, (float)params[0].r * lacunarity, (float)params[1].r * gain, (int)std::max(2.0, params[2].r * octaves));
        float b = stb_perlin_turbulence_noise3(0.78423, scaleY, scaleX, (float)params[0].r * lacunarity, (float)params[1].r * gain, (int)std::max(2.0, params[2].r * octaves));
        
        return Color(r, g, b);
    }
    
    Color randScalar(OperatorArguments args, const ParameterSet& params) {
        return params[0];
    }
    
    Color randColor(OperatorArguments args, const ParameterSet& params) {
        return params[0];
    }
    
    FunctionStorage::FunctionStorage(
            std::initializer_list<std::pair<FunctionID, Function*>>&& init
    ) {
        size_t max_value = init.size();
        for (const auto& v : init){
            int enumID = (int)v.first;
            max_value = std::max(max_value, (size_t)enumID);
        }
        functions = new Function*[max_value];
        
        for (auto& v : init){
            functions[(int)v.first] = v.second;
            functionsInStorage.insert(v.first);
        }
        size = max_value;
    }
}