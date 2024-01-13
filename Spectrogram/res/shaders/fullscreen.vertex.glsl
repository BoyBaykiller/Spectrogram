#version 460 core

const vec2 Vertices[] =
{
    vec2( -1.0, -1.0 ),
    vec2(  3.0, -1.0 ),
    vec2( -1.0,  3.0 )
};

layout(location = 0) out InOutVars
{
    vec2 TexCoord;
} outData;

void main()
{
    gl_Position = vec4(Vertices[gl_VertexID], 0.0, 1.0);
    outData.TexCoord = gl_Position.xy * 0.5 + 0.5;
}