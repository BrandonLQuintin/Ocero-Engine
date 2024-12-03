#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture2;
uniform bool blueColor;
uniform bool firstTextureAtlas;

void main()
{
    // Sample main texture
    vec4 texelColor = texture(texture2, TexCoord);

    // Calculate shadow offset in UV space (1/16 = 0.0625 is one character in the atlas)
    // Reduce the shadow offset and clamp it to stay within the current character's bounds
    vec2 shadowOffset = vec2(0.0625 * 0.1, -0.0625 * 0.01);
    vec2 shadowUV = clamp(TexCoord + shadowOffset,
                         vec2(floor(TexCoord.x / 0.0625) * 0.0625, floor(TexCoord.y / 0.0625) * 0.0625),
                         vec2(ceil(TexCoord.x / 0.0625) * 0.0625, ceil(TexCoord.y / 0.0625) * 0.0625));
    vec4 shadowTexel = texture(texture2, shadowUV);

    // Render shadow first if it exists and is within bounds
    if (shadowTexel.a > 0.1 && shadowUV.x >= floor(TexCoord.x / 0.0625) * 0.0625) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Dark gray, fully opaque shadow
        return;
    }

    // Then render text if it exists
    if (texelColor.a > 0.1) {
        vec3 color = blueColor ? vec3(0.0, 0.0, 0.5) : vec3(0.5, 0.0, 0.0);
        FragColor = vec4(color, 1.0); // Make text fully opaque
        return;
    }

    // Discard transparent pixels
    discard;
}
