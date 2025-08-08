#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include "../globals.h"

extern std::vector<sf::Sound> sounds;

// Lazy accessor to avoid static initialization issues
sf::SoundBuffer& getPunchBuffer();

bool loadSoundBuffer(const std::string& filePath);

sf::Sound* findAvailableSound();

void createAndPlaySound();

void playSound();

void playSoundSilentlyMultipleTimes(int times);
