//
// Created by brett on 7/17/23.
//

#ifndef PARKSNREC_UTIL_H
#define PARKSNREC_UTIL_H


namespace parks {

enum class ParameterType {
    SCALAR, COLOR, VARIABLE, IMAGE
};

struct Image {
    std::shared_ptr<double> image;
    unsigned int width, height;
    
    void write(unsigned int x, unsigned int y, const blt::vec3d& color) const {
        auto pos = x * CHANNELS + y * width * CHANNELS;
        image.get()[pos] = color.x();
        image.get()[pos + 1] = color.y();
        image.get()[pos + 2] = color.z();
    }
    
    [[nodiscard]] blt::vec3d read(unsigned int x, unsigned int y) const {
        auto pos = x * CHANNELS + y * width * CHANNELS;
        auto r = image.get()[pos];
        auto g = image.get()[pos + 1];
        auto b = image.get()[pos + 2];
        return blt::vec3d{r, g, b};
    }
};

struct Parameter {
    private:
        ParameterType type;
        std::variant<double, blt::vec3d, unsigned int, Image> value;
    public:
        explicit Parameter(double s): value(s) {type = ParameterType::SCALAR;}
        explicit Parameter(blt::vec3d c): value(c) {type = ParameterType::COLOR;}
        explicit Parameter(unsigned int v): value(v) {type = ParameterType::VARIABLE;}
        explicit Parameter(Image&& i): value(i) {type = ParameterType::IMAGE;}
        
        [[nodiscard]] ParameterType getType() const {
            return type;
        }
        
        template<typename T>
        [[nodiscard]] T& get() {
            return std::get<T>(value);
        }
        
        template<typename T>
        [[nodiscard]] const T& get() const {
            return std::get<T>(value);
        }
        
        template<typename Op>
        Parameter apply(Op f, double scalar) const {
            if (type == ParameterType::SCALAR)
                return Parameter{f(get<double>(), scalar)};
            else if (type == ParameterType::COLOR) {
                auto color = get<blt::vec3d>();
                return Parameter{blt::vec3d{f(color.x(), scalar), f(color.y(), scalar), f(color.z(), scalar)}};
            } else if (type == ParameterType::VARIABLE){
                return Parameter{f((double)get<unsigned int>(), scalar)};
            } else if (type == ParameterType::IMAGE){
                auto image = get<Image>();
                auto newImage = Image{std::shared_ptr<double>(new double[image.width * image.height * CHANNELS]), image.width, image.height};
                
                for (unsigned int i = 0; i < image.width; i++){
                    for (unsigned int j = 0; j < image.height; j++){
                        auto oldColor = image.read(i, j);
                        newImage.write(i, j, blt::vec3d{f(oldColor.x(), scalar), f(oldColor.y(), scalar), f(oldColor.z(), scalar)});
                    }
                }
                
                return Parameter{std::move(newImage)};
            }
        }
        
        template<typename Op>
        Parameter apply(Op f, const blt::vec3d& color) const {
            if (type == ParameterType::SCALAR)
                return Parameter{blt::vec3d{f(get<double>(), color.x()), f(get<double>(), color.y()), f(get<double>(), color.z())}};
            else if (type == ParameterType::COLOR) {
                auto ourColor = get<blt::vec3d>();
                return Parameter{blt::vec3d{f(ourColor.x(), color.x()), f(ourColor.y(), color.y()), f(ourColor.z(), color.z())}};
            } else if (type == ParameterType::VARIABLE){
                return Parameter{blt::vec3d{f(get<unsigned int>(), color.x()), f(get<unsigned int>(), color.y()), f(get<unsigned int>(), color.z())}};
            } else if (type == ParameterType::IMAGE){
                auto image = get<Image>();
                auto newImage = Image{std::shared_ptr<double>(new double[image.width * image.height * CHANNELS]), image.width, image.height};
                
                for (unsigned int i = 0; i < image.width; i++){
                    for (unsigned int j = 0; j < image.height; j++){
                        auto oldColor = image.read(i, j);
                        newImage.write(i, j, blt::vec3d{f(oldColor.x(), color.x()), f(oldColor.y(), color.y()), f(oldColor.z(), color.z())});
                    }
                }
                
                return Parameter{std::move(newImage)};
            }
        }
        
        template<typename Op>
        Parameter apply(Op f, unsigned int variable) const {
            if (type == ParameterType::SCALAR)
                return Parameter{f(get<double>(), (double) variable)};
            else if (type == ParameterType::COLOR) {
                auto color = get<blt::vec3d>();
                return Parameter{blt::vec3d{f(color.x(), (double) variable), f(color.y(), (double)variable), f(color.z(), (double)variable)}};
            } else if (type == ParameterType::VARIABLE){
                return Parameter{f(get<unsigned int>(), variable)};
            } else if (type == ParameterType::IMAGE){
                auto image = get<Image>();
                auto newImage = Image{std::shared_ptr<double>(new double[image.width * image.height * CHANNELS]), image.width, image.height};
                
                for (unsigned int i = 0; i < image.width; i++){
                    for (unsigned int j = 0; j < image.height; j++){
                        auto oldColor = image.read(i, j);
                        newImage.write(i, j, blt::vec3d{f(oldColor.x(), (double)variable), f(oldColor.y(), (double)variable), f(oldColor.z(), (double)variable)});
                    }
                }
                
                return Parameter{std::move(newImage)};
            }
        }
        
        template<typename Op>
        Parameter apply(Op f, const Image& image) const {
            if (type == ParameterType::IMAGE){
                auto ourImage = get<Image>();
                if (ourImage.width != image.width || ourImage.height != image.height){
                    BLT_ERROR("Unable to apply to images of differing sizes!");
                    throw std::runtime_error("Unable to apply to images of differing sizes!");
                }
                auto newImage = Image{std::shared_ptr<double>(new double[ourImage.width * ourImage.height * CHANNELS]), ourImage.width, ourImage.height};
                
                for (unsigned int i = 0; i < ourImage.width; i++){
                    for (unsigned int j = 0; j < ourImage.height; j++){
                        auto oldColor = ourImage.read(i, j);
                        auto newColor = image.read(i, j);
                        newImage.write(i, j, blt::vec3d{f(oldColor.x(), (double)newColor.x()), f(oldColor.y(), (double)newColor.y()), f(oldColor.z(), (double)newColor.z())});
                    }
                }
                
                return Parameter{std::move(newImage)};
            } else {
                BLT_WARN("Please apply non-images to images instead of images to non-images");
            }
        }
        
        template<typename Op>
        inline Parameter apply(Op f, const Parameter& param) const {
            if (param.type == ParameterType::SCALAR)
                return apply(f, param.get<double>());
            else if (type == ParameterType::COLOR)
                return apply(f, param.get<blt::vec3d>());
            else if (type == ParameterType::VARIABLE)
                return apply(f, param.get<unsigned int>());
            else
                return apply(f, param.get<Image>());
        }
};

}


#endif //PARKSNREC_UTIL_H
