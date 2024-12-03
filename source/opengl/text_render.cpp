#include "text_render.h"

std::map<char, std::vector<float>> characterUV = {
    {'a', returnTextureUV(0, 0)},
    {'b', returnTextureUV(1, 0)},
    {'c', returnTextureUV(2, 0)},
    {'d', returnTextureUV(3, 0)},
    {'e', returnTextureUV(4, 0)},
    {'f', returnTextureUV(5, 0)},
    {'g', returnTextureUV(6, 0)},
    {'h', returnTextureUV(7, 0)},
    {'i', returnTextureUV(8, 0)},
    {'j', returnTextureUV(9, 0)},
    {'k', returnTextureUV(10, 0)},
    {'l', returnTextureUV(11, 0)},
    {'m', returnTextureUV(12, 0)},
    {'n', returnTextureUV(13, 0)},
    {'o', returnTextureUV(14, 0)},
    {'p', returnTextureUV(15, 0)},
    {'q', returnTextureUV(0, 1)},
    {'r', returnTextureUV(1, 1)},
    {'s', returnTextureUV(2, 1)},
    {'t', returnTextureUV(3, 1)},
    {'u', returnTextureUV(4, 1)},
    {'v', returnTextureUV(5, 1)},
    {'w', returnTextureUV(6, 1)},
    {'x', returnTextureUV(7, 1)},
    {'y', returnTextureUV(8, 1)},
    {'z', returnTextureUV(9, 1)},

    {'.', returnTextureUV(10, 1)},
    {',', returnTextureUV(11, 1)},
    {'?', returnTextureUV(12, 1)},
    {'!', returnTextureUV(13, 1)},
    {':', returnTextureUV(14, 1)},
    {';', returnTextureUV(15, 1)},
    {'(', returnTextureUV(0, 2)},
    {')', returnTextureUV(1, 2)},
    {'/', returnTextureUV(2, 2)},
    {'"', returnTextureUV(3, 2)},
    {'-', returnTextureUV(4, 2)},
    {'_', returnTextureUV(5, 2)},
    {'=', returnTextureUV(6, 2)},

    {'1', returnTextureUV(7, 2)},
    {'2', returnTextureUV(8, 2)},
    {'3', returnTextureUV(9, 2)},
    {'4', returnTextureUV(10, 2)},
    {'5', returnTextureUV(11, 2)},
    {'6', returnTextureUV(12, 2)},
    {'7', returnTextureUV(13, 2)},
    {'8', returnTextureUV(14, 2)},
    {'9', returnTextureUV(15, 2)},
    {'0', returnTextureUV(0, 3)},
    {' ', returnTextureUV(10, 10)},

    {'+', returnTextureUV(0, 4)},
    {'*', returnTextureUV(1, 4)},
    {'<', returnTextureUV(2, 4)},
    {'>', returnTextureUV(3, 4)},
    {'[', returnTextureUV(4, 4)},
    {']', returnTextureUV(5, 4)},
    {'\'', returnTextureUV(6, 4)},
    {'&', returnTextureUV(7, 4)},

    // Xbox buttons
    {'\x01', returnTextureUV(0, 5)}, // A
    {'\x02', returnTextureUV(1, 5)}, // B
    {'\x03', returnTextureUV(2, 5)}, // X
    {'\x04', returnTextureUV(3, 5)}, // Y

    // PlayStation buttons
    {'\x05', returnTextureUV(0, 6)}, // X
    {'\x06', returnTextureUV(1, 6)}, // O
    {'\x07', returnTextureUV(2, 6)}, // Square
    {'\x08', returnTextureUV(3, 6)}, // Triangle

    // D-pad
    {'\x09', returnTextureUV(0, 7)}, // Down
    {'\x0A', returnTextureUV(1, 7)}, // Right
    {'\x0B', returnTextureUV(2, 7)}, // Left
    {'\x0C', returnTextureUV(3, 7)}, // Up

    // Controller Icons
    {'`', returnTextureUV(0, 8)}, // Left Bumper
    {'~', returnTextureUV(3, 8)}, // Start
};

