// Our simple shader
#version 410

in vec4 Position;
in vec4 Color;

out vec4 vColor;

uniform mat4 ProjectionViewModel;

void main()
{
   vColor = Color;
   gl_Position = ProjectionViewModel * Position;
}