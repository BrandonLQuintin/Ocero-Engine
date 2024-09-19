#pragma once
#include <iostream>
#include <random>
#include <array>
#include "../globals.h"

std::array<int, TREE_AMOUNT * 2> generateRandomConsistentNumbers(int chunkX, int chunkZ);

std::pair<int, int> getChunkCoordinates(float playerX, float playerZ);
