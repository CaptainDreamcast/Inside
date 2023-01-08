#include "talkselect.h"

#include "gamescreen.h"
#include "bg.h"
#include "menubar.h"
#include "talk.h"

#define MENU_Z 10

class SingleTalk {
public:
	string name;
	string talk;
	string active;
	string disabled;
	string removed;
};

class TalkSelect {
public:
	bool isActive = false;
	int menubg;
	int arrowDown;
	int arrowUp;
	int scrollbarBG;
	int scrollbarFG;
	int mMenuBGs[3];
	int mMenuTexts[3];
	int talkOffset = 0;
	int selectedTalk = 0;
	int frameSentinel = 0;

	map<string, std::vector<SingleTalk>> loadedTalks;
	std::vector<SingleTalk> activeTalks;

	TalkSelect() {
		menubg = addBlitzEntity(Position(240, 80, MENU_Z));
		addBlitzMugenAnimationComponentStatic(menubg, getGameSprites(), 200, 0);

		for (int i = 0; i < 3; i++) {
			mMenuBGs[i] = addBlitzEntity(Position(240, 80 + 49 * i, MENU_Z + 1));
			addBlitzMugenAnimationComponent(mMenuBGs[i], getGameSprites(), getGameAnimations(), 201);

			mMenuTexts[i] = addMugenTextMugenStyle("ayyyo", Vector3D(240, 80 + 49 * i + 30, MENU_Z + 2), Vector3DI(1, 0, 0));
		}

		arrowDown = addBlitzEntity(Position(240, 230, MENU_Z + 3));
		addBlitzMugenAnimationComponentStatic(arrowDown, getGameSprites(), 204, 0);

		arrowUp = addBlitzEntity(Position(240, 80, MENU_Z + 3));
		addBlitzMugenAnimationComponentStatic(arrowUp, getGameSprites(), 205, 0);

		scrollbarBG = addBlitzEntity(Position(303, 82, MENU_Z + 4));
		addBlitzMugenAnimationComponentStatic(scrollbarBG, getGameSprites(), 202, 0);

		scrollbarFG = addBlitzEntity(Position(303, 82, MENU_Z + 5));
		addBlitzMugenAnimationComponentStatic(scrollbarFG, getGameSprites(), 203, 0);

		loadTalks();
		setActivity(false);
	}

	void loadTalks() {
		loadSingleTalk("field");
		loadSingleTalk("house");
		loadSingleTalk("barn");
	}

	void loadSingleTalk(const std::string& name) {
		auto path = "click/" + name + "_talk.def";
		MugenDefScript script;
		loadMugenDefScript(&script, path.c_str());

		auto& talks = loadedTalks[name];
		auto group = script.mFirstGroup;
		while (group) {
			talks.push_back(loadTalkFromGroup(group));
			group = group->mNext;
		}
		unloadMugenDefScript(&script);

	}

	SingleTalk loadTalkFromGroup(MugenDefScriptGroup* group)
	{
		SingleTalk ret;
		ret.name = getSTLMugenDefStringOrDefaultAsGroup(group, "name", "");
		ret.talk = getSTLMugenDefStringOrDefaultAsGroup(group, "talk", "");
		ret.active = getSTLMugenDefStringOrDefaultAsGroup(group, "active", "");
		ret.disabled = getSTLMugenDefStringOrDefaultAsGroup(group, "disabled", "");
		ret.removed = getSTLMugenDefStringOrDefaultAsGroup(group, "removed", "");
		return ret;
	}

	void update() {
		if (!isActive) return;
		updateInput();
		frameSentinel++;
	}

	void updateInput() {
		if (hasPressedUpFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 2);
			moveUp();
		}
		if (hasPressedDownFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 2);
			moveDown();
		}
		if (hasPressedBFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 3);
			returnToMenu();
		}
		if (hasPressedAFlank() && frameSentinel > 0) {
			tryPlayMugenSound(getGameSounds(), 1, 1);
			startSelectedTalk();
		}
	}

	void startSelectedTalk() {
		startTalk(activeTalks[talkOffset + selectedTalk].talk, TalkReturnPoint::TALK_SELECT);
		setActivity(false);
	}

	void returnToMenu() {
		setActivity(false);
		setMenuBarActivity(true);
	}

	void moveUp() {
		if (selectedTalk == 0) {
			if (talkOffset > 0) {
				talkOffset -= 1;
			}
			else {
				selectedTalk = min(2, int(activeTalks.size()) - 1);
				talkOffset = max(0, int(activeTalks.size()) - 3);
			}
		}
		else {
			selectedTalk -= 1;
		}
		updateVisibleTalks();
	}

	void moveDown() {
		if (selectedTalk == 2 || selectedTalk == int(activeTalks.size()) - 1) {
			if (talkOffset < int(activeTalks.size())- 3) {
				talkOffset += 1;
			}
			else {
				selectedTalk = 0;
				talkOffset = 0;
			}
		}
		else {
			selectedTalk += 1;
		}
		updateVisibleTalks();
	}

	void resetVisibleTalks() {
		talkOffset = 0;
		selectedTalk = 0;

		auto bgName = getBGName();
		auto allTalks = loadedTalks[bgName];
		activeTalks = filterTalksByGlobalVars(allTalks);
		setScrollingActivity(activeTalks.size() > 3);

	}
	void updateVisibleTalks() 
	{
		
		setBlitzMugenAnimationVisibility(arrowUp, (talkOffset > 0));
		setBlitzMugenAnimationVisibility(arrowDown, ((talkOffset + 2) < (int(activeTalks.size()) - 1)));

		for (int i = 0; i < 3; i++) {
			setBlitzMugenAnimationVisibility(mMenuBGs[i], false);
			setMugenTextVisibility(mMenuTexts[i], false);
		}

		for (size_t i = 0u; i < min(size_t(3u), activeTalks.size()); i++)
		{
			setBlitzMugenAnimationVisibility(mMenuBGs[i], true);
			changeBlitzMugenAnimation(mMenuBGs[i], 201);
			setMugenTextVisibility(mMenuTexts[i], true);
			changeMugenText(mMenuTexts[i], activeTalks[talkOffset + i].name.c_str());
			setMugenTextColor(mMenuTexts[i], COLOR_WHITE);
		}

		int scrollbarStartY = 82;
		int scrollbarSizeY = 98;
		double scrollbarT = talkOffset / double(activeTalks.size() - 3);
		setBlitzEntityPositionY(scrollbarFG, scrollbarStartY + scrollbarSizeY * scrollbarT);

		changeBlitzMugenAnimation(mMenuBGs[selectedTalk], 202);
		if (activeTalks[talkOffset + selectedTalk].disabled != "" && getGlobalVariable(activeTalks[talkOffset + selectedTalk].disabled))
		{
			setMugenTextColor(mMenuTexts[selectedTalk], COLOR_GRAY);
		}
		else
		{
			setMugenTextColor(mMenuTexts[selectedTalk], COLOR_RED);
		}
	}

	void setScrollingActivity(bool val)
	{
		setBlitzMugenAnimationVisibility(scrollbarBG, val);
		setBlitzMugenAnimationVisibility(scrollbarFG, val);
	}

	std::vector<SingleTalk> filterTalksByGlobalVars(const std::vector<SingleTalk>& allTalks) {
		std::vector<SingleTalk> ret;
		for (auto& talk : allTalks) {
			if (talk.active != "" && !getGlobalVariable(talk.active)) continue;
			if (talk.removed != "" && getGlobalVariable(talk.removed)) continue;
			ret.push_back(talk);
		}
		return ret;
	}

	void setActivity(bool val) {
		frameSentinel = 0;
		isActive = val;
		setBlitzMugenAnimationVisibility(menubg, val);
		setBlitzMugenAnimationVisibility(arrowDown, val);
		setBlitzMugenAnimationVisibility(arrowUp, val);
		setBlitzMugenAnimationVisibility(scrollbarBG, val);
		setBlitzMugenAnimationVisibility(scrollbarFG, val);
		for (int i = 0; i < 3; i++) setBlitzMugenAnimationVisibility(mMenuBGs[i], val);
		for (int i = 0; i < 3; i++) setMugenTextVisibility(mMenuTexts[i], val);

		if (val)
		{
			resetVisibleTalks();
			updateVisibleTalks();
		}
	}
};

EXPORT_ACTOR_CLASS(TalkSelect);

void setTalkSelectActivity(bool val)
{
	gTalkSelect->setActivity(val);
}
