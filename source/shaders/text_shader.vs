#version 330 core
layout (location = 0) in vec2 aPos;        // Base quad vertices
layout (location = 1) in vec2 aTexCoord;   // Base texture coordinates
layout (location = 2) in vec4 aUVBounds;   // UV bounds (uStart, uEnd, vStart, vEnd)
layout (location = 3) in vec2 aOffset;     // Character position offset

out vec2 TexCoord;

void main()
{
    vec2 worldPos = aPos + aOffset;
    gl_Position = vec4(worldPos, 0.0, 1.0);
    
    // Interpolate UV coordinates based on the UV bounds
    TexCoord = vec2(
        mix(aUVBounds.x, aUVBounds.y, aTexCoord.x),
        mix(aUVBounds.z, aUVBounds.w, aTexCoord.y)
    );
}
