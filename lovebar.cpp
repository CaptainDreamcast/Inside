#include "lovebar.h"

#include "gamescreen.h"

#define BAR_Z  2

class LoveBar {
public:
	int entityBG;
	int entityFG;
	bool paused = false;

	LoveBar() {
		entityBG = addBlitzEntity(Position(65, 5, BAR_Z));
		addBlitzMugenAnimationComponentStatic(entityBG, getGameSprites(), 101, 0);

		entityFG = addBlitzEntity(Position(65, 5, BAR_Z + 1));
		addBlitzMugenAnimationComponentStatic(entityFG, getGameSprites(), 101, 1);
	}
	void update() {
		if (paused) return;
		auto currentScale = getBlitzEntityScale(entityFG).x;
		currentScale = max(0.0, currentScale - (1.0 / (120 * 60.0)));
		setBlitzEntityScaleX(entityFG, currentScale);

	}

	void setPaused(bool paused) {
		setBlitzMugenAnimationVisibility(entityBG, !paused);
		setBlitzMugenAnimationVisibility(entityFG, !paused);
		this->paused = paused;
	}
};

EXPORT_ACTOR_CLASS(LoveBar);

void setLoveBarPaused(bool paused)
{
	if (!paused) return;
	gLoveBar->setPaused(paused);
}
