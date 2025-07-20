#version 330 core

// ------- DEFAULT RAYLIB SHADER STUFF ------- //
// input vertex attrs
in vec2 fragTexCoord;
in vec4 fragColor;

// texture uniforms
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform sampler2D lighting;

uniform sampler2D noise;
uniform int width;
uniform int height;
uniform float time;
uniform float scrollx;
uniform float scrolly;

// color output
out vec4 finalColor;

// -------- CUSTOM STUFF -------- //

void main()
{
    // texel color from sampler2D

    vec2 uv = fract(vec2(fragTexCoord.x * float(width) / float(height), fragTexCoord.y * 2.0));
    vec3 fog = texture(noise, fract(vec2(uv.x + scrollx * 0.001, uv.y - scrolly * 0.001 + sin(time * 0.02)))).rgb + texture(noise, fract(vec2(uv.x + sin(time * 0.03) + scrollx * 0.001, uv.y - scrolly * 0.001) * 2.0)).rgb;

    if (fog.r * 0.5 > 0.5)
    {
        fog = vec3(0.5);
    } else if (fog.r * 0.5 > 0.3) {
        fog = vec3(0.2);
    } else {
        fog = vec3(0.1);
    }

    vec4 color = texture(texture0, vec2(fragTexCoord.x, fragTexCoord.y)) * colDiffuse * fragColor;
    color.rgb += fog * 0.1;

    vec4 lighting = texture(lighting, fract(vec2(fragTexCoord.x + fog.r * 0.01, fragTexCoord.y + fog.r * 0.005)));

    if (lighting.r > 0.6)
    {
        lighting.rgb = vec3(1.0);
    } else if (lighting.r > 0.05)
    {
        lighting.rgb = vec3(0.6);
    } else {
        lighting.rgb = vec3(0.5);
    }

    color.rgb = color.rgb * 0.2 + color.rgb * lighting.rgb;

    // output color
    finalColor = color;
}
