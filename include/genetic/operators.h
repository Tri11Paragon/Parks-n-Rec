//
// Created by brett on 7/11/23.
//

#ifndef PARKSNREC_OPERATORS_H
#define PARKSNREC_OPERATORS_H

#include <blt/std/random.h>
#include <blt/std/time.h>
#include <cmath>
#include <stb/stb_perlin.h>
#include <random>
#include <utility>

namespace parks::genetic {
    
    constexpr long seed = 213434;
    
    double randomDouble(double min, double max) {
        std::mt19937 rng(blt::system::getCurrentTimeNanoseconds());
        static std::uniform_real_distribution<double> gen(0, 1);
        
        return gen(rng) * (max - min) + min;
    }
    
    int randomInt(int min, int max) {
        return (int)randomDouble(min, max);
    }
    
    struct Color {
        Color(double r, double g, double b): r(r), g(g), b(b) {}
        explicit Color(double v): Color(v,v,v){}
        double r, g, b;
        
        [[nodiscard]] double v() const {
            return std::sqrt(r * r + g * g + b * b);
        }
    };
    
    struct OperatorArguments {
        double x, y;
        long long time;
        int arguments;
        Color left, right;
    };
    
    class Operator {
        public:
            [[nodiscard]] inline virtual Color apply(const OperatorArguments& args) const = 0;
            virtual ~Operator() = default;
    };
    
    class ZeroOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(0);
            }
    };
    
    class OneOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(1);
            }
    };
    
    class XOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(args.x);
            }
    };
    
    class YOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(args.y);
            }
    };
    
    class MultiplicationOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return right;
                    case 2:
                        return left;
                    case 3:
                        return {left.r * right.r, left.g * right.g, left.b * right.b};
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class AdditionOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return right;
                    case 2:
                        return left;
                    case 3:
                        return {left.r + right.r, left.g + right.g, left.b + right.b};
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class SubtractionOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return Color{-right.r, -right.g, -right.b};
                    case 2:
                        return Color{-left.r, -left.g, -left.b};
                    case 3:
                        return {left.r - right.r, left.g - right.g, left.b - right.b};
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class ModOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return right;
                    case 2:
                        return left;
                    case 3:
                        return {(double)((long)left.r % (long)std::max(1.0, right.r)),
                                (double)((long)left.g % (long)std::max(1.0, right.g)),
                                (double)((long)left.b % (long)std::max(1.0, right.b))};
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class MinOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                return {std::min(left.r, right.r), std::min(left.g, right.g), std::min(left.b, right.b)};
            }
    };
    
    class MaxOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                return {std::max(left.r, right.r), std::max(left.g, right.g), std::max(left.b, right.b)};
            }
    };
    
    class LogOperator : public Operator {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return {std::log(right.r), std::log(right.g), std::log(right.b)};
                    case 2:
                        return {std::log(left.r), std::log(left.g), std::log(left.b)};
                    case 3:
                        return {std::log(left.r) + std::log(right.r), std::log(left.g) + std::log(right.g), std::log(left.b) + std::log(right.b)};
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    constexpr double MIN = 0;
    constexpr double MAX = 1;
    constexpr double SCALE_MAX = 2048;
    
    class PerlinBWOperator : public Operator {
        private:
            double _y, _z;
            float scale = 256.523;
        public:
            PerlinBWOperator() {
                do {
                    _y = randomDouble(MIN, MAX);
                } while (trunc(_y) == _y);
                do {
                    _z = randomDouble(MIN, MAX);
                } while (trunc(_z) == _z);
                do {
                    scale = (float)randomDouble(MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
            }
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{stb_perlin_noise3((float)args.time / scale, (float)_y, (float)_z, 0, 0, 0)};
                    case 1:
                        return Color{
                                stb_perlin_noise3((float)right.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                        };
                    case 2:
                        return Color{
                                stb_perlin_noise3((float)left.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                        };
                    case 3:
                        return Color{
                                stb_perlin_noise3((float)left.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                                + stb_perlin_noise3((float)right.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                        };
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class PerlinColorOperator : public Operator {
        private:
            double uniques[12]{};
            float scale = 1;
        public:
            PerlinColorOperator() {
                for (double& unique : uniques) {
                    do {
                        unique = randomDouble(MIN, MAX);
                    } while (trunc(unique) == unique);
                }
                scale = (float)randomDouble(1, SCALE_MAX);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return {stb_perlin_noise3((float)args.time / scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)(float)args.time / scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)(float)args.time / scale, 0, 0, 0)};
                    case 1:
                        return {
                                stb_perlin_noise3((float)right.r / scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)right.g / scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)right.b / scale, 0, 0, 0)
                        };
                    case 2:
                        return {
                                stb_perlin_noise3((float)left.r / scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)left.g / scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)left.b / scale, 0, 0, 0)
                        };
                    case 3:
                        return {
                                stb_perlin_noise3((float)left.r / scale, (float)uniques[0], (float)uniques[1], 0, 0, 0)
                                + stb_perlin_noise3((float)right.r / scale, (float)uniques[2], (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)left.g / scale, (float)uniques[5], 0, 0, 0)
                                + stb_perlin_noise3((float)uniques[6], (float)right.g / scale, (float)uniques[7], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[8], (float)uniques[9], (float)left.b / scale, 0, 0, 0)
                                + stb_perlin_noise3((float)uniques[10], (float)uniques[11], (float)right.b / scale, 0, 0, 0)
                        };
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class PerlinRidgeOperator : public Operator {
        private:
            double _y, _z;
            int octaves;
            float lacunarity = 2.0;
            float gain = 0.5;
            float offset = 1.0;
            float scale = 256.523;
        public:
            PerlinRidgeOperator() {
                do {
                    _y = randomDouble(MIN, MAX);
                } while (trunc(_y) == _y);
                do {
                    _z = randomDouble(MIN, MAX);
                } while (trunc(_z) == _z);
                do {
                    scale = (float)randomDouble(MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return {
                                stb_perlin_ridge_noise3((float)args.time / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)_y, (float)(float)args.time / scale, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)_z, (float)_y, (float)(float)args.time / scale, lacunarity, gain, offset, octaves)
                        };
                    case 1:
                        return {
                                stb_perlin_ridge_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        };
                    case 2:
                        return {
                                stb_perlin_ridge_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        };
                    case 3:
                        return {
                                stb_perlin_ridge_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        };
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class PerlinFBMOperator : public Operator {
        private:
            double _y, _z;
            int octaves;
            float lacunarity = 2.0;
            float gain = 0.5;
            float scale = 256.523;
        public:
            PerlinFBMOperator() {
                do {
                    _y = randomDouble(MIN, MAX);
                } while (trunc(_y) == _y);
                do {
                    _z = randomDouble(MIN, MAX);
                } while (trunc(_z) == _z);
                do {
                    scale = (float)randomDouble(MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return {
                                stb_perlin_fbm_noise3((float)args.time / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)_y, (float)(float)args.time / scale, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)_z, (float)_y, (float)(float)args.time / scale, lacunarity, gain, octaves)
                        };
                    case 1:
                        return {
                                stb_perlin_fbm_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                    case 2:
                        return {
                                stb_perlin_fbm_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                    case 3:
                        return {
                                stb_perlin_fbm_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class PerlinTurbulenceOperator : public Operator {
        private:
            double _y, _z;
            int octaves;
            float lacunarity = 2.0;
            float gain = 0.5;
            float scale = 256.523;
        public:
            PerlinTurbulenceOperator() {
                do {
                    _y = randomDouble(MIN, MAX);
                } while (trunc(_y) == _y);
                do {
                    _z = randomDouble(MIN, MAX);
                } while (trunc(_z) == _z);
                do {
                    scale = (float)randomDouble(MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return {
                                stb_perlin_turbulence_noise3((float)args.time / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)_y, (float)(float)args.time / scale, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)_z, (float)_y, (float)(float)args.time / scale, lacunarity, gain, octaves)
                        };
                    case 1:
                        return {
                                stb_perlin_turbulence_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                    case 2:
                        return {
                                stb_perlin_turbulence_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                    case 3:
                        return {
                                stb_perlin_turbulence_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        };
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class ColorNoiseOperator : public Operator {
        private:
            double scale;
        public:
            ColorNoiseOperator() {
                do {
                    scale = (float)randomDouble(1, 255);
                } while (trunc((double)scale) == (double)scale);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return {randomDouble(0, scale), randomDouble(0, scale), randomDouble(0, scale)};
            }
    };
    
    enum class Operators {
//        Zero,
//        One,
        X,
        Y,
        Multiplication,
        Addition,
        Subtraction,
        Modulo,
        Min,
        Max,
        Log,
        PerlinBW,
        PerlinColor,
        PerlinRidge,
        PerlinFBM,
        PerlinTurbulence,
        ColorNoise
    };
    
    struct OperatorProperties {
        Operators index;
        std::string opCode;
        int acceptsInput; // 0000 00lr (bit mask) accepts l -> left subtree; r -> right subtree
    };
    
    const inline OperatorProperties operatorInfo[] = {
//            {Operators::Zero, "0", 0},
//            {Operators::One, "1", 0},
            {Operators::X, "X", 0},
            {Operators::Y, "Y", 0},
            {Operators::Multiplication, "*", 3},
            {Operators::Addition, "+", 3},
            {Operators::Subtraction, "-", 3},
            {Operators::Modulo, "%", 3},
            {Operators::Min, "Min", 3},
            {Operators::Max, "Max", 3},
            {Operators::Log, "Log", 3},
            {Operators::PerlinBW, "PerlinBW", 3},
            {Operators::PerlinColor, "PerlinColor", 3},
            {Operators::PerlinRidge, "PerlinRidge", 3},
            {Operators::PerlinFBM, "PerlinFBM", 3},
            {Operators::PerlinTurbulence, "PerlinTurbulence", 3},
            {Operators::ColorNoise, "ColorNoise", 0},
    };
}

#endif //PARKSNREC_OPERATORS_H
