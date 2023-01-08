#include "gamescreen.h"

#include "bg.h"
#include "character.h"
#include "lovebar.h"
#include "menubar.h"
#include "look.h"
#include "talkselect.h"
#include "move.h"
#include "talk.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;

	std::map<std::string, bool> mGlobalVars;
} gGameScreenData;

class GameScreen {
public:
	GameScreen() {
		gGameScreenData.mSprites = loadMugenSpriteFileWithoutPalette("game/GAME.sff");
		gGameScreenData.mAnimations = loadMugenAnimationFile("game/GAME.air");
		gGameScreenData.mSounds = loadMugenSoundFile("game/GAME.snd");
		gGameScreenData.mGlobalVars.clear();
		stopStreamingMusicFile();

		instantiateActor(getBG());
		instantiateActor(getCharacter());
		instantiateActor(getLoveBar());
		instantiateActor(getMenuBar());
		instantiateActor(getLook());
		instantiateActor(getTalkSelect());
		instantiateActor(getMove());
		instantiateActor(getTalk());

		setMenuBarActivity(false);
		setLoveBarPaused(true);
		setBG("house");

		addFadeIn(20, NULL);
	}
	void update()
	{
	}
};

EXPORT_SCREEN_CLASS(GameScreen);

MugenSpriteFile* getGameSprites()
{
	return &gGameScreenData.mSprites;
}

MugenAnimations* getGameAnimations()
{
	return &gGameScreenData.mAnimations;
}

MugenSounds* getGameSounds()
{
	return &gGameScreenData.mSounds;
}

bool getGlobalVariable(const std::string& name)
{
	string lowerName = name;
	turnStringLowercase(lowerName);
	return gGameScreenData.mGlobalVars[lowerName];
}

void setGlobalVariable(const std::string& name)
{
	string lowerName = name;
	turnStringLowercase(lowerName);
	gGameScreenData.mGlobalVars[lowerName] = true;
}
