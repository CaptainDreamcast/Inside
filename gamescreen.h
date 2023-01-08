#pragma once

#include <prism/blitz.h>


Screen* getGameScreen();

MugenSpriteFile* getGameSprites();
MugenAnimations* getGameAnimations();
MugenSounds* getGameSounds();

bool getGlobalVariable(const std::string& name);
void setGlobalVariable(const std::string& name);