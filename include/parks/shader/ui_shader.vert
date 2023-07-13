#ifdef __cplusplus
#include <string>
static std::string UIShaderVertex = R"("
#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 uvs;

layout (std140) uniform Matrices
{
    mat4 perspective;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
};

uniform mat4 transform;

void main()
{
    gl_Position = ortho * transform * vec4(position.x, position.y, position.z, 1.0);
    uvs = uv;
}


")";
#endif