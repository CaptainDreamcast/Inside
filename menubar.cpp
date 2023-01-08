#include "menubar.h"

#include "gamescreen.h"

#include "lookbutton.h"
#include "talkbutton.h"
#include "flirtbutton.h"
#include "movebutton.h"
#include "look.h"
#include "character.h"
#include "talkselect.h"
#include "move.h"
#include "bg.h"

#define MENUBAR_Z 10

class MenuBar {
public:
	bool isActive = true;
	int entity;
	int selectedOption = 0;
	MenuBar() {
		entity = addBlitzEntity(Position(160, 240, MENUBAR_Z));
		addBlitzMugenAnimationComponentStatic(entity, getGameSprites(), 100, 0);

		instantiateActor(getLookButton());
		instantiateActor(getTalkButton());
		instantiateActor(getMoveButton());

		setLookButtonSelected();
	}

	void update() {
		if (!isActive) return;
		updateInput();
	}

	void updateInput() {
		if (hasPressedLeftFlank()) {
			selectedOption = (selectedOption + 3 - 1) % 3;
			while(!isButtonActive(selectedOption))	selectedOption = (selectedOption + 3 - 1) % 3;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setButtonActive();
		}
		
		if (hasPressedRightFlank()) {
			selectedOption = (selectedOption + 1) % 3;
			while (!isButtonActive(selectedOption)) selectedOption = (selectedOption + 1) % 3;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setButtonActive();
		}

		if (hasPressedAFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 1);
			chooseSelectedOption();
		}
	}

	void setButtonActive() {
		setLookButtonUnselected();
		setTalkButtonUnselected();
		setMoveButtonUnselected();

		if (selectedOption == 0) {
			setLookButtonSelected();
		}
		else if (selectedOption == 1) {
			setTalkButtonSelected();
		}
		else {
			setMoveButtonSelected();
		}
	}

	void chooseSelectedOption() {
		if (selectedOption == 0) {
			startLookScreen();
		}
		else if (selectedOption == 1) {
			startTalkSelect();
		}
		else if (selectedOption == 2) {
			startMove();
		}
	}

	int isButtonActive(int index) {
		if (index == 1) return isTalkButtonActive();
		else return true;
	}

	void setActive() {
		isActive = true;
		setBlitzMugenAnimationVisibility(entity, true);
		setLookButtonActive();
		if (getBGName() != "shed") {
			setTalkButtonActive();
		}
		setMoveButtonActive();
		setCharacterVisibility(true);
	}

	void setInactive() {
		isActive = false;
		setBlitzMugenAnimationVisibility(entity, false);
		setLookButtonInactive();
		setTalkButtonInactive();
		setMoveButtonInactive();
	}

	void startLookScreen() {
		setInactive();
		setLookActivity(true);
	}

	void startTalkSelect() {
		setMenuBarActivity(false);
		setTalkSelectActivity(true);
	}

	void startMove() {
		setMenuBarActivity(false);
		setMoveActivity(true);
	}
};

EXPORT_ACTOR_CLASS(MenuBar);

void setMenuBarActivity(bool val)
{
	if (val) {
		gMenuBar->setActive();
	}
	else {
		gMenuBar->setInactive();
	}
}
