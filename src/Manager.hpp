#pragma once
#include <random>

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

	std::string sharedReplacementLabel = "";
	std::string sharedReplacementSprite = "";
	std::string chosenMode = "";

	static Manager* getSharedInstance() {
		if (!instance) {
			instance = new Manager();
		}
		return instance;
	}
	static bool isSupportedExtension(const std::string& fileName) {
		return !fileName.empty() && (geode::utils::string::endsWith(fileName, ".png") ||  geode::utils::string::endsWith(fileName, ".jpg"));
	}
	static std::string grabRandomString(std::vector<std::string> vector = getSharedInstance()->quotes) {
		std::mt19937 randomSeed(std::random_device{}());
		std::shuffle(vector.begin(), vector.end(), randomSeed);
		return vector.front();
	}
	static void generateNewSprites(std::string customLCTMode) {
		Manager* manager = getSharedInstance();
		log::info("customLCTMode: {}", customLCTMode);
		if (customLCTMode == "Combined" || customLCTMode == "Images") {
			if (!manager->customLevelCompleteSprites.empty()) manager->customLevelCompleteSprites.clear();
			for (const auto& file : std::filesystem::directory_iterator((Mod::get()->getConfigDir() / R"(levelCompleteImages)").string())) {
				if (isSupportedExtension(file.path().extension().string())) {
					manager->customLevelCompleteSprites.push_back(file.path().string());
				}
			}
			std::string chosenSprite; // init sprite
			if (!manager->customLevelCompleteSprites.empty()) chosenSprite = grabRandomString(manager->customLevelCompleteSprites);
			else if (customLCTMode == "Images" && chosenSprite.empty()) return; // return early if no file name was selected even though user selected "Images" mode
			manager->sharedReplacementSprite = chosenSprite;
		}
		if (customLCTMode == "Combined" || customLCTMode == "Oxygene One") {
			manager->sharedReplacementLabel = Mod::get()->getSettingValue<bool>("customLevelCompleteTextsOnly") ? grabRandomString(manager->customLevelCompleteQuotes) : grabRandomString(manager->levelCompleteQuotes);
		}
		if (customLCTMode == "Combined") customLCTMode = manager->grabRandomString(manager->knownCLCTModesBesidesCombined);
		log::info("customLCTMode: {} (should not be Combined)", customLCTMode);
		if (customLCTMode == "Combined") {
			return log::info("something went terribly wrong --- check contents of the `manager->knownCLCTModesBesidesCombined` vector");
		}
		manager->chosenMode = customLCTMode;
	}
};