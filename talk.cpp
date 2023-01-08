#include "talk.h"

#include "gamescreen.h"
#include "menubar.h"
#include "character.h"
#include "look.h"
#include "talkselect.h"
#include "lovebar.h"
#include "bg.h"
#include "finalscreen.h"

#define TEXTBOX_Z 10

void gotoFinaleCB(void*);

class TalkAction {
public:
	string name;
	string text;
	int questionCount;
};

class Talk {
public:
	bool isActive = true;
	int mTextBox;
	int mArrow;
	int mNameText;
	int mText;
	int mItem;
	int mQuestionAnswers[3];
	int mQuestionAnswerTexts[3];
	int currentSection = -1;
	bool isQuestionActive = false;
	int questionCount;
	int mSelectedAnswer = 0;
	string questionTargets[3];
	TalkReturnPoint returnPoint;

	std::map<string, std::vector<TalkAction>> mLoadedActions;

	std::vector<TalkAction> mTalkActions;

	Talk() {
		mTextBox = addBlitzEntity(Position(160, 235, TEXTBOX_Z));
		addBlitzMugenAnimationComponentStatic(mTextBox, getGameSprites(), 270, 0);

		mArrow = addBlitzEntity(Position(300, 230, TEXTBOX_Z + 2));
		addBlitzMugenAnimationComponentStatic(mArrow, getGameSprites(), 271, 0);

		mNameText = addMugenTextMugenStyle("msdqwdqewqweqweqwe", Position(16, 174, TEXTBOX_Z + 1), Vector3DI(1, 0, 1));
		mText = addMugenTextMugenStyle("msdqwdqewqweqweqwe", Position(15, 190, TEXTBOX_Z + 1), Vector3DI(-1, 7, 1));
		setMugenTextTextBoxWidth(mText, 290);

		mItem = addBlitzEntity(Position(160, 100, TEXTBOX_Z + 2));
		addBlitzMugenAnimationComponent(mItem, getGameSprites(), getGameAnimations(), -1);

		mQuestionAnswers[0] = addBlitzEntity(Position(160, 40, TEXTBOX_Z));
		addBlitzMugenAnimationComponent(mQuestionAnswers[0], getGameSprites(), getGameAnimations(), 272);

		mQuestionAnswers[1] = addBlitzEntity(Position(160, 80, TEXTBOX_Z));
		addBlitzMugenAnimationComponent(mQuestionAnswers[1], getGameSprites(), getGameAnimations(), 272);

		mQuestionAnswers[2] = addBlitzEntity(Position(160, 120, TEXTBOX_Z));
		addBlitzMugenAnimationComponent(mQuestionAnswers[2], getGameSprites(), getGameAnimations(), 272);

		mQuestionAnswerTexts[0] = addMugenTextMugenStyle("msdqwdqewqweqweqwe", Position(160, 60, TEXTBOX_Z + 1), Vector3DI(1, 0, 0));
		mQuestionAnswerTexts[1] = addMugenTextMugenStyle("msdqwdqewqweqweqwe", Position(160, 100, TEXTBOX_Z + 1), Vector3DI(1, 0, 0));
		mQuestionAnswerTexts[2] = addMugenTextMugenStyle("msdqwdqewqweqweqwe", Position(160, 140, TEXTBOX_Z + 1), Vector3DI(1, 0, 0));
		
		loadAllTalkActionsFromFile();
		setInactive();
	}
	void update() {
		if (!isActive) return;
		updateInput();
	}

	void updateInput() {
		if (isQuestionActive)
		{
			if (hasPressedUpFlank()) {
				tryPlayMugenSound(getGameSounds(), 1, 2);
				mSelectedAnswer = (mSelectedAnswer - 1 + questionCount) % questionCount;
				setSelectedAnswer();
			}
			if (hasPressedDownFlank()) {
				tryPlayMugenSound(getGameSounds(), 1, 2);
				mSelectedAnswer = (mSelectedAnswer + 1) % questionCount;
				setSelectedAnswer();
			}

			if (hasPressedAFlank()) {
				tryPlayMugenSound(getGameSounds(), 1, 0);
				setInactive();
				startTalk(questionTargets[mSelectedAnswer], returnPoint);
			}
		}
		else {
			if (isMugenTextBuiltUp(mText))
			{
				setBlitzMugenAnimationVisibility(mArrow, true);
			}

			if (hasPressedAFlank() || hasPressedStartFlank() || hasPressedX()) {
				if (hasPressedAFlank()) {
					tryPlayMugenSound(getGameSounds(), 1, 0);
				}
				if (isMugenTextBuiltUp(mText)) {
					startNextSection();
				}
				else {
					setMugenTextBuiltUp(mText);
				}
			}
		}
	}

	void setInactive()
	{
		setBlitzMugenAnimationVisibility(mTextBox, false);
		setBlitzMugenAnimationVisibility(mItem, false);
		setBlitzMugenAnimationVisibility(mArrow, false);
		setMugenTextVisibility(mNameText, false);
		setMugenTextVisibility(mText, false);
		for (int i = 0; i < 3; i++) setBlitzMugenAnimationVisibility(mQuestionAnswers[i], false);
		for (int i = 0; i < 3; i++) setMugenTextVisibility(mQuestionAnswerTexts[i], false);
		setLoveBarPaused(false);
		isActive = false;
	}

	void start(const std::string& talkName, TalkReturnPoint returnPoint)
	{
		auto name = talkName;
		turnStringLowercase(name);

		mTalkActions = mLoadedActions[name];
		
		setLoveBarPaused(true);
		setBlitzMugenAnimationVisibility(mItem, false);
		isQuestionActive = false;
		this->returnPoint = returnPoint;
		currentSection = -1;
		isActive = true;
		startNextSection();
	}

	void loadAllTalkActionsFromFile() {
		MugenDefScript script;
		loadMugenDefScript(&script, "talk/all.def");

		MugenDefScriptGroup* group = script.mFirstGroup;
		while (group)
		{
			mLoadedActions[group->mName] = loadTalkActionsFromGroup(group);
			group = group->mNext;
		}

		unloadMugenDefScript(&script);
	}

	std::vector<TalkAction> loadTalkActionsFromGroup(MugenDefScriptGroup* group) {
		std::vector<TalkAction> ret;
		auto iterator = list_iterator_begin(&group->mOrderedElementList);
		while (iterator)
		{
			TalkAction action;
			MugenDefScriptGroupElement* element = (MugenDefScriptGroupElement*)list_iterator_get(iterator);

			if (element->mName == "globalvar")
			{
				action.name = element->mName;
				action.text = getSTLMugenDefStringVariableAsElement(element);
				action.questionCount = 0;
			}
			else if (element->mName == "item")
			{
				action.name = element->mName;
				action.text = getSTLMugenDefStringVariableAsElement(element);
				action.questionCount = 0;
			}
			else if (element->mName == "question")
			{
				action.name = element->mName;
				action.text = "";
				action.questionCount = getMugenDefNumberVariableAsElement(element);
			}
			else if (element->mName == "soundeffect")
			{
				action.name = element->mName;
				action.text = "";
				action.questionCount = getMugenDefNumberVariableAsElement(element);
			}
			else {
				action.name = element->mName;
				action.text = getSTLMugenDefStringVariableAsElement(element);
				action.questionCount = 0;
			}
			ret.push_back(action);

			if (!list_has_next(iterator))
			{
				iterator = nullptr;
			}
			else {
				list_iterator_increase(&iterator);
			}
		}
		return ret;
	}

	int itemNameToAnimNo(const std::string& name) {
		if (name == "dummy") return 300;
		if (name == "milk") return 301;
		if (name == "eggs") return 302;
		if (name == "feed") return 303;
		if (name == "cake") return 304;
		if (name == "carrot") return 305;
		if (name == "key") return 306;
		if (name == "gun") return 307;
		if (name == "alien") return 308;
		if (name == "grain") return 309;
		if (name == "card") return 310;
		return -1;
	}

