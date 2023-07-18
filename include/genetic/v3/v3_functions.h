//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_V3_FUNCTIONS_H
#define PARKSNREC_V3_FUNCTIONS_H

#include <functional>
#include <utility>
#include <genetic/util.h>
#include <genetic/v3/arguments.h>

namespace parks::genetic {
    
    inline const ParameterSet EMPTY_SET;
    
    constexpr unsigned char ARGS_SINGLE = 0b00000001;
    constexpr unsigned char ARGS_BOTH = 0b00000010;
    constexpr unsigned char ARGS_DONT_CARE = 0b01000000;
    constexpr unsigned char ARGS_NONE = 0b10000000;
    
    constexpr unsigned char ARGS_VARIABLES = 0b00000100;
    constexpr unsigned char ARGS_SCALARS = 0b00001000;
    constexpr unsigned char ARGS_COLORS = 0b00010000;
    constexpr unsigned char ARGS_FUNCS = 0b00100000;
    

    Color add(OperatorArguments args, const ParameterSet& params);
    Color subtract(OperatorArguments args, const ParameterSet& params);
    Color multiply(OperatorArguments args, const ParameterSet& params);
    Color divide(OperatorArguments args, const ParameterSet& params);
    Color mod(OperatorArguments args, const ParameterSet& params);
    Color round(OperatorArguments args, const ParameterSet& params);
    Color min(OperatorArguments args, const ParameterSet& params);
    Color max(OperatorArguments args, const ParameterSet& params);
    Color abs(OperatorArguments args, const ParameterSet& params);
    Color log(OperatorArguments args, const ParameterSet& params);
    Color sin(OperatorArguments args, const ParameterSet& params);
    Color cos(OperatorArguments args, const ParameterSet& params);
    Color atan(OperatorArguments args, const ParameterSet& params);
    Color noise(OperatorArguments args, const ParameterSet& params);
    Color colorNoise(OperatorArguments args, const ParameterSet& params);
    
    enum class FunctionID {
         ADD, SUBTRACT, MULTIPLY, DIVIDE, MOD, ROUND, MIN, MAX, ABS, LOG, SIN, COS, ATAN, NOISE, COLOR_NOISE
    };
    
    class Function {
        private:
            std::function<Color(OperatorArguments, ParameterSet)> func;
            unsigned int requiredScalars;
            unsigned int requiredColors;
            unsigned char acceptsArgs;
        public:
            Function(std::function<Color(OperatorArguments, ParameterSet)> func, unsigned int requiredScalars, unsigned int requiredColors, unsigned int acceptsArgs): func(std::move(func)), requiredScalars(requiredScalars), requiredColors(requiredColors), acceptsArgs(acceptsArgs) {
            }
            
            // in the case of single argument, it is provided to the left side!
            [[nodiscard]] inline bool singleArgument() const {
                return acceptsArgs & ARGS_SINGLE;
            }
            
            [[nodiscard]] inline bool bothArgument() const {
                return acceptsArgs & ARGS_BOTH;
            }
            
            [[nodiscard]] inline bool dontCareArgument() const {
                return acceptsArgs & ARGS_DONT_CARE;
            }
            
            [[nodiscard]] inline bool allowsArgument() const {
                return acceptsArgs & ARGS_NONE;
            }
            
            [[nodiscard]] inline bool allowedVariables() const {
                return acceptsArgs & ARGS_VARIABLES;
            }
            
            [[nodiscard]] inline bool allowedScalars() const {
                return acceptsArgs & ARGS_SCALARS;
            }
            
            [[nodiscard]] inline bool allowedColors() const {
                return acceptsArgs & ARGS_COLORS;
            }
            
            [[nodiscard]] inline bool allowedFuncs() const {
                return acceptsArgs & ARGS_FUNCS;
            }
            
            [[nodiscard]] ParameterSet generateRandomParameters() const {
                ParameterSet set;
                for (unsigned int i = 0; i < requiredScalars; i++)
                    set.add(RandomScalar::get());
                for (unsigned int i = 0; i < requiredColors; i++)
                    set.add(RandomColor::get());
                return set;
            }
            
            inline Color call(OperatorArguments args, const ParameterSet& set){
                return func(args, set);
            }
            
    };
    
    std::unordered_map<FunctionID, Function> functions = {
            {FunctionID::ADD, Function{parks::genetic::add, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::SUBTRACT, Function{parks::genetic::subtract, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MULTIPLY, Function{parks::genetic::multiply, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::DIVIDE, Function{parks::genetic::divide, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MOD, Function{parks::genetic::mod, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ROUND, Function{parks::genetic::round, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MIN, Function{parks::genetic::min, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MAX, Function{parks::genetic::max, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ABS, Function{parks::genetic::abs, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::LOG, Function{parks::genetic::log, 0, 0, ARGS_SINGLE | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::SIN, Function{parks::genetic::sin, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::COS, Function{parks::genetic::cos, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ATAN, Function{parks::genetic::atan, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::NOISE, Function{parks::genetic::noise, 3, 0, ARGS_NONE}},
            {FunctionID::COLOR_NOISE, Function{parks::genetic::colorNoise, 3, 0, ARGS_NONE}},
    };

}

#endif //PARKSNREC_V3_FUNCTIONS_H
