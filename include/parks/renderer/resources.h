//
// Created by brett on 6/12/23.
//

#ifndef PARKSNREC_RESOURCES_H
#define PARKSNREC_RESOURCES_H

#include <string>
#include <parks/renderer/OpenGL.h>

namespace parks::resources {
    
    namespace _defaults_ {
        static const std::string ASSUME_TEXTURE_NAME_ID = "NULL.NULL";
    }
    
    void init(int threads);
    void loadTexture(const std::string& texture_path, const std::string& texture_name = _defaults_::ASSUME_TEXTURE_NAME_ID);
    /**
     * @param texture_name texture name set when loading or path
     * @return GlTexture based on a texture name. Will return null if texture isn't loaded.
     */
    GLTexture2D* getTexture(const std::string& texture_name);
    /**
     * The blocking method which will wait for all textures to be loaded into memory
     */
    void beginLoading();
    void cleanup();
    
}

#endif //PARKSNREC_RESOURCES_H
