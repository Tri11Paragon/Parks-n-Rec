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
    
    bool chance(int bound = 2){
        return randomInt(0, 100) <= bound;
    }
    
    struct Color {
        Color(double r, double g, double b): r(r), g(g), b(b) {}
        explicit Color(double v): Color(v,v,v){}
        double r, g, b;
        
        [[nodiscard]] double v() const {
            return std::sqrt(r * r + g * g + b * b);
        }
        
        Color& normalize(){
            return *this;
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
            // returns modified
            inline virtual Operator* mutate() = 0;
            // returns new
            [[nodiscard]] inline virtual Operator* breed(Operator* other) const = 0;
            virtual ~Operator() = default;
    };
    
    template<typename T>
    class OperatorBase : public Operator {
        protected:
            inline static double lerp(double x, double y) {
                return (x + y) / 2;
            }
        public:
            [[nodiscard]] inline T* create() const {
                return new T;
            }
            
            [[nodiscard]] inline Operator* mutate() override {
                return this;
            }
            
            [[nodiscard]] inline Operator* breed(Operator* other) const override {
                return create();
            }
            
            [[nodiscard]] inline T* cast(Operator* op) const {
                return dynamic_cast<T*>(op);
            }
    };
    
    class RandomScalarOperator : public OperatorBase<RandomScalarOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(randomDouble(0, 1));
            }
    };
    
    class RandomColorOperator : public OperatorBase<RandomColorOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color{randomDouble(0, 1),randomDouble(0, 1),randomDouble(0, 1)}.normalize();
            }
    };
    
    class XOperator : public OperatorBase<XOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(args.x);
            }
    };
    
    class YOperator : public OperatorBase<YOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color(args.y);
            }
    };
    
    class MultiplicationOperator : public OperatorBase<MultiplicationOperator>{
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return right.normalize();
                    case 2:
                        return left.normalize();
                    case 3:
                        return Color{left.r * right.r, left.g * right.g, left.b * right.b}.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class AdditionOperator : public OperatorBase<AdditionOperator> {
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
                        return Color{left.r + right.r, left.g + right.g, left.b + right.b}.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class SubtractionOperator : public OperatorBase<SubtractionOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return Color{-right.r, -right.g, -right.b}.normalize();
                    case 2:
                        return Color{-left.r, -left.g, -left.b}.normalize();
                    case 3:
                        return Color{left.r - right.r, left.g - right.g, left.b - right.b}.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class ModOperator : public OperatorBase<ModOperator> {
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
                        return Color{(double)((long)left.r % (long)std::max(1.0, right.r)),
                                (double)((long)left.g % (long)std::max(1.0, right.g)),
                                (double)((long)left.b % (long)std::max(1.0, right.b))}.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    class MinOperator : public OperatorBase<MinOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                return Color{std::min(left.r, right.r), std::min(left.g, right.g), std::min(left.b, right.b)}.normalize();
            }
    };
    
    class MaxOperator : public OperatorBase<MaxOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                return Color{std::max(left.r, right.r), std::max(left.g, right.g), std::max(left.b, right.b)}.normalize();
            }
    };
    
    class LogOperator : public OperatorBase<LogOperator> {
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{0};
                    case 1:
                        return Color{std::log(right.r), std::log(right.g), std::log(right.b)}.normalize();
                    case 2:
                        return Color{std::log(left.r), std::log(left.g), std::log(left.b)}.normalize();
                    case 3:
                        return Color{std::log(left.r) + std::log(right.r), std::log(left.g) + std::log(right.g), std::log(left.b) + std::log(right.b)}.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
    };
    
    constexpr double MIN = 0;
    constexpr double SCALE_MIN = 1;
    constexpr double MAX = 1;
    constexpr double SCALE_MAX = 2048;
