#include "lookbutton.h"

#include <math.h>

#include "gamescreen.h"

#define BUTTON_Z 15

class LookButton {
public:
	int entity;
	double animTime = 0.0;
	bool isActive = true;

	LookButton() {
		entity = addBlitzEntity(Position(60, 217, BUTTON_Z));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 102);
	}
	void update() {
		if (!isActive) return;
		updateAnimation();
	}

	void updateAnimation() {
		auto animation = getBlitzMugenAnimationAnimationNumber(entity);
		if (animation != 112) return;

		setBlitzEntityScale2D(entity, 0.9 + 0.1 * sin(animTime));
		animTime += 0.3;

	}
};

EXPORT_ACTOR_CLASS(LookButton);

void setLookButtonSelected()
{
	changeBlitzMugenAnimation(gLookButton->entity, 112);
	gLookButton->animTime = 0.0;
	
}

void setLookButtonUnselected()
{
	changeBlitzMugenAnimation(gLookButton->entity, 102);
	setBlitzEntityScale2D(gLookButton->entity, 1.0);
}

void setLookButtonActive()
{
	setBlitzMugenAnimationVisibility(gLookButton->entity, true);
	gLookButton->isActive = true;
}

void setLookButtonInactive()
{
	setBlitzMugenAnimationVisibility(gLookButton->entity, false);
	gLookButton->isActive = false;
}
