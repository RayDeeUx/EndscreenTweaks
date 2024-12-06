#include <Geode/modify/PlayLayer.hpp>
#include <random>
#include "Manager.hpp"

#define getModBool Mod::get()->getSettingValue<bool>
#define getModString Mod::get()->getSettingValue<std::string>
#define managerMacro Manager::getSharedInstance()

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
	static std::string grabRandomString(std::vector<std::string> vector = managerMacro->quotes) {
		std::mt19937 randomSeed(std::random_device{}());
		std::shuffle(vector.begin(), vector.end(), randomSeed);
		return vector.front();
	}
	static bool isSupportedExtension(const std::string& fileName) {
		return !fileName.empty() && (geode::utils::string::endsWith(fileName, ".png") ||  geode::utils::string::endsWith(fileName, ".jpg"));
	}
	void levelComplete() {
		if (!m_level) return PlayLayer::levelComplete();
		Manager* manager = managerMacro;
		manager->lastFlukedPercent = this->m_level->m_normalPercent.value();
		if (getModBool("dontShowFlukeIfZero") && manager->lastFlukedPercent == 0) { manager->lastFlukedPercent = 200; }
		// set it to an impossibly high value because i am too lazy to write yet another compound boolean statement
		PlayLayer::levelComplete();
	}
	void onQuit() {
		Manager::getSharedInstance()->lastFlukedPercent = 0;
		PlayLayer::onQuit();
	}
	void showCompleteText() {
		PlayLayer::showCompleteText();
		if (!m_level || !getModBool("enabled") || !getModBool("customLevelCompleteText") || getModString("alsoReplacePlayLayerLCT") == "Disabled") return;
		Manager* manager = managerMacro;
		CCSprite* toModify = nullptr;
		for (const auto node : CCArrayExt<CCNode*>(this->getChildren())) {
			if (cocos::isSpriteFrameName(node, "GJ_levelComplete_001.png")) {
				toModify = static_cast<CCSprite*>(node);
				break;
			}
		}
		if (!toModify) return;
		std::string customLCTMode = getModString("customLCTMode");
		auto originalOpacity = toModify->getOpacity();
		bool lctReplaced = false;
		toModify->setOpacity(0);
		if (customLCTMode == "Combined" || customLCTMode == "Images") {
			if (!manager->customLevelCompleteSprites.empty()) manager->customLevelCompleteSprites.clear();
			for (const auto& file : std::filesystem::directory_iterator((Mod::get()->getConfigDir() / R"(levelCompleteImages)").string())) {
				if (isSupportedExtension(file.path().extension().string())) {
					manager->customLevelCompleteSprites.push_back(file.path().string());
				}
			}
			std::string chosenSprite; // init sprite
			if (!manager->customLevelCompleteSprites.empty()) chosenSprite = grabRandomString(manager->customLevelCompleteSprites);
			else if (customLCTMode == "Images" && chosenSprite.empty()) return toModify->setOpacity(originalOpacity); // return early if no file name was selected even though user selected "Images" mode
			manager->sharedReplacementSprite = CCSprite::create(chosenSprite.c_str());
		}
		if (customLCTMode == "Combined" || customLCTMode == "Oxygene One") {
			std::string customQuote = getModBool("customLevelCompleteTextsOnly") ? grabRandomString(manager->customLevelCompleteQuotes) : grabRandomString(manager->levelCompleteQuotes);
			CCLabelBMFont* replacementLabel = CCLabelBMFont::create(customQuote.c_str(), "levelCompleteFont.fnt"_spr);
			replacementLabel->setExtraKerning(5);
			manager->sharedReplacementLabel = replacementLabel;
		}
		if (customLCTMode == "Combined") customLCTMode = grabRandomString(manager->knownCLCTModesBesidesCombined);
		if (customLCTMode == "Oxygene One" && manager->sharedReplacementLabel && !lctReplaced) {
			toModify->addChild(manager->sharedReplacementLabel);
			manager->sharedReplacementLabel->setPosition(toModify->getContentSize() / 2.f);
			manager->sharedReplacementLabel->limitLabelWidth(370.f, 1.0f, 0.25f);
			manager->sharedReplacementLabel->setID("custom-level-complete-label-playlayer"_spr);
			lctReplaced = true;
		} else if (customLCTMode == "Images" && manager->sharedReplacementSprite && !lctReplaced) {
			toModify->addChild(manager->sharedReplacementSprite);
			manager->sharedReplacementSprite->setPosition(toModify->getContentSize() / 2.f);
			manager->sharedReplacementSprite->setID("custom-level-complete-sprite-playlayer"_spr);
			lctReplaced = true;
		}
		if (!lctReplaced) return toModify->setOpacity(originalOpacity);
	}
};