#include "functions.h"

std::array<int, TREE_AMOUNT * 2> generateRandomConsistentNumbers(int chunkX, int chunkZ){

    std::seed_seq seed{chunkX, chunkZ};
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dist(0, SMALL_CHUNK_SIZE);

    // Generate 10 random numbers
    std::array<int, TREE_AMOUNT * 2> randomNumbers;
    for (int i = 0; i < TREE_AMOUNT * 2; ++i) {
        randomNumbers[i] = dist(generator);
    }

    return randomNumbers;
}

std::pair<int, int> getChunkCoordinates(float playerX, float playerZ) {
    int chunkX = static_cast<int>(std::floor(playerX / SMALL_CHUNK_SIZE));
    int chunkZ = static_cast<int>(std::floor(playerZ / SMALL_CHUNK_SIZE));
    return {chunkX, chunkZ};
}
