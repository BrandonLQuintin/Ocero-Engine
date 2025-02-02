#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// opengl & SFML
#include <glad/glad.h> // loads OpenGL pointers
#include <GLFW/glfw3.h> // window manager
#include <SFML/Audio.hpp>

//#include <stb_image.h> // helps load images (commented out because it's in textures.h)
#include <shader_m.h> // shader class from learnopengl.com on lesson "Coordinate Systems"

// opengl math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// REST API for ollama
#include <curl/curl.h>
#include "llm/llm_functions.h"

// variables (may also contain functions)
#include "globals.h"
#include "opengl/camera_variables.h"
#include "shapes/shapes.h"
#include "shapes/initialize_shape_vertices.h"

// functions
#include "opengl/window_functions.h"
#include "opengl/shader_functions.h"
#include "opengl/textures.h"
#include "opengl/text_render.h"
#include "game/calculate_fps.h"
#include "game/entity.h"
#include "game/trees.h"
#include "game/gameplay.h"
#include "game/main_menu.h"
#include "shapes/terrain.h"
#include "game/sound.h"
#include "resources/data/dataCollection.h"

int main(){
    GLFWwindow* window = createWindow();
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Shader billboardShader("shaders/billboard_shader.vs", "shaders/billboard_shader.fs");
    Shader phongShader("shaders/phong_lighting.vs", "shaders/phong_lighting.fs");
    Shader t("shaders/text_shader.vs", "shaders/text_shader.fs"); // variable name shortened so I can call it faster with renderText()

    billboardShader.use();
    billboardShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    billboardShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    billboardShader.setVec3("lightPos", glm::vec3(-10000.0f, 1000.0f, 10000.0f));

    phongShader.use();
    phongShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    phongShader.setVec3("lightPos", glm::vec3(-1000.0f, 1000.0f, 1000.0f));

    generatePlaneVertices(phongTerrainVector, phongTerrainIndicesVector);
    initializeAllShapes();
    initTextRendering();  // Initialize text rendering after shapes are initialized

    // ----- TEXTURES -----
    unsigned int texture1, texture2, texture3, texture4, texture5;
    loadTexture(texture1, "resources/textures/TextureAtlas.png");
    loadTexture(texture2, "resources/textures/TextAtlas.png");
    loadTexture(texture3, "resources/textures/GrassTiles.png");


    loadTexture(texture4, "resources/textures/SnowTiles.png"); // Snow textures set in effects part of rendering.
    loadTexture(texture5, "resources/textures/SnowTextureAtlas.png");

    billboardShader.use();
    billboardShader.setInt("texture1", 0);
    billboardShader.setInt("texture2", 1);
    billboardShader.setInt("texture5", 4);

    phongShader.use();
    phongShader.setInt("texture1", 0);
    phongShader.setInt("texture2", 1);
    phongShader.setInt("texture3", 2);
    phongShader.setInt("texture4", 3);
    phongShader.setInt("texture5", 4);

    t.use();
    t.setInt("texture1", 0);
    t.setInt("texture2", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture5);

    // first atlas values
    std::vector<float> boxAtlasUV = returnTextureUV(0, 0);
    std::vector<float> cobbleAtlasUV = returnTextureUV(1, 0);
    std::vector<float> oceroAtlasUV = returnTextureUV(2, 0);
    std::vector<float> shadowAtlasUV = returnTextureUV(0, 2);
    std::vector<float> rainAtlasUV = returnTextureUV(2, 2);

    std::vector<float> playerBackUV = returnTextureUV(0, 3);
    std::vector<float> playerRightUV = returnTextureUV(0, 4);
    std::vector<float> playerFrontUV = returnTextureUV(0, 5);
    std::vector<float> playerLeftUV = returnTextureUV(0, 6);
    std::vector<float> playerInjuryUV = returnTextureUV(1, 5);
    std::vector<float> playerShieldUV = returnTextureUV(0, 1);

    std::vector<float> enemyBackUV = returnTextureUV(0, 7);
    std::vector<float> enemyRightUV = returnTextureUV(0, 8);
    std::vector<float> enemyFrontUV = returnTextureUV(0, 9);
    std::vector<float> enemyLeftUV = returnTextureUV(0, 10);
    std::vector<float> enemyInjuryUV = returnTextureUV(1, 9);

    std::vector<float> dust1AtlasUV = returnTextureUV(13, 3);
    std::vector<float> dust2AtlasUV = returnTextureUV(14, 3);
    std::vector<float> dust3AtlasUV = returnTextureUV(15, 3);

    std::vector<float> spark1AtlasUV = returnTextureUV(13, 0);
    std::vector<float> spark2AtlasUV = returnTextureUV(14, 0);
    std::vector<float> spark3AtlasUV = returnTextureUV(15, 0);

    std::vector<float> punch1AtlasUV = returnTextureUV(13, 1);
    std::vector<float> punch2AtlasUV = returnTextureUV(14, 1);
    std::vector<float> punch3AtlasUV = returnTextureUV(15, 1);

    std::vector<float> redSquareUV = returnTextureUV(1, 1);

    std::vector<float> cloudAtlasUV = {0.6875f, 1.0f, 0.25f, 0.375f}; // these textures take up multiple 64x64 pixel grids so I gave it hard coded numbers.
    std::vector<float> treeAtlasUV = {0.75f, 1.0f, 0.0625f, 0.25f};

    // ----- INITIALIZE SOUNDS -----
    if (!loadSoundBuffer("resources/sounds/punch.wav")) {
        std::cerr << "Could not load sound file." << std::endl;
        return -1;
    }
    playSoundSilentlyMultipleTimes(30);
    music.openFromFile("resources/music/Dual-Dragon.wav");
    music.setVolume(30.0f);
    music.setLoop(true);

    winSong.openFromFile("resources/music/WinSong.wav");
    winSong.setVolume(30.0f);

    sf::SoundBuffer buffer2;
    if (!buffer2.loadFromFile("resources/sounds/explosion.wav")) {
        return -1; // error loading file
    }
    explosionSound.setBuffer(buffer2);
    explosionSound.setVolume(75.0f);

    // ----- INITIALIZE OBJECTS -----

    // initialize various sphere locations
    float heightOffset;

    // enemy
    enemyGoTo = glm::vec3(0.0f, 0.0f, 0.0f);
    enemyGoTo.y = getHeight(enemyGoTo.x, enemyGoTo.z) + 0.51f;
    enemy[3][0] = enemyGoTo.x;
    enemy[3][1] = enemyGoTo.y;
    enemy[3][2] = enemyGoTo.z;

    // player
    player[3][0] = 0.0f;
    player[3][1] = getHeight(0.0f, -5.0f) + 0.51f;
    player[3][2] = -3.5f;

    cameraPos.x = player[3][0];
    cameraPos.y = player[3][1] + 3.5f;
    cameraPos.z = player[3][2] + 3.5f;

    heightOffset = cameraPos.y - 3.5f;

    // generate sphere locations
    generateSurroundingPositions(initialSpherePositions, 25);
    for (int i = 0; i < initialSpherePositions.size(); i++){
        initialSpherePositions[i] = initialSpherePositions[i] * glm::vec3(30.0f, 30.0f, 30.0f);
        initialSpherePositions[i].y = 10.0f + heightOffset;
    }

    // 1 terrain
    shape terrains[1];
    int terrainsArraySize = sizeof(terrains) / sizeof(terrains[0]);

    // 2 boxes
    shape boxes[2];
    int boxesArraySize = sizeof(boxes) / sizeof(boxes[0]);
    boxes[0].modelMatrix = glm::translate(boxes[0].modelMatrix, glm::vec3(1.0f, 0.5f + heightOffset, -3.0f));
    boxes[1].modelMatrix = glm::translate(boxes[1].modelMatrix, glm::vec3(1.0f, 1.5f + heightOffset, -3.0f));

    // 1 floor
    shape floors[1];
    int floorsArraySize = sizeof(floors) / sizeof(floors[0]);
    floors[0].modelMatrix = glm::translate(floors[0].modelMatrix, glm::vec3(0.0f, 0.0f + heightOffset, 0.0f));
    floors[0].modelMatrix = glm::scale(floors[0].modelMatrix, glm::vec3(500.0f));

    // 1 pyramid
    shape pyramids[1];
    int pyramidsArraySize = sizeof(pyramids) / sizeof(pyramids[0]);
    pyramids[0].modelMatrix = glm::translate(pyramids[0].modelMatrix, glm::vec3(0.0f, 0.0f + heightOffset, -3.0f));

    // 25 spheres
    shape spheres[25];
    int spheresArraySize = sizeof(spheres) / sizeof(spheres[0]);
    for (int i = 0; i < spheresArraySize; i++){
        spheres[i].modelMatrix = glm::translate(spheres[i].modelMatrix, initialSpherePositions[i]);
    }

    // 1 cone
    shape cones[1];
    int conesArraySize = sizeof(cones) / sizeof(cones[0]);
    cones[0].modelMatrix = glm::translate(cones[0].modelMatrix, glm::vec3(-1.0f, 0.0f + heightOffset, -3.0f));

    // 1 tube
    shape tubes[1];
    int tubesArraySize = sizeof(tubes) / sizeof(tubes[0]);
    tubes[0].modelMatrix = glm::translate(tubes[0].modelMatrix, glm::vec3(-2.0f, 0.0f + heightOffset, -3.0f));

    // 1 billboard
    shape billboards[3];
    int billboardsArraySize = sizeof(billboards) / sizeof(billboards[0]);
    for (int i = 0; i < billboardsArraySize; i++){
        billboards[i].modelMatrix = glm::translate(billboards[i].modelMatrix, glm::vec3(1 - (i * 1.1), 0.0f + heightOffset, -5.0f));
    }

    // rain drops
    rainEntity rainDrops[600];
    int rainDropsArraySize = sizeof(rainDrops) / sizeof(rainDrops[0]);
    for (int i = 0; i < rainDropsArraySize; i++)
        initializeRainLocation(rainDrops[i]);

    if (ENABLE_DOWNFALL){
        phongShader.use();
        phongShader.setBool("isRaining", true);
        phongShader.setFloat("fogDensity", FOG_DENSITY);
        billboardShader.use();
        billboardShader.setBool("isRaining", true);
        billboardShader.setFloat("fogDensity", FOG_DENSITY);
    }

    int i = 0;

    // ----- MAIN PROGRAM -----
    while (!glfwWindowShouldClose(window)){

        if (i == 0){

            i++;
        }


        std::string animationText;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (mainMenu){
            if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
                detectController(1);
            }
            else{
                detectController(0);
            }
            processMainMenu(window, t, menuChoice);
            timeSinceLastEnemyWait = glfwGetTime();
        }
        if (glfwJoystickPresent(GLFW_JOYSTICK_2)) {
                detectController(1);
            }
            else{
                detectController(0);
        }

        if (SLOW_MO)
            deltaTime /= SLOW_MO_MULTIPLIER;

        processInput(window);

        glClearColor(0.792f, 0.957f, 1.00f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!FREECAM_CONTROLS_ENABLED && !enable_text_mode){
            cameraFront = glm::normalize(glm::vec3(player[3][0], player[3][1], player[3][2]) - cameraPos);
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        }

        billboardShader.use();
        billboardShader.setMat4("view", view);
        billboardShader.setMat4("projection", projection);
        billboardShader.setVec3("viewPos", cameraPos);

        phongShader.use();
        phongShader.setMat4("view", view);
        phongShader.setMat4("projection", projection);
        phongShader.setVec3("viewPos", cameraPos);

        // ----- EFFECTS ------

        if (toggle_snow){
            phongShader.use();
            phongShader.setBool("isSnowing", true);
            billboardShader.use();
            billboardShader.setBool("isSnowing", true);
        }
        else{
            phongShader.use();
            phongShader.setBool("isSnowing", false);
            billboardShader.use();
            billboardShader.setBool("isSnowing", false);
        }


        // NO EFFECTS USED IN ENGINE (ONLY IN GAME)

        // ----- OBJECTS ------

        // ### BOXES
        phongShader.use();
        glBindVertexArray(phongBoxVAO);
        setTextureUV(phongShader, boxAtlasUV, false);
        for (int i = 0; i < boxesArraySize; i++){
            if (i == 1){
                setTextureUV(phongShader, oceroAtlasUV, false);
            }
            phongShader.setMat4("model", boxes[i].modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ### PYRAMIDS
        phongShader.use();
        glBindVertexArray(phongPyramidVAO);
        setTextureUV(phongShader, boxAtlasUV, false);
        for (int i = 0; i < pyramidsArraySize; i++){

            phongShader.setMat4("model", pyramids[i].modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 18);
        }

        // ### SPHERES
        phongShader.use();
        glBindVertexArray(phongSphereVAO);
        setTextureUV(phongShader, boxAtlasUV, false);
        for (int i = 0; i < spheresArraySize; i++){

            phongShader.setMat4("model", spheres[i].modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, phongSphereVerticesArraySize);
        }

        // ### CONES
        phongShader.use();
        glBindVertexArray(phongConeVAO);
        setTextureUV(phongShader, boxAtlasUV, false);
        for (int i = 0; i < conesArraySize; i++){

            phongShader.setMat4("model", cones[i].modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, phongConeVerticesArraySize);
        }

        // ### TUBES
        phongShader.use();
        glBindVertexArray(phongCylinderVAO);
        setTextureUV(phongShader, boxAtlasUV, false);
        for (int i = 0; i < tubesArraySize; i++){

            phongShader.setMat4("model", tubes[i].modelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, phongCylinderVerticesArraySize);
        }

        // ### BILLBOARDS
        billboardShader.use();
        glBindVertexArray(phongBillboardVAO);
        setTextureUV(billboardShader, oceroAtlasUV, false);
        for (int i = 0; i < billboardsArraySize; i++){
            billboards[i].modelMatrix[3][1] += 1 * deltaTime;
            billboards[i].modelMatrix[3][2] += -1 * deltaTime;

            billboardShader.setMat4("model", billboards[i].modelMatrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // ### TERRAIN
        for (int i = 0; i < terrainsArraySize; i++){
            phongShader.use();
            glBindVertexArray(phongTerrainVAO);
            setTextureUV(phongShader, oceroAtlasUV, true);

            phongShader.setMat4("model", terrains[i].modelMatrix);
            glDrawElements(GL_TRIANGLES, phongTerrainIndicesVector.size(), GL_UNSIGNED_INT, 0);
        }

        // ### RAIN
        if (ENABLE_DOWNFALL){
            billboardShader.use();
            glBindVertexArray(phongBillboardVAO);
            setTextureUV(billboardShader, rainAtlasUV, false);
            for (int i = 0; i < rainDropsArraySize; i++){
                rainDrops[i].modelMatrix[3][1] -= rainDrops[i].speed * deltaTime;

                if (rainDrops[i].modelMatrix[3][1] < cameraPos.y - 5.0f){ // check if rain is below player first before performing other checks
                    setRainLocation(rainDrops[i]);
                }
                else{
                    float distanceFromPlayerZ = std::fabs(rainDrops[i].modelMatrix[3][2] - cameraPos.z);
                    float distanceFromPlayerX = std::fabs(rainDrops[i].modelMatrix[3][0] - cameraPos.x);

                    if (distanceFromPlayerZ > 15.0f || distanceFromPlayerX > 15.0f){
                        setRainLocation(rainDrops[i]);
                    }
                }

                billboardShader.setMat4("model", rainDrops[i].modelMatrix);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }


        // ### TREES
        phongShader.use();
        phongShader.setBool("isTree", true);

        glEnable(GL_BLEND);
        glBindVertexArray(phongBillboardVAO);
        setTextureUV(phongShader, treeAtlasUV, false);

        // return list of chunks to render 3x3 chunks.
        std::array<std::pair<int, int>, 9> renderingChunks;
        std::pair<int, int> origin = getChunkCoordinates(cameraPos.x, cameraPos.z); // origin x & y chunk
        generateSurroundingChunks(renderingChunks, origin);

        initializeTreeLocations(renderingChunks);

        // uses binary search to insert in correct order instead of sorting every frame
        for (int x = 0; x < 9; x++){
            for (int i = 0; i < trees.size(); i++) { // Adds activeTrees based on distance (before rendering) so transparency occurs correctly.
                trees[x][i].distanceFromCamera = renderDistanceCheck(cameraPos, trees[x][i].modelMatrix[3]);

                if (trees[x][i].distanceFromCamera < DESPAWN_DISTANCE) {
                    bool inserted = false;
                    auto it = std::lower_bound(activeTrees.begin(), activeTrees.end(), trees[x][i],
                        [](const tree& a, const tree& b) {
                            return a.distanceFromCamera > b.distanceFromCamera;
                        });
                    activeTrees.insert(it, trees[x][i]);

                }
            }
        }

        for (int i = 0; i < activeTrees.size(); i++) {
            if (activeTrees[i].distanceFromCamera < 5.0f){
                phongShader.setBool("isTransparent", true);
                phongShader.setMat4("model", activeTrees[i].modelMatrix);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                phongShader.setMat4("model", activeTrees[i].cloneModelMatrix);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                phongShader.setBool("isTransparent", false);
            }
            else{
                phongShader.setMat4("model", activeTrees[i].modelMatrix);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                phongShader.setMat4("model", activeTrees[i].cloneModelMatrix);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        activeTrees.clear();
        phongShader.setBool("isTree", false);

        glDisable(GL_BLEND);
        int fps = calculateAverageFPS(timeSinceLastFPSCalculation, deltaTime, fpsVector, SLOW_MO);

        // ----- DRAW TEXT ------

        std::string text;
        if (SHOW_FPS)
            text += std::to_string(fps) + " fps";
        if (FREECAM_CONTROLS_ENABLED){
            text += "\\camera coordinates: [" + std::to_string(cameraPos.x) + ", "+ std::to_string(cameraPos.y) + ", " + std::to_string(cameraPos.z) + "]";
        }

        renderText(t, text);
        renderText(t, animationText);
        if (textInputBuffer != ""){
                renderText(t, "\\\\\\\\\\\\\\\\> " + textInputBuffer);
        }
        renderText(t, LLMfunctionSelection);
        renderText(t, llmOutput);

        // end of a frame

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();
    return 0;

}