std::map<int, std::string> textOptions = {
    {1, "\\\\\\\\\\                                  take that, kuro!"},
    {2, "\\\\\\\\\\                                   whisker punch!"},
    {3, "\\\\\\\\\\                                    slow motion!"},
    {4, "\\\\\\\\\\                                      whiskers!"},
    {5, "\\\\\\\\\\                                       no way!"}
};

// Structure to hold character vertex data
struct CharacterVertex {
    float position[2];  // x, y position
    float texCoords[2]; // u, v coordinates
};

// Structure to hold character instance data
struct CharacterInstance {
    float offset[2];    // Position offset
    float uvBounds[4];  // UV bounds (uStart, uEnd, vStart, vEnd)
};

// Maximum number of characters in a batch
const size_t MAX_BATCH_CHARACTERS = 1024;
std::vector<CharacterVertex> baseQuad;
std::vector<CharacterInstance> instanceData;
GLuint quadVBO, instanceVBO;

void initTextRendering() {
    // Create base quad vertices
    baseQuad = {
        {{-0.013f,  0.035f}, {0.0f, 1.0f}},  // Top-left
        {{ 0.013f,  0.035f}, {1.0f, 1.0f}},  // Top-right
        {{-0.013f, -0.035f}, {0.0f, 0.0f}},  // Bottom-left
        {{ 0.013f, -0.035f}, {1.0f, 0.0f}}   // Bottom-right
    };

    // Create and setup VAO, VBOs
    glBindVertexArray(textVAO);

    // Setup quad VBO
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CharacterVertex) * baseQuad.size(), baseQuad.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (void*)offsetof(CharacterVertex, position));

    // Texture coordinate attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (void*)offsetof(CharacterVertex, texCoords));

    // Setup instance VBO
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CharacterInstance) * MAX_BATCH_CHARACTERS, nullptr, GL_DYNAMIC_DRAW);

    // Instance UV bounds attribute (4 floats)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*)offsetof(CharacterInstance, uvBounds));
    glVertexAttribDivisor(2, 1);

    // Instance position offset attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*)offsetof(CharacterInstance, offset));
    glVertexAttribDivisor(3, 1);

    instanceData.reserve(MAX_BATCH_CHARACTERS);
}

void renderText(Shader shader, std::string input) {
    if (!ENABLE_TEXT)
        return;

    glDisable(GL_DEPTH_TEST);
    shader.use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(textVAO);

    float textXOffset = -0.98f;
    float textYOffset = 0.95f;

    instanceData.clear();

    // Process all characters and prepare instance data
    for (char c : input) {
        if (c == '\\') {
            textXOffset = -0.98f;
            textYOffset -= 0.1f;
            continue;
        }

        auto it = characterUV.find(c);
        if (c != '\\' && it != characterUV.end()) {
            const auto& uvCoords = it->second;
            CharacterInstance instance;
            instance.offset[0] = textXOffset;
            instance.offset[1] = textYOffset;
            // Copy all 4 UV coordinates
            for (int i = 0; i < 4; i++) {
                instance.uvBounds[i] = uvCoords[i];
            }
            instanceData.push_back(instance);

            float charWidth = 0.0115f;
            textXOffset += charWidth * 1.8f;
            if (textXOffset > 0.9f) {
                textXOffset = -0.98f;
                textYOffset -= 0.07f * 1.5f;
            }
        }
    }

    if (!instanceData.empty()) {
        // Update instance buffer with new data
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(CharacterInstance) * instanceData.size(), instanceData.data());

        // Draw all characters in a single instanced draw call
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instanceData.size());
    }

    // Restore OpenGL state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);  // Re-enable depth testing for 3D objects
}

void newDialogue(){
    int oldChoice = dialogueChoice;
    dialogueChoice = randomInRange(1.0f, textOptions.size() + 1.0f);
    while (dialogueChoice == oldChoice){
        dialogueChoice = randomInRange(1.0f, textOptions.size() + 1.0f);
    }

    dialogue = textOptions[dialogueChoice];
}
