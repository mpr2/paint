#version 460 core
layout (location = 0) in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D tex_screen;
uniform sampler2D tex_canvas;

void main() {
    vec4 tex1 = texture(tex_screen, texCoord);
    vec4 tex2 = texture(tex_canvas, texCoord);

    vec4 canvas_color = vec4(1., 1., 1., 1.);

    tex2 = mix(canvas_color, tex2, tex2.a);
    FragColor = mix(tex2, tex1, tex1.a);
}
