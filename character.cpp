#include "character.h"

#include "gamescreen.h"

#define CHARACTER_Z 5

class Character {
public:
	int entity;

	Character() {
		entity = addBlitzEntity(Position(160, 240, CHARACTER_Z));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 10);
	}
	void update() {}
};

EXPORT_ACTOR_CLASS(Character);

void setCharacterVisibility(bool val)
{
	setBlitzMugenAnimationVisibility(gCharacter->entity, val);
}

int nameToCharacterAnimNo(const std::string& name) {
	if (name == "boss") return 10;
	if (name == "giovanni") return 11;
	if (name == "scarecrow") return 12;
	if (name == "lion") return 13;
	return -1;
}



void changeCharacterAnimation(const std::string& name)
{
	auto newAnimNo = nameToCharacterAnimNo(name);
	if (newAnimNo == -1 && name != "empty") return;
	changeBlitzMugenAnimation(gCharacter->entity, newAnimNo);
}
