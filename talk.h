#pragma once

#include <prism/blitz.h>

ActorBlueprint getTalk();

enum class TalkReturnPoint {
	MENU,
	LOOK,
	TALK_SELECT
};

void startTalk(const std::string& talkName, TalkReturnPoint returnPoint);

int isTalkActive();