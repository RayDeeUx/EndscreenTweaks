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

	std::vector<std::string> loadedModMenus;

	std::vector<std::string> knownCLCTModesBesidesCombined = {"Images", "Oxygene One"};

	std::vector<std::string> downwellStages = {"CAVERNS", "CATACOMBS", "AQUIFIER", "LIMBO", "ABYSS"};
	std::vector<std::string> parappaResults = {"COOL", "GOOD", "BAD", "AWFUL"};
	std::vector<std::string> sonicNames = {"PLAYER", "SONIC", "SHADOW", "TAILS", "KNUCKLES"};
	std::vector<std::string> cupheadResults = {"A BRAND NEW RECORD!", "GOLLY! A PERFECT SCORE!"};

	std::map<std::string, std::string> modIDToModMenu = {
		{"eclipse.eclipse-menu", "ECLIPSE MENU"},
		{"firee.prism", "PRISM MENU"},
		{"prevter.openhack", "OPENHACK"},
		{"tobyadd.gdh", "GDH"},
		{"tobyadd.gdh_mobile", "GDH"},
		{"absolllute.megahack", "MEGAHACK"},
		{"thesillydoggo.qolmod", "QOLMOD"}
	};

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
	// too lazy to make Utils.cpp / Utils.hpp file pair so everything is here
	static bool isSupportedExtension(const std::string& fileName) {
		return !fileName.empty() && (geode::utils::string::endsWith(fileName, ".png") ||  geode::utils::string::endsWith(fileName, ".jpg"));
	}
	static std::string grabRandomString(std::vector<std::string> vector = getSharedInstance()->quotes) {
		if (vector.empty()) return "";
		std::mt19937 randomSeed(std::random_device{}());
		std::shuffle(vector.begin(), vector.end(), randomSeed);
		return vector.front();
	}
	static int getRandInt(const int max = 3) {
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uni(1, max);
		return uni(rng);
	}
	static void generateNewSprites(std::string customLCTMode, PlayLayer* playLayer) {
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
			std::string replacementLabelText = Mod::get()->getSettingValue<bool>("customLevelCompleteTextsOnly") ? grabRandomString(manager->customLevelCompleteQuotes) : grabRandomString(manager->levelCompleteQuotes);
			/*
			CAVERN-0 CLEARED [RANDOM NUMBER, 1-3]
			SONIC MADE IT THROUGH ACT 0 [RANDOM NUMBER, 1-3]
			RESULT: -1 EXP, $-2 MONEY, -3 COINS [-1 = STAR, -2 = ORB COUNT, -3 = COLLECT COIN COUNT]
			U RAPPIN' RESULT! [RANDOM: COOL GOOD BAD AWFUL]
			A KNOCKOUT! [CHOOSE BETWEEN - GOLLY! A PERFECT SCORE! / A BRAND NEW RECORD! - IF LEVEL RATED]
			*/
			log::info("replacementLabelText before changes: {}", replacementLabelText);
			if (replacementLabelText == "SONIC MADE IT THROUGH ACT 0" && getRandInt(1000) == 1) {
				replacementLabelText = "HAHA, ONE!";
			}
			GJGameLevel* level = playLayer->m_level;
			std::string chosenModID = grabRandomString(manager->loadedModMenus);
			if (replacementLabelText == "DOWNWELL-0 CLEARED" || replacementLabelText == "SONIC MADE IT THROUGH ACT 0") {
				replacementLabelText = utils::string::replace(replacementLabelText, "0", utils::numToString(getRandInt()));
				replacementLabelText = utils::string::replace(replacementLabelText, "DOWNWELL", grabRandomString(manager->downwellStages));
				replacementLabelText = utils::string::replace(replacementLabelText, "SONIC", grabRandomString(manager->sonicNames));
				if (replacementLabelText.starts_with("PLAYER")) {
					replacementLabelText = utils::string::replace(replacementLabelText, "PLAYER", utils::string::toUpper(GameManager::get()->m_playerName));
				}
			} else if (replacementLabelText == "U RAPPIN' RESULT!") {
				replacementLabelText = utils::string::replace(replacementLabelText, "RESULT", grabRandomString(manager->parappaResults));
			} else if (replacementLabelText == "A KNOCKOUT!" && level->m_stars.value() != 0) {
				replacementLabelText = grabRandomString(manager->cupheadResults);
			} else if (replacementLabelText == "RESULT: -1 EXP, $-2 MONEY, -3 ITEMS") {
				replacementLabelText = utils::string::replace(replacementLabelText, "-1", utils::numToString(level->m_stars.value()));
				replacementLabelText = utils::string::replace(replacementLabelText, "-2", utils::numToString(getRandInt(500)));
				replacementLabelText = utils::string::replace(replacementLabelText, "-3", utils::numToString(getRandInt()));
				if (replacementLabelText.ends_with("1 ITEMS")) {
					replacementLabelText = utils::string::replace(replacementLabelText, "1 ITEMS", "1 ITEMS");
				}
			} else if (replacementLabelText == "SPONSORED BY YOUR LOCAL MOD MENU" && !manager->loadedModMenus.empty() && manager->modIDToModMenu.contains(chosenModID)) {
				replacementLabelText = utils::string::replace(replacementLabelText, "YOUR LOCAL MOD MENU", manager->modIDToModMenu.find(chosenModID)->second);
			}
			log::info("replacementLabelText after changes: {}", replacementLabelText);
			manager->sharedReplacementLabel = replacementLabelText;
		}
		if (customLCTMode == "Combined") customLCTMode = manager->grabRandomString(manager->knownCLCTModesBesidesCombined);
		log::info("customLCTMode: {} (should not be Combined)", customLCTMode);
		log::info("sharedReplacementSprite: {}", manager->sharedReplacementSprite);
		log::info("sharedReplacementLabel: {}", manager->sharedReplacementLabel);
		if (customLCTMode == "Combined") {
			return log::info("something went terribly wrong --- check contents of the `manager->knownCLCTModesBesidesCombined` vector");
		}
		manager->chosenMode = customLCTMode;
	}
};