	void startNextSection() {
		currentSection += 1;
		if (currentSection >= int(mTalkActions.size())) {
			setInactive();
			if (returnPoint == TalkReturnPoint::MENU) {
				setMenuBarActivity(true);
			}
			else if (returnPoint == TalkReturnPoint::LOOK) {
				setLookActivity(true);
			}
			else if (returnPoint == TalkReturnPoint::TALK_SELECT) {
				setTalkSelectActivity(true);
			}
			else {
				setMenuBarActivity(true);
			}
			return;
		}

		if (mTalkActions[currentSection].name == "globalvar") {
			setGlobalVariable(mTalkActions[currentSection].text);
			startNextSection();
			return;
		}

		if (mTalkActions[currentSection].name == "soundeffect") {
			int id = mTalkActions[currentSection].questionCount;
			tryPlayMugenSound(getGameSounds(), 2, id);
			startNextSection();
			return;
		}

		if (mTalkActions[currentSection].name == "music") {
			if (mTalkActions[currentSection].text == "none") {
				stopStreamingMusicFile();
			}
			else {
				streamMusicFile(("tracks/" + mTalkActions[currentSection].text + ".ogg").c_str());
			}
			startNextSection();
			return;
		}

		if (mTalkActions[currentSection].name == "item") {
			changeBlitzMugenAnimation(mItem, itemNameToAnimNo(mTalkActions[currentSection].text));
			setBlitzMugenAnimationVisibility(mItem, true);
			startNextSection();
			return;
		}

		if (mTalkActions[currentSection].name == "goto") {
			setInactive();
			setBG(mTalkActions[currentSection].text);
			if (!isTalkActive()) setMenuBarActivity(true);
			return;
		}

		if (mTalkActions[currentSection].name == "talk") {
			setInactive();
			startTalk(mTalkActions[currentSection].text, returnPoint);
			return;
		}

		if (mTalkActions[currentSection].name == "forcelook") {
			setInactive();
			forceLooking();
			setLookActivity(true);
			return;
		}

		if (mTalkActions[currentSection].name == "finale") {
			setInactive();
			addFadeOut(20, gotoFinaleCB);
			return;
		}

		if (mTalkActions[currentSection].name == "question") {
			questionCount = mTalkActions[currentSection].questionCount;
			currentSection += 1;
			auto& questionTextTalkAction = mTalkActions[currentSection];
			setTalkActionToText(questionTextTalkAction);
			for (int i = 0; i < questionCount; i++) {
				currentSection += 1;
				questionTargets[i] = mTalkActions[currentSection].name;
				setBlitzMugenAnimationVisibility(mQuestionAnswers[i], true);
				setMugenTextVisibility(mQuestionAnswerTexts[i], true);
				changeMugenText(mQuestionAnswerTexts[i], mTalkActions[currentSection].text.c_str());
			}
			setBlitzMugenAnimationVisibility(mArrow, false);
			mSelectedAnswer = 0;
			setSelectedAnswer();
			isQuestionActive = true;
			return;
		}

		setBlitzMugenAnimationVisibility(mArrow, false);
		for (int i = 0; i < 3; i++) setBlitzMugenAnimationVisibility(mQuestionAnswers[i], false);
		for (int i = 0; i < 3; i++) setMugenTextVisibility(mQuestionAnswerTexts[i], false);
		setTalkActionToText(mTalkActions[currentSection]);
	}

	void setSelectedAnswer() {
		for (int i = 0; i < 3; i++) changeBlitzMugenAnimation(mQuestionAnswers[i], 272);
		for (int i = 0; i < 3; i++) setMugenTextColor(mQuestionAnswerTexts[i], COLOR_WHITE);
		changeBlitzMugenAnimation(mQuestionAnswers[mSelectedAnswer], 273);
		setMugenTextColor(mQuestionAnswerTexts[mSelectedAnswer], COLOR_RED);
	}

	void setTalkActionToText(const TalkAction& talkAction) 
	{
		setBlitzMugenAnimationVisibility(mTextBox, true);
		setBlitzMugenAnimationVisibility(mArrow, false);
		changeCharacterAnimation(talkAction.name);
		auto name = talkAction.name;
		name[0] = toupper(name[0]);
		name.push_back(':');
		changeMugenText(mNameText, name.c_str());
		setMugenTextVisibility(mNameText, true);
		setMugenTextVisibility(mText, true);
		changeMugenText(mText, talkAction.text.c_str());
		setMugenTextBuildup(mText, 1);
	}
};

EXPORT_ACTOR_CLASS(Talk);

void startTalk(const std::string& talkName, TalkReturnPoint returnPoint)
{
	gTalk->start(talkName, returnPoint);
}

int isTalkActive()
{
	return gTalk->isActive;
}

void gotoFinaleCB(void*) {
	setNewScreen(getFinalScreen());
}