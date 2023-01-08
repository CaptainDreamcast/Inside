#include "move.h"

#include "gamescreen.h"
#include "menubar.h"
#include "bg.h"
#include "talk.h"

#define BG_Z 10
#define BUTTON_Z 12

void moveFadeOutFinishedCB(void*);

class Move {
public:
	bool isActive = true;
	bool isFading = false;
	int bg;
	int buttons[4];
	int selectedOption = 0;
	int frameSentinel = 0;

	Move() {
		bg = addBlitzEntity(Position(0, 240, BG_Z));
		addBlitzMugenAnimationComponentStatic(bg, getGameSprites(), 250, 0);

		buttons[0] = addBlitzEntity(Position(4, 157, BUTTON_Z));
		addBlitzMugenAnimationComponentStatic(buttons[0], getGameSprites(), 251, 0);

		buttons[1] = addBlitzEntity(Position(161, 157, BUTTON_Z));
		addBlitzMugenAnimationComponentStatic(buttons[1], getGameSprites(), 251, 1);
		
		buttons[2] = addBlitzEntity(Position(4, 197, BUTTON_Z));
		addBlitzMugenAnimationComponentStatic(buttons[2], getGameSprites(), 251, 2);

		buttons[3] = addBlitzEntity(Position(161, 197, BUTTON_Z));
		addBlitzMugenAnimationComponentStatic(buttons[3], getGameSprites(), 251, 3);

		setActivity(false);
	}
	void update() {
		if (!isActive || isFading) return;
		updateInput();
		frameSentinel++;
	}

	void updateInput() {
		if (hasPressedLeftFlank()) {
			if (selectedOption == 0) selectedOption = 1;
			else if (selectedOption == 1) selectedOption = 0;
			else if (selectedOption == 2) selectedOption = 3;
			else if (selectedOption == 3) selectedOption = 2;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setSelectedOption();
		}
		if (hasPressedRightFlank()) {
			if (selectedOption == 0) selectedOption = 1;
			else if (selectedOption == 1) selectedOption = 0;
			else if (selectedOption == 2) selectedOption = 3;
			else if (selectedOption == 3) selectedOption = 2;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setSelectedOption();
		}
		if (hasPressedUpFlank()) {
			if (selectedOption == 0) selectedOption = 2;
			else if (selectedOption == 1) selectedOption = 3;
			else if (selectedOption == 2) selectedOption = 0;
			else if (selectedOption == 3) selectedOption = 1;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setSelectedOption();
		}
		if (hasPressedDownFlank()) {
			if (selectedOption == 0) selectedOption = 2;
			else if (selectedOption == 1) selectedOption = 3;
			else if (selectedOption == 2) selectedOption = 0;
			else if (selectedOption == 3) selectedOption = 1;
			tryPlayMugenSound(getGameSounds(), 1, 2);
			setSelectedOption();
		}
		if (hasPressedBFlank()) {
			tryPlayMugenSound(getGameSounds(), 1, 3);
			setActivity(false);
			setMenuBarActivity(true);
		}

		if (hasPressedAFlank() && frameSentinel > 0) {
			tryPlayMugenSound(getGameSounds(), 1, 1);
			addFadeOut(20, moveFadeOutFinishedCB);
			isFading = true;
		}
	}

	void setSelectedOption() {
		for (int i = 0; i < 4; i++) setBlitzMugenAnimationVisibility(buttons[i], false);
		setBlitzMugenAnimationVisibility(buttons[selectedOption], true);
	}

	void setActivity(bool val) {
		isActive = val;
		setBlitzMugenAnimationVisibility(bg, val);
		for (int i = 0; i < 4; i++) setBlitzMugenAnimationVisibility(buttons[i], val);
		if (val) {
			setSelectedOption();
		}
		frameSentinel = 0;
	}

	void updateBG() {
		string name = "";
		if (selectedOption == 0) name = "field";
		else if (selectedOption == 1) name = "house";
		else if (selectedOption == 2) name = "barn";
		else name = "shed";
		setBG(name);
	}
};

EXPORT_ACTOR_CLASS(Move);

void setMoveActivity(bool val)
{
	gMove->setActivity(val);
}

void moveFadeOutFinishedCB(void*)
{
	gMove->updateBG();
	if(!isTalkActive()) setMenuBarActivity(true);
	gMove->setActivity(false);
	unsetScreenBlack();
	gMove->isFading = false;
	addFadeIn(20, NULL);
}