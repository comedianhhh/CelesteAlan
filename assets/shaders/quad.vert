#version 430 core
//https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Shader_modules
void main()
{
    // Generating Vertices on the GPU
    // mostly because we have a 2D Engine

    // OpenGL Coordinates
    // -1/ 1                1/ 1
    // -1/-1                1/-1

    vec2 vertices[6] = 
    {
        //Top left
        vec2(-0.5, 0.5),
        //Bottom left
        vec2(-0.5, -0.5),
        //Top right
        vec2(0.5, 0.5),
        //Top right
        vec2(0.5, 0.5),
        //Bottom left
        vec2(-0.5, -0.5),
        //Bottom Right
        vec2(0.5, -0.5)
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}