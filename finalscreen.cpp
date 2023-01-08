#include "finalscreen.h"


static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;
} gFinalScreenData;

void gotoTitleCB(void*);

class FinalScreen {
public:
	int stage = 0;
	int entity;
	FinalScreen() {
		gFinalScreenData.mSprites = loadMugenSpriteFileWithoutPalette("final/FINAL.sff");
		gFinalScreenData.mAnimations = loadMugenAnimationFile("final/FINAL.air");
		gFinalScreenData.mSounds = loadMugenSoundFile("game/GAME.snd");

		entity = addBlitzEntity(Vector3D(0, 0, 1));
		addBlitzMugenAnimationComponent(entity, &gFinalScreenData.mSprites, &gFinalScreenData.mAnimations, 1);

		stopStreamingMusicFile();
		streamMusicFile("tracks/boss.ogg");
		addFadeIn(20, NULL);
	}
	void update()
	{
		if (stage > 0) return;
		if (hasPressedAFlank() || hasPressedStartFlank()) {
			tryPlayMugenSound(&gFinalScreenData.mSounds, 1, 1);
			addFadeOut(20, gotoTitleCB);
		}
	}
};

EXPORT_SCREEN_CLASS(FinalScreen);

void gotoTitleCB(void*) {
	gFinalScreen->stage++;
	changeBlitzMugenAnimation(gFinalScreen->entity, 2);
	unsetScreenBlack();
	addFadeIn(20, NULL);
}