//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_FUNCTIONS_V3_H
#define PARKSNREC_FUNCTIONS_V3_H

#include <functional>
#include <utility>
#include <genetic/util.h>
#include <genetic/v3/arguments.h>
#include <unordered_set>

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
    

    Color randScalar(OperatorArguments args, const ParameterSet& params);
    Color randColor(OperatorArguments args, const ParameterSet& params);
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
         RAND_SCALAR, RAND_COLOR, ADD, SUBTRACT, MULTIPLY, DIVIDE, MOD, ROUND, MIN, MAX, ABS, LOG, SIN, COS, ATAN, NOISE, COLOR_NOISE
    };
    
    class Function {
        private:
            std::function<Color(OperatorArguments, ParameterSet)> func;
            unsigned int requiredScalars;
            unsigned int requiredColors;
            unsigned char acceptsArgs;
        public:
            const std::string name;
            Function(std::string name, std::function<Color(OperatorArguments, ParameterSet)> func, unsigned int requiredScalars, unsigned int requiredColors, unsigned int acceptsArgs): func(std::move(func)), requiredScalars(requiredScalars), requiredColors(requiredColors), acceptsArgs(acceptsArgs), name(std::move(name)) {
            }
            Function(const Function& f) = delete;
            Function& operator=(const Function& f) = delete;
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
                return bothArgument() || dontCareArgument() || singleArgument();
            }
            
            [[nodiscard]] inline bool disallowsArgument() const {
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
            
            [[nodiscard]] inline Color call(OperatorArguments args, const ParameterSet& set) const {
                return func(args, set);
            }
            
    };
    
    class FunctionStorage {
        private:
            std::unordered_set<FunctionID> functionsInStorage;
            Function** functions;
            size_t size = 0;
        public:
            FunctionStorage(std::initializer_list<std::pair<FunctionID, Function*>>&& init);
            
            [[nodiscard]] const Function& operator[](FunctionID id) const {
                return *functions[(int)id];
            }
            
            inline FunctionID select(){
                auto first = functionsInStorage.begin();
                std::advance(first, randomInt(0, (int)functionsInStorage.size()));
                return *first;
            }
            
            ~FunctionStorage(){
                for (size_t i = 0; i < size; i++)
                    delete functions[i];
                delete[] functions;
            }
    };
    
    inline FunctionStorage functions = {
            {FunctionID::RAND_SCALAR, new Function{"RS", parks::genetic::randScalar, 1, 0, ARGS_NONE}},
            {FunctionID::RAND_COLOR, new Function{"RC", parks::genetic::randColor, 0, 1, ARGS_NONE}},
            {FunctionID::ADD, new Function{"+", parks::genetic::add, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::SUBTRACT, new Function{"-", parks::genetic::subtract, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MULTIPLY, new Function{"*", parks::genetic::multiply, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::DIVIDE, new Function{"/", parks::genetic::divide, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MOD, new Function{"%", parks::genetic::mod, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ROUND, new Function{"ROUND", parks::genetic::round, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MIN, new Function{"MIN", parks::genetic::min, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::MAX, new Function{"MAX", parks::genetic::max, 0, 0, ARGS_BOTH | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ABS, new Function{"ABS", parks::genetic::abs, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::LOG, new Function{"LOG", parks::genetic::log, 0, 0, ARGS_SINGLE | ARGS_SCALARS | ARGS_COLORS | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::SIN, new Function{"SIN", parks::genetic::sin, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::COS, new Function{"COS", parks::genetic::cos, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::ATAN, new Function{"ATAN", parks::genetic::atan, 0, 0, ARGS_SINGLE | ARGS_VARIABLES | ARGS_FUNCS}},
            {FunctionID::NOISE, new Function{"Noise", parks::genetic::noise, 5, 0, ARGS_BOTH | ARGS_VARIABLES}},
            {FunctionID::COLOR_NOISE, new Function{"ColorNoise", parks::genetic::colorNoise, 5, 0, ARGS_BOTH | ARGS_VARIABLES}},
    };

}

#endif //PARKSNREC_FUNCTIONS_V3_H