#define SCALE_OP /
    
    class PerlinBWOperator : public OperatorBase<PerlinBWOperator> {
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
                    scale = (float)randomDouble(SCALE_MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
            }
        public:
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{stb_perlin_noise3((float)args.time SCALE_OP scale, (float)_y, (float)_z, 0, 0, 0)};
                    case 1:
                        return Color{
                                stb_perlin_noise3((float)right.v() SCALE_OP scale, (float)_y, (float)_z, 0, 0, 0)
                        }.normalize();
                    case 2:
                        return Color{
                                stb_perlin_noise3((float)left.v() SCALE_OP scale, (float)_y, (float)_z, 0, 0, 0)
                        }.normalize();
                    case 3:
                        return Color{
                                stb_perlin_noise3((float)left.v() SCALE_OP scale, (float)_y, (float)_z, 0, 0, 0)
                                + stb_perlin_noise3((float)right.v() SCALE_OP scale, (float)_y, (float)_z, 0, 0, 0)
                        }.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
            [[nodiscard]] inline Operator* mutate() final {
                if (chance())
                    _y += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    _z += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    scale += (float)randomDouble(-SCALE_MAX, SCALE_MAX) / 2;
                return this;
            }
            
            [[nodiscard]] inline Operator* breed(Operator* other) const final {
                auto parent = cast(other);
                auto perlin = create();
                perlin->_y = lerp(parent->_y, _y);
                perlin->_z = lerp(parent->_z, _z);
                perlin->scale = (float)lerp(parent->scale, scale);
                return perlin;
            }
    };
    
    class PerlinColorOperator : public OperatorBase<PerlinColorOperator> {
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
                scale = (float)randomDouble(SCALE_MIN, SCALE_MAX);
                BLT_TRACE("Scale: %f", scale);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{stb_perlin_noise3((float)args.time SCALE_OP scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)(float)args.time SCALE_OP scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)(float)args.time SCALE_OP scale, 0, 0, 0)}.normalize();
                    case 1:
                        return Color{
                                stb_perlin_noise3((float)right.r SCALE_OP scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)right.g SCALE_OP scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)right.b SCALE_OP scale, 0, 0, 0)
                        }.normalize();
                    case 2:
                        return Color{
                                stb_perlin_noise3((float)left.r SCALE_OP scale, (float)uniques[0], (float)uniques[1], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[2], (float)left.g SCALE_OP scale, (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)uniques[5], (float)left.b SCALE_OP scale, 0, 0, 0)
                        }.normalize();
                    case 3:
                        return Color{
                                stb_perlin_noise3((float)left.r SCALE_OP scale, (float)uniques[0], (float)uniques[1], 0, 0, 0)
                                + stb_perlin_noise3((float)right.r SCALE_OP scale, (float)uniques[2], (float)uniques[3], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[4], (float)left.g SCALE_OP scale, (float)uniques[5], 0, 0, 0)
                                + stb_perlin_noise3((float)uniques[6], (float)right.g SCALE_OP scale, (float)uniques[7], 0, 0, 0),
                                stb_perlin_noise3((float)uniques[8], (float)uniques[9], (float)left.b SCALE_OP scale, 0, 0, 0)
                                + stb_perlin_noise3((float)uniques[10], (float)uniques[11], (float)right.b SCALE_OP scale, 0, 0, 0)
                        }.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
            [[nodiscard]] inline Operator* breed(Operator* other) const final {
                auto parent = cast(other);
                auto perlin = create();
                for (int i = 0; i < 12; i++)
                    perlin->uniques[i] = lerp(parent->uniques[i], uniques[i]);
                perlin->scale = (float)lerp(parent->scale, scale);
                return perlin;
            }
            [[nodiscard]] inline Operator* mutate() final {
                for (auto& u : uniques)
                    if (chance())
                        u += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    scale += (float)randomDouble(-SCALE_MAX, SCALE_MAX) / 2;
                return this;
            }
    };
    
    class PerlinRidgeOperator : public OperatorBase<PerlinRidgeOperator> {
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
                    scale = (float)randomDouble(SCALE_MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{
                                stb_perlin_ridge_noise3((float)args.time SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)_y, (float)(float)args.time SCALE_OP scale, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)_z, (float)_y, (float)(float)args.time SCALE_OP scale, lacunarity, gain, offset, octaves)
                        }.normalize();
                    case 1:
                        return Color{
                                stb_perlin_ridge_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        }.normalize();
                    case 2:
                        return Color{
                                stb_perlin_ridge_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        }.normalize();
                    case 3:
                        return Color{
                                stb_perlin_ridge_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves),
                                stb_perlin_ridge_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                                + stb_perlin_ridge_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, offset, octaves)
                        }.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
            [[nodiscard]] inline Operator* breed(Operator* other) const final {
                auto parent = cast(other);
                auto perlin = create();
                perlin->_y = lerp(parent->_y, _y);
                perlin->_z = lerp(parent->_z, _z);
                perlin->octaves = (int)lerp(parent->octaves, octaves);
                perlin->gain = (float)lerp(parent->gain, gain);
                perlin->lacunarity = (float)lerp(parent->lacunarity, lacunarity);
                perlin->offset = (float)lerp(parent->offset, offset);
                perlin->scale = (float)lerp(parent->scale, scale);
                return perlin;
            }
            [[nodiscard]] inline Operator* mutate() final {
                if (chance())
                    _y += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    _z += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    scale += (float)randomDouble(-SCALE_MAX, SCALE_MAX) / 2;
                if (chance())
                    octaves += randomInt(-2, 2);
                octaves = std::max(2, octaves);
                if (chance())
                    gain += (float)randomDouble(-1, 1)/2;
                if (chance())
                    lacunarity += (float)randomDouble(-1, 1)/2;
                if (chance())
                    offset += (float)randomDouble(-1, 1)/2;
                return this;
            }
    };
    
    class PerlinFBMOperator : public OperatorBase<PerlinFBMOperator> {
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
                    scale = (float)randomDouble(SCALE_MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{
                                stb_perlin_fbm_noise3((float)args.time SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)_y, (float)(float)args.time SCALE_OP scale, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)_z, (float)_y, (float)(float)args.time SCALE_OP scale, lacunarity, gain, octaves)
                        }.normalize();
                    case 1:
                        return Color{
                                stb_perlin_fbm_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                    case 2:
                        return Color{
                                stb_perlin_fbm_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                    case 3:
                        return Color{
                                stb_perlin_fbm_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_fbm_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_fbm_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
            [[nodiscard]] inline Operator* breed(Operator* other) const final {
                auto parent = cast(other);
                auto perlin = create();
                perlin->_y = lerp(parent->_y, _y);
                perlin->_z = lerp(parent->_z, _z);
                perlin->octaves = (int)lerp(parent->octaves, octaves);
                perlin->gain = (float)lerp(parent->gain, gain);
                perlin->lacunarity = (float)lerp(parent->lacunarity, lacunarity);
                perlin->scale = (float)lerp(parent->scale, scale);
                return perlin;
            }
            [[nodiscard]] inline Operator* mutate() final {
                if (chance())
                    _y += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    _z += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    scale += (float)randomDouble(-SCALE_MAX, SCALE_MAX) / 2;
                if (chance())
                    octaves += randomInt(-2, 2);
                octaves = std::max(2, octaves);
                if (chance())
                    gain += (float)randomDouble(-1, 1)/2;
                if (chance())
                    lacunarity += (float)randomDouble(-1, 1)/2;
                return this;
            }
    };
    
    class PerlinTurbulenceOperator : public OperatorBase<PerlinTurbulenceOperator> {
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
                    scale = (float)randomDouble(SCALE_MIN, SCALE_MAX);
                } while (trunc((double)scale) == (double)scale);
                
                octaves = randomInt(2, 12);
                gain = (float)randomDouble(0.1, 1);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                auto left = args.left;
                auto right = args.right;
                switch (args.arguments){
                    case 0:
                        return Color{
                                stb_perlin_turbulence_noise3((float)args.time SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)_y, (float)(float)args.time SCALE_OP scale, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)_z, (float)_y, (float)(float)args.time SCALE_OP scale, lacunarity, gain, octaves)
                        }.normalize();
                    case 1:
                        return Color{
                                stb_perlin_turbulence_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                    case 2:
                        return Color{
                                stb_perlin_turbulence_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                    case 3:
                        return Color{
                                stb_perlin_turbulence_noise3((float)left.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.r SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.g SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves),
                                stb_perlin_turbulence_noise3((float)left.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                                + stb_perlin_turbulence_noise3((float)right.b SCALE_OP scale, (float)_y, (float)_z, lacunarity, gain, octaves)
                        }.normalize();
                }
                throw std::runtime_error("You shouldn't be able to reach here!");
            }
            [[nodiscard]] inline Operator* breed(Operator* other) const final {
                auto parent = cast(other);
                auto perlin = create();
                perlin->_y = lerp(parent->_y, _y);
                perlin->_z = lerp(parent->_z, _z);
                perlin->octaves = (int)lerp(parent->octaves, octaves);
                perlin->gain = (float)lerp(parent->gain, gain);
                perlin->lacunarity = (float)lerp(parent->lacunarity, lacunarity);
                perlin->scale = (float)lerp(parent->scale, scale);
                return perlin;
            }
            [[nodiscard]] inline Operator* mutate() final {
                if (chance())
                    _y += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    _z += randomDouble(-MAX, MAX) / 2;
                if (chance())
                    scale += (float)randomDouble(-SCALE_MAX, SCALE_MAX) / 2;
                if (chance())
                    octaves += randomInt(-2, 2);
                octaves = std::max(2, octaves);
                if (chance())
                    gain += (float)randomDouble(-1, 1)/2;
                if (chance())
                    lacunarity += (float)randomDouble(-1, 1)/2;
                return this;
            }
    };
    
    class ColorNoiseOperator : public OperatorBase<ColorNoiseOperator> {
        private:
            double scale;
        public:
            ColorNoiseOperator() {
                do {
                    scale = (float)randomDouble(1, 255);
                } while (trunc((double)scale) == (double)scale);
            }
            [[nodiscard]] inline Color apply(const OperatorArguments& args) const final {
                return Color{randomDouble(0, scale), randomDouble(0, scale), randomDouble(0, scale)}.normalize();
            }
    };
    
    enum class Operators {
        RandScalar,
        RandColor,
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
            {Operators::RandScalar, "S", 0},
            {Operators::RandColor, "C", 0},
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
