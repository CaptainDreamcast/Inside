#include "bg.h"

#include "gamescreen.h"
#include "talk.h"
#include "character.h"
#include "menubar.h"

class BG {
public:
	int entity;
	std::string name = "field";
	std::set<string> hasVisited;

	BG() {
		entity = addBlitzEntity(Position(0, 0, 1));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 1);
	}

	std::string getName() {
		return name;
	}

	int nameToAnimNo(const std::string& name) {
		if (name == "field") return 1;
		if (name == "house") return 2;
		if (name == "barn") return 3;
		if (name == "shed") return 4;
		if (name == "lab") return 5;
		return 1;
	}

	void setName(const std::string& name) {
		this->name = name;

		changeBlitzMugenAnimation(entity, nameToAnimNo(name));

		if (!stl_set_contains(hasVisited, name)) {

			stopStreamingMusicFile();
			changeCharacterAnimation("empty");
			setMenuBarActivity(false);
			startTalk(name + "_opening", TalkReturnPoint::MENU);
			hasVisited.insert(name);
		}
		else {
			if (name == "field") {
				changeCharacterAnimation("scarecrow");
				streamMusicFile("tracks/scarecrow.ogg");
			}
			if (name == "house") {
				changeCharacterAnimation("giovanni");
				streamMusicFile("tracks/giovanni.ogg");
			}
			if (name == "barn") {
				changeCharacterAnimation("lion");
				streamMusicFile("tracks/lion.ogg");
			}
			if (name == "shed") {
				changeCharacterAnimation("boss");
				stopStreamingMusicFile();
			}
			if (name == "lab") {
				stopStreamingMusicFile();
				changeCharacterAnimation("boss");
			}
		}
	}

	void update() {}
};

EXPORT_ACTOR_CLASS(BG);

std::string getBGName()
{
	return gBG->getName();
}

void setBG(const std::string& name)
{
	gBG->setName(name);
}