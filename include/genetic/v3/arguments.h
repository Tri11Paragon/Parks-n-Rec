//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_ARGUMENTS_H
#define PARKSNREC_ARGUMENTS_H

#include <genetic/util.h>

namespace parks::genetic {
    
    constexpr double d = 0.4;
    constexpr double m = 0.2;
    
    struct Color {
        double r, g, b;
        
        explicit Color(double _r, double _g, double _b) {
            r = _r;
            g = _g;
            b = _b;
            if (r < 0)
                r = std::abs(r);
            if (g < 0)
                g = std::abs(g);
            if (b < 0)
                b = std::abs(b);
            if (r > 1)
                r = r - trunc(r);
            if (g > 1)
                g = g - trunc(g);
            if (b > 1)
                b = b - trunc(b);
        }
        
        explicit Color(double v) {
            r = v;
            g = 0;
            b = 0;
        }
    };
    
    inline Color normalize(Color c) {
        auto mag = std::sqrt(c.r * c.r + c.g * c.g + c.b * c.b);
        if (mag == 0)
            return c;
        return Color{c.r / mag, c.g / mag, c.b / mag};
    }
    
    struct RandomScalar {
        inline static Color get() {
            return Color{randomDouble(0, 1)};
        }
        
        inline static Color get(Color s) {
            return Color{s.r + randomDouble(-d, d)};
        }
        
        inline static Color get(Color s1, Color s2) {
            auto min = std::min(s1.r, s2.r);
            auto max = std::max(s1.r, s2.r);
            if (min == max)
                return Color{min};
            return Color{randomDouble(min, max)};
        }
    };
    
    struct RandomColor {
        inline static Color get() {
            return normalize(Color{randomDouble(0, 1), randomDouble(0, 1), randomDouble(0, 1)});
        }
        
        inline static Color get(Color color) {
            return normalize(
                    Color{color.r + randomDouble(-d, d), color.g + randomDouble(-d, d), color.b +
                                                                                       randomDouble(
                                                                                               -d, d
                                                                                       )}
            );
        }
        
        inline static Color get(Color p1, Color p2) {
            auto minR = std::min(p1.r, p2.r);
            auto maxR = std::max(p1.r, p2.r);
            auto minG = std::min(p1.g, p2.g);
            auto maxG = std::max(p1.g, p2.g);
            auto minB = std::min(p1.b, p2.b);
            auto maxB = std::max(p1.b, p2.b);
            return normalize(
                    Color{randomDouble(minR, maxR), randomDouble(minG, maxG), randomDouble(minB, maxB)}
            );
        }
    };
    
    struct OperatorArguments {
        int numberOfArgs;
        Color left, right;
    };
    
    class ParameterSet {
        private:
            std::vector<Color> parameters;
        public:
            ParameterSet() = default;
            
            inline const Color& operator[](int index) const {return parameters[index];}
            inline size_t size(){return parameters.size();}
            
            void add(Color c) {parameters.push_back(c);}
    };
    
}

#endif //PARKSNREC_ARGUMENTS_H
