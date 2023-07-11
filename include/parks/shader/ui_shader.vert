#ifdef __cplusplus
#include <string>
static std::string BasicShaderVertex = R"("
#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;

out vec3 colors;
out vec2 uvs;

layout (std140) uniform Matrices
{
    mat4 perspective;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
};

void main()
{
    gl_Position = pvm * vec4(position.x, position.y, position.z, 1.0);
    colors = color;
    uvs = uv;
}


")";
#endif