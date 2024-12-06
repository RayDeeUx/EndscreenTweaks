#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:

	bool calledAlready = false;

	std::vector<std::string> quotes;
	std::vector<std::string> customQuotes;
	std::vector<std::string> wETMigration;
	std::vector<std::string> levelCompleteQuotes;
	std::vector<std::string> customLevelCompleteQuotes;

	std::vector<std::string> customLevelCompleteSprites;
	std::vector<std::string> knownCLCTModesBesidesCombined = {"Images", "Oxygene One"};

	std::string fallbackString = "We've got too many players to congratulate on level completions. Beat this level again for an actual message.";

	int fps = -1;
	int64_t originalOpacity = 0;
	int lastFlukedPercent = 0;

	bool isCompactEndscreen;
	bool isGDMO;
	bool isLevelComplete = false;

	float compactEndscreenFallbackPosition = CCDirector::get()->getWinSize().width * 0.6f;

	CCLabelBMFont* sharedReplacementLabel = nullptr;
	CCSprite* sharedReplacementSprite = nullptr;

	static Manager* getSharedInstance() {
		if (!instance) {
			instance = new Manager();
		}
		return instance;
	}

};