#version 330 core

// ------- DEFAULT RAYLIB SHADER STUFF ------- //
// input vertex attrs
in vec2 fragTexCoord;
in vec4 fragColor;

// texture uniforms
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// color output
out vec4 finalColor;

// -------- CUSTOM STUFF -------- //

void main()
{
    // texel color from sampler2D
    vec4 color = texture(texture0, fragTexCoord) * colDiffuse * fragColor;

    // output color
    finalColor = color;
}
