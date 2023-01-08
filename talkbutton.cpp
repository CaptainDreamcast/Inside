#include "talkbutton.h"

#include "gamescreen.h"

#define BUTTON_Z 15

class TalkButton {
public:
	bool isActive = true;
	int entity;
	double animTime = 0.0;
	TalkButton() {
		entity = addBlitzEntity(Position(160, 217, BUTTON_Z));
		addBlitzMugenAnimationComponent(entity, getGameSprites(), getGameAnimations(), 103);
	}
	void update() {
		if (!isActive) return;
		updateAnimation();
	}

	void updateAnimation() {
		auto animation = getBlitzMugenAnimationAnimationNumber(entity);
		if (animation != 113) return;

		setBlitzEntityScale2D(entity, 0.9 + 0.1 * sin(animTime));
		animTime += 0.3;

	}
};

EXPORT_ACTOR_CLASS(TalkButton);

void setTalkButtonSelected()
{
	changeBlitzMugenAnimation(gTalkButton->entity, 113);
	gTalkButton->animTime = 0.0;

}

void setTalkButtonUnselected()
{
	changeBlitzMugenAnimation(gTalkButton->entity, 103);
	setBlitzEntityScale2D(gTalkButton->entity, 1.0);
}

void setTalkButtonActive()
{
	setBlitzMugenAnimationVisibility(gTalkButton->entity, true);
	gTalkButton->isActive = true;
}

void setTalkButtonInactive()
{
	setBlitzMugenAnimationVisibility(gTalkButton->entity, false);
	gTalkButton->isActive = false;
}

bool isTalkButtonActive()
{
	return gTalkButton->isActive;
}