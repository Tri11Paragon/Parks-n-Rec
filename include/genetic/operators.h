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
    
    class Operator {
        public:
            const std::string opString;
            explicit Operator(std::string  opString): opString(std::move(opString)) {};
            [[nodiscard]] inline virtual Color apply(double x, double y, unsigned int time, Color left, Color right) const = 0;
            virtual ~Operator() = default;
    };
    
    class ZeroOperator : public Operator {
        public:
            ZeroOperator(): Operator("0"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return Color(0);
            }
    };
    
    class OneOperator : public Operator {
        public:
            OneOperator(): Operator("1"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return Color(1);
            }
    };
    
    class XOperator : public Operator {
        public:
            XOperator(): Operator("X"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return Color(x);
            }
    };
    
    class YOperator : public Operator {
        public:
            YOperator(): Operator("Y"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return Color(y);
            }
    };
    
    class MultiplicationOperator : public Operator {
        public:
            MultiplicationOperator(): Operator("*"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {left.r * right.r, left.g * right.g, left.b * right.b};
            }
    };
    
    class AdditionOperator : public Operator {
        public:
            AdditionOperator(): Operator("+"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {left.r + right.r, left.g + right.g, left.b + right.b};
            }
    };
    
    class SubtractionOperator : public Operator {
        public:
            SubtractionOperator(): Operator("-"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {left.r - right.r, left.g - right.g, left.b - right.b};
            }
    };
    
    class ModOperator : public Operator {
        public:
            ModOperator(): Operator("%"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {(double)((long)left.r % (long)std::max(1.0, right.r)),
                        (double)((long)left.g % (long)std::max(1.0, right.g)),
                        (double)((long)left.b % (long)std::max(1.0, right.b))};
            }
    };
    
    class MinOperator : public Operator {
        public:
            MinOperator(): Operator("Min"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {std::min(left.r, right.r), std::min(left.g, right.g), std::min(left.b, right.b)};
            }
    };
    
    class MaxOperator : public Operator {
        public:
            MaxOperator(): Operator("Max"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {std::max(left.r, right.r), std::max(left.g, right.g), std::max(left.b, right.b)};
            }
    };
    
    class LogOperator : public Operator {
        public:
            LogOperator(): Operator("Log"){}
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {std::log(left.r) + std::log(right.r), std::log(left.g) + std::log(right.g), std::log(left.b) + std::log(right.b)};
            }
    };
    
    class PerlinBWOperator : public Operator {
        private:
            double _y, _z;
            float scale = 256.523;
        public:
            PerlinBWOperator(): Operator("PerlinBW"){
                constexpr double MIN = 0;
                constexpr double MAX = 8192;
                constexpr double SCALE_MAX = 256;
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
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return Color{
                        stb_perlin_noise3((float)left.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                        + stb_perlin_noise3((float)right.v() / scale, (float)_y, (float)_z, 0, 0, 0)
                };
            }
    };
    
    class PerlinColorOperator : public Operator {
        private:
            double _y, _z;
            float scale = 256.523;
        public:
            PerlinColorOperator(): Operator("PerlinColor"){
                constexpr double MIN = 0;
                constexpr double MAX = 8192;
                constexpr double SCALE_MAX = 256;
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
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {
                        stb_perlin_noise3((float)left.r / scale, (float)_y, (float)_z, 0, 0, 0)
                        + stb_perlin_noise3((float)right.r / scale, (float)_y, (float)_z, 0, 0, 0),
                        stb_perlin_noise3((float)left.g / scale, (float)_y, (float)_z, 0, 0, 0)
                        + stb_perlin_noise3((float)right.g / scale, (float)_y, (float)_z, 0, 0, 0),
                        stb_perlin_noise3((float)left.b / scale, (float)_y, (float)_z, 0, 0, 0)
                        + stb_perlin_noise3((float)right.b / scale, (float)_y, (float)_z, 0, 0, 0)
                };
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
            PerlinRidgeOperator(): Operator("PerlinRidge"){
                constexpr double MIN = 0;
                constexpr double MAX = 8192;
                constexpr double SCALE_MAX = 256;
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
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {
                        stb_perlin_ridge_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        + stb_perlin_ridge_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                        stb_perlin_ridge_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        + stb_perlin_ridge_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                        stb_perlin_ridge_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        + stb_perlin_ridge_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                };
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
            PerlinFBMOperator(): Operator("PerlinFBM"){
                constexpr double MIN = 0;
                constexpr double MAX = 8192;
                constexpr double SCALE_MAX = 256;
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
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {
                        stb_perlin_fbm_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_fbm_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                        stb_perlin_fbm_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_fbm_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                        stb_perlin_fbm_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_fbm_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                };
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
            PerlinTurbulenceOperator(): Operator("PerlinTurb"){
                constexpr double MIN = 0;
                constexpr double MAX = 8192;
                constexpr double SCALE_MAX = 256;
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
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {
                        stb_perlin_turbulence_noise3((float)left.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_turbulence_noise3((float)right.r / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                        stb_perlin_turbulence_noise3((float)left.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_turbulence_noise3((float)right.g / scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                        stb_perlin_turbulence_noise3((float)left.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        + stb_perlin_turbulence_noise3((float)right.b / scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                };
            }
    };
    
    class ColorNoiseOperator : public Operator {
        private:
            double scale;
        public:
            ColorNoiseOperator(): Operator("ColorNoise"){
                do {
                    scale = (float)randomDouble(1, 255);
                } while (trunc((double)scale) == (double)scale);
            }
            [[nodiscard]] inline Color apply(double x, double y, unsigned int time, Color left, Color right) const final {
                return {randomDouble(0, scale), randomDouble(0, scale), randomDouble(0, scale)};
            }
    };
}

#endif //PARKSNREC_OPERATORS_H
