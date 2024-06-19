#version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 2) in vec2 texCoord;
    out vec2 TexCoord;
    uniform mat4 modelMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 projectionMatrix;
    void main()
    {
        vec4 viewPos = viewMatrix * modelMatrix * vec4(position, 0.0f);
        viewPos.w = 1.0;
        gl_Position = projectionMatrix * viewPos;
        TexCoord = texCoord;
    }