#include "movebutton.h"

#include <math.h>

#include "gamescreen.h"

#define BUTTON_Z 15

class MoveButton {
public:
	int entity;
	double animTime = 0.0;
	bool isActive = true;

	MoveButton() {
		entity = addBlitzEntity(Position(260, 217, BUTTON_Z));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 105);
	}
	void update() {
		if (!isActive) return;
		updateAnimation();
	}

	void updateAnimation() {
		auto animation = getBlitzMugenAnimationAnimationNumber(entity);
		if (animation != 115) return;

		setBlitzEntityScale2D(entity, 0.9 + 0.1 * sin(animTime));
		animTime += 0.3;

	}
};

EXPORT_ACTOR_CLASS(MoveButton);

void setMoveButtonSelected()
{
	changeBlitzMugenAnimation(gMoveButton->entity, 115);
	gMoveButton->animTime = 0.0;
	
}

void setMoveButtonUnselected()
{
	changeBlitzMugenAnimation(gMoveButton->entity, 105);
	setBlitzEntityScale2D(gMoveButton->entity, 1.0);
}

void setMoveButtonActive()
{
	setBlitzMugenAnimationVisibility(gMoveButton->entity, true);
	gMoveButton->isActive = true;
}

void setMoveButtonInactive()
{
	setBlitzMugenAnimationVisibility(gMoveButton->entity, false);
	gMoveButton->isActive = false;
}
