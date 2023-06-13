#include <string>
static std::string BasicShaderFragment = R"("
#version 330 core

in vec3 colors;
in vec2 uvs;

out vec4 color;

uniform sampler2D tex;

void main()
{
    color = vec4(colors, 1.0) * texture(tex, uvs);
}

")";