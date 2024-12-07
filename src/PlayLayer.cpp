#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"

#define getModBool Mod::get()->getSettingValue<bool>
#define getModString Mod::get()->getSettingValue<std::string>
#define managerMacro Manager::getSharedInstance()

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
	void levelComplete() {
		if (!m_level) return PlayLayer::levelComplete();
		Manager* manager = managerMacro;
		manager->lastFlukedPercent = this->m_level->m_normalPercent.value();
		if (getModBool("dontShowFlukeIfZero") && manager->lastFlukedPercent == 0) { manager->lastFlukedPercent = 200; }
		// set it to an impossibly high value because i am too lazy to write yet another compound boolean statement
		PlayLayer::levelComplete();
	}
	void onQuit() {
		Manager* manager = managerMacro;
		manager->lastFlukedPercent = 0;
		manager->sharedReplacementSprite = "";
		manager->sharedReplacementLabel = "";
		manager->chosenMode = "";
		PlayLayer::onQuit();
	}
	void showCompleteText() {
		PlayLayer::showCompleteText();
		if (!m_level || !getModBool("enabled") || !getModBool("customLevelCompleteText")) return;
		Manager* manager = managerMacro;
		CCSprite* toModify = nullptr;
		for (const auto node : CCArrayExt<CCNode*>(this->getChildren())) {
			if (cocos::isSpriteFrameName(node, "GJ_levelComplete_001.png")) {
				toModify = static_cast<CCSprite*>(node);
				break;
			}
		}
		if (!toModify) return;
		std::string customLCTModePlayLayer = getModString("customLCTMode");
		bool lctReplaced = false;
		manager->generateNewSprites(customLCTModePlayLayer);
		// "omg ery why not just choose a mode first???"
		// assigning it to manager = easier "shared replacement" with EndLevelLayer later
		if (getModString("alsoReplacePlayLayerLCT") == "Disabled") return;
		auto origOpacity = toModify->getOpacity();
		toModify->setOpacity(0);
		if (manager->chosenMode == "Images" && !manager->sharedReplacementSprite.empty() && !lctReplaced) {
			CCSprite* newSprite = CCSprite::create(manager->sharedReplacementSprite.c_str());
			toModify->addChild(newSprite);
			toModify->updateLayout();
			newSprite->setPosition(toModify->getContentSize() / 2.f);
			newSprite->setID("custom-level-complete-sprite-playlayer"_spr);
			lctReplaced = true;
		}
		if (manager->chosenMode == "Oxygene One" && !manager->sharedReplacementLabel.empty() && !lctReplaced) {
			CCLabelBMFont* newLabel = CCLabelBMFont::create(manager->sharedReplacementLabel.c_str(), "levelCompleteFont.fnt"_spr);
			newLabel->setExtraKerning(5);
			newLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			toModify->addChild(newLabel);
			toModify->updateLayout();
			newLabel->setPosition(toModify->getContentSize() / 2.f);
			newLabel->setID("custom-level-complete-label-playlayer"_spr);
			lctReplaced = true;
		}
		if (!lctReplaced) {
			toModify->setOpacity(origOpacity);
			return log::info("failed replacing sprite. customLCTMode: {} | manager->chosenMode: {}", customLCTModePlayLayer, manager->chosenMode);
		}
	}
};