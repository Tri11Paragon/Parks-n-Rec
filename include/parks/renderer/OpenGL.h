//
// Created by brett on 6/11/23.
//

#ifndef PARKSNREC_OPENGL_H
#define PARKSNREC_OPENGL_H

#include <glad/gl.h>
#include <vector>
#include <blt/math/math.h>
#include <blt/std/string.h>
#include <string>
#include <unordered_map>
#include <stb/stb_image.h>

namespace parks {
    
    class GLTexture2D {
        private:
            GLuint textureID = 0;
            struct {
                int width = 0, height = 0, channels = 4;
            } textureInfo;
        public:
            GLTexture2D() {
                glGenTextures(1, &textureID);
            }
            
            void bind() const {
                glBindTexture(GL_TEXTURE_2D, textureID);
            }
            
            void allocate(GLenum type, int width, int height, int channels = 4) {
                auto storage_type = channels == 4 ? GL_RGBA : GL_RGB;
                bind();
                glTexImage2D(GL_TEXTURE_2D, 0, storage_type, width, height, 0, storage_type, type,
                             nullptr);
                textureInfo.width = width;
                textureInfo.height = height;
                textureInfo.channels = channels;
            }
            
            void upload(void* data, GLenum type, int width, int height, int channels = 4) {
                auto storage_type = channels == 4 ? GL_RGBA : GL_RGB;
                bind();
                if (textureInfo.width != width || textureInfo.height != height || textureInfo.channels != channels)
                    allocate(type, width, height, channels);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, storage_type, type, data);
                textureInfo.width = width;
                textureInfo.height = height;
                textureInfo.channels = channels;
            }
            
            ~GLTexture2D() {
                glDeleteTextures(1, &textureID);
            }
    };
    
    struct VBOData {
        GLenum target;
        void* data;
        GLuint size;
        GLenum usage = GL_STATIC_DRAW;
        
        VBOData(GLenum target, void* data, GLuint size);
        
        VBOData(GLenum target, void* data, GLuint size, GLenum usage);
    };
    
    struct VBOAttributes {
        GLuint loc;
        GLint size;
        GLenum type;
        GLsizei stride;
        GLint offset = 0;
        
        VBOAttributes(GLuint loc, GLint size, GLenum type, GLsizei stride, GLint offset);
        
        VBOAttributes(GLuint loc, GLint size, GLenum type, GLsizei stride);
        
        VBOAttributes(GLuint loc, GLint size, GLenum type);
    };
    
    class VAOStorageObject {
        private:
            struct VAO {
                GLuint vaoID = 0;
            };
            struct VBO {
                GLuint vboID;
                GLuint bindType;
            };
            
            VAO vao;
            std::vector<VBO> associatedVBOs;
            VBO ebo;
        public:
            VAOStorageObject() {
                glGenVertexArrays(1, &vao.vaoID);
            }
            
            /**
             * Binds only the VAO and none of the VBOs
             */
            inline void bind() const {
                glBindVertexArray(vao.vaoID);
            }
            
            inline void bindEBO() const {
                glBindBuffer(ebo.bindType, ebo.vboID);
            }
            
            /**
             * Binds both the VAO and all the associated VBOs
             */
            inline void bindAll() const {
                bind();
                for (const VBO& v : associatedVBOs)
                    glBindBuffer(v.bindType, v.vboID);
            }
            
            void createVBO(VBOData data) {
                VBO vbo{};
                vbo.bindType = data.target;
                glGenBuffers(1, &vbo.vboID);
                glBindBuffer(data.target, vbo.vboID);
                glBufferData(data.target, data.size, data.data, data.usage);
                associatedVBOs.push_back(vbo);
                if (vbo.bindType == GL_ELEMENT_ARRAY_BUFFER)
                    ebo = vbo;
            }
            
            void createVBO(VBOData data, VBOAttributes attribs) {
                createVBO(data);
                glVertexAttribPointer(
                        attribs.loc, attribs.size, attribs.type, GL_FALSE, attribs.stride,
                        (GLvoid*) (size_t) attribs.offset
                );
                glEnableVertexAttribArray(attribs.loc);
            }
            
            void createVBO(VBOData data, const std::vector<VBOAttributes>& attribs){
                createVBO(data);
                for (auto attrib : attribs) {
                    glVertexAttribPointer(
                            attrib.loc, attrib.size, attrib.type, GL_FALSE, attrib.stride,
                            (GLvoid*) (size_t) attrib.offset
                    );
                    glEnableVertexAttribArray(attrib.loc);
                }
            }
            
            ~VAOStorageObject() {
                glDeleteVertexArrays(1, &vao.vaoID);
                for (const auto& v : associatedVBOs)
                    glDeleteBuffers(1, &v.vboID);
            }
    };
    
    class ShaderBase {
        protected:
            struct IntDefaultedToMinusOne {
                GLint i = -1;
            };
            std::unordered_map<std::string, IntDefaultedToMinusOne> uniformVars;
            GLuint programID = 0;
            
            inline GLint getUniformLocation(const std::string& name) {
                if (uniformVars[name].i != -1)
                    return uniformVars[name].i;
                int loc = glGetUniformLocation(programID, name.c_str());
                uniformVars[name].i = loc;
                return loc;
            }
        
        public:
            inline void bind() const {
                glUseProgram(programID);
            }
            
            inline void setBool(const std::string& name, bool value) {
                glUniform1i(getUniformLocation(name), (int) value);
            }
            
            inline void setInt(const std::string& name, int value) {
                glUniform1i(getUniformLocation(name), value);
            }
            
            inline void setFloat(const std::string& name, float value) {
                glUniform1f(getUniformLocation(name), value);
            }
            
            inline void setMatrix(const std::string& name, blt::mat4x4& matrix) {
                glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.ptr());
            }
            
            inline void setVec3(const std::string& name, const blt::vec3& vec) {
                glUniform3f(getUniformLocation(name), vec.x(), vec.y(), vec.z());
            }
            
            inline void setVec4(const std::string& name, const blt::vec4& vec) {
                glUniform4f(getUniformLocation(name), vec.x(), vec.y(), vec.z(), vec.w());
            }
            
            inline void setVec2(const std::string& name, float x, float y) {
                glUniform2f(getUniformLocation(name), x, y);
            }
            
            inline void setVec3(const std::string& name, float x, float y, float z) {
                glUniform3f(getUniformLocation(name), x, y, z);
            }
            
            inline void setVec4(const std::string& name, float x, float y, float z, float w) {
                glUniform4f(getUniformLocation(name), x, y, z, w);
            }
    };

/**
 * This part was made for this assignment and will likely be used in future projects
 */
    class ComputeShader : public ShaderBase {
        private:
            GLuint shaderID = 0;
        public:
            explicit ComputeShader(const std::string& shader_source, bool loadAsString = true);
            
            inline void execute(int x, int y, int z) const {
                bind();
                glDispatchCompute(x, y, z);
            }
            
            ~ComputeShader();
    };

/**
 * Note: This is taken from my final project,
 * https://github.com/Tri11Paragon/COSC-3P98-Final-Project/blob/main/include/render/gl.h
 */
    
    class Shader : public ShaderBase {
        private:
            GLuint vertexShaderID = 0;
            GLuint fragmentShaderID = 0;
            // while these will remain unused. (Webgl2 apparently doesn't support them despite being based on GL4.3? that's a TODO!)
            GLuint geometryShaderID = 0;
            // this would be very useful however it is highly unlikely webgl will support it
            // im leaving some of this stuff in here because I might expand the native application to use some of it.
            // im trying to keep the web and native versions the same though
            GLuint tessellationShaderID = 0;
            
            static unsigned int createShader(const std::string& source, int type);
        
        public:
            /**
             * Creates a shader
             * @param vertex vertex shader source or file
             * @param fragment fragment shader source or file
             * @param geometry geometry shader source or file (optional)
             * @param load_as_string load the shader as a string (true) or use the string to load the shader as a file (false)
             */
            Shader(
                    const std::string& vertex, const std::string& fragment,
                    const std::string& geometry = "",
                    bool load_as_string = true
            );
            
            Shader(Shader&& move) noexcept;
            
            // used to set the location of VAOs to the in variables in opengl shaders.
            void bindAttribute(int attribute, const std::string& name) const;
            
            // used to set location of shared UBOs like the perspective and view matrix
            void setUniformBlockLocation(const std::string& name, int location) const;
            
            ~Shader();
    };
}

#endif //PARKSNREC_OPENGL_H
