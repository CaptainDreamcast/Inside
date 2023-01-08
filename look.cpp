#include "look.h"

#include "gamescreen.h"
#include "character.h"
#include "menubar.h"
#include "bg.h"
#include "talk.h"

#define RECTANGLE_Z 2

class LookRectangle {
public:
	Vector2D pos;
	Vector2D size;
	string talkName;
	std::vector<std::pair<string, bool>> globalVariableDependencies;
};

class Look {
public:
	bool isActive = false;
	int rectangle;
	int frameSentinel = 0;
	bool isForced = false;

	map<string, std::vector<LookRectangle>> loadedRectangles;

	Look() {
		rectangle = addBlitzEntity(Position(160, 120, RECTANGLE_Z));
		addBlitzMugenAnimationComponent(rectangle, getGameSprites(), getGameAnimations(), 130);
		setBlitzMugenAnimationVisibility(rectangle, isActive);
		loadLookRectangles();
	}

	void loadLookRectangles() {
		loadSingleLookRectangle("field");
		loadSingleLookRectangle("house");
		loadSingleLookRectangle("barn");
		loadSingleLookRectangle("shed");
		loadSingleLookRectangle("lab");
	}

	void loadSingleLookRectangle(const std::string& name) {
		auto path = "click/" + name + "_click.def";
		MugenDefScript script;
		loadMugenDefScript(&script, path.c_str());

		std::vector<LookRectangle>& lookRectangles = loadedRectangles[name];
		auto group = script.mFirstGroup;
		while (group) {
			lookRectangles.push_back(loadClickRectangleFromGroup(group));
			group = group->mNext;
		}
		unloadMugenDefScript(&script);
	}

	LookRectangle loadClickRectangleFromGroup(MugenDefScriptGroup* group)
	{
		LookRectangle ret;
		ret.pos = getMugenDefVector2DOrDefaultAsGroup(group, "pos", Vector2D(0, 0));
		ret.size = getMugenDefVector2DOrDefaultAsGroup(group, "size", Vector2D(0, 0));
		ret.talkName = getSTLMugenDefStringOrDefaultAsGroup(group, "talk", "");

		for (auto element : group->mElements) {
			if (element.first == "pos" || element.first == "size" || element.first == "talk") continue;
			ret.globalVariableDependencies.push_back(make_pair(element.first, getMugenDefNumberVariableAsElement(&element.second)));
		}
		return ret;
	}

	void update() {
		if (!isActive) return;
		updateInput();
		frameSentinel++;
	}

	void updateInput() {
		updateSelection();
		updateRectangleMovement();
		updateReturn();
	}

	void updateRectangleMovement() {
		auto speed = 5;
		Vector2D delta = Vector2D(0.0, 0.0);
		if (hasPressedLeft()) {
			delta.x -= speed;
		}
		if (hasPressedRight()) {
			delta.x += speed;
		}
		if (hasPressedUp()) {
			delta.y -= speed;
		}
		if (hasPressedDown()) {
			delta.y += speed;
		}
		auto pos = getBlitzEntityPosition(rectangle) + delta;
		pos.x = clamp(pos.x, 0.0, 320.0);
		pos.y = clamp(pos.y, 0.0, 240.0);
		setBlitzEntityPosition(rectangle, pos);
	}

	void updateSelection() {
		if (hasPressedAFlank() && frameSentinel > 0) {
			tryPlayMugenSound(getGameSounds(), 1, 1);
			selectAtPosition();
		}
	}

	void selectAtPosition() {
		auto& rectangles = loadedRectangles[getBGName()];
		auto pos = getBlitzEntityPosition(rectangle);
		for (int i = rectangles.size() - 1; i >= 0; i--) {
			auto& rectangle = rectangles[i];
			if (pos.x < rectangle.pos.x || pos.x > rectangle.pos.x + rectangle.size.x) continue;
			if (pos.y < rectangle.pos.y || pos.y > rectangle.pos.y + rectangle.size.y) continue;
			auto globalVars = true;
			for (auto globalVar : rectangle.globalVariableDependencies) {
				if (getGlobalVariable(globalVar.first) != globalVar.second)
				{
					globalVars = false;
					break;
				}
			}
			if (!globalVars) continue;
			startTalk(rectangle.talkName, TalkReturnPoint::LOOK);
			setLookActivity(false);
			break;
		}
	}

	void updateReturn() {
		if (hasPressedBFlank() && !isForced) {
			tryPlayMugenSound(getGameSounds(), 1, 3);
			setMenuBarActivity(true);
			setLookActivity(false);
		}
	}
};

EXPORT_ACTOR_CLASS(Look);

void setLookActivity(bool val)
{
	gLook->isActive = val;
	gLook->frameSentinel = 0;
	setBlitzMugenAnimationVisibility(gLook->rectangle, val);
	setCharacterVisibility(!val);
}

void forceLooking()
{
	gLook->isForced = true;
}
