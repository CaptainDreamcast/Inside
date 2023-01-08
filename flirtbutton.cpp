#include "flirtbutton.h"

#include "gamescreen.h"

#define BUTTON_Z 15

class FlirtButton {
public:
	bool isActive = true;
	int entity;
	double animTime = 0.0;

	FlirtButton() {
		entity = addBlitzEntity(Position(260, 220, BUTTON_Z));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 104);
	}

	void update() {
		if (!isActive) return;
		updateAnimation();
	}

	void updateAnimation() {
		auto animation = getBlitzMugenAnimationAnimationNumber(entity);
		if (animation != 114) return;

		setBlitzEntityScale2D(entity, 0.9 + 0.1 * sin(animTime));
		animTime += 0.3;

	}
};

EXPORT_ACTOR_CLASS(FlirtButton);

void setFlirtButtonSelected()
{
	changeBlitzMugenAnimation(gFlirtButton->entity, 114);
	gFlirtButton->animTime = 0.0;

}

void setFlirtButtonUnselected()
{
	changeBlitzMugenAnimation(gFlirtButton->entity, 104);
	setBlitzEntityScale2D(gFlirtButton->entity, 1.0);
}

void setFlirtButtonActive()
{
	setBlitzMugenAnimationVisibility(gFlirtButton->entity, true);
	gFlirtButton->isActive = true;
}

void setFlirtButtonInactive()
{
	setBlitzMugenAnimationVisibility(gFlirtButton->entity, false);
	gFlirtButton->isActive = false;
}