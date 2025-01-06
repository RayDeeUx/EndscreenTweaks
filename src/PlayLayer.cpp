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
		manager->generateNewSprites(customLCTModePlayLayer, this);
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
			if (getModBool("scaleCustomLevelCompleteImages")) {
				const float contentHeight = newSprite->getContentHeight();
				if (contentHeight < 36.5f) newSprite->setScale(36.5f / contentHeight);
				else if (contentHeight > 150.f) newSprite->setScaleY(150.f / contentHeight);
				const float contentWidth = newSprite->getContentWidth();
				if (contentWidth * newSprite->getScaleX() > 400.f) newSprite->setScaleX(400.f / contentWidth);
			}
			lctReplaced = true;
		}
		if (manager->chosenMode == "Oxygene One" && !manager->sharedReplacementLabel.empty() && !lctReplaced) {
			// Main Font, untouched
			CCLabelBMFont* newLabel = CCLabelBMFont::create(manager->sharedReplacementLabel.c_str(), "levelCompleteFont.fnt"_spr);
			newLabel->setExtraKerning(5);
			newLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			toModify->addChild(newLabel);
			toModify->updateLayout();
			newLabel->setPosition(toModify->getContentSize() / 2.f);
			newLabel->setID("custom-level-complete-label-playlayer"_spr);
			newLabel->setZOrder(1);

			// Underlay font
			CCLabelBMFont* underLabel = CCLabelBMFont::create(manager->sharedReplacementLabel.c_str(), "levelCompleteFont_underlay.fnt"_spr);
			underLabel->setExtraKerning(5);
			underLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			toModify->addChild(underLabel);
			toModify->updateLayout();
			underLabel->setPosition(toModify->getContentSize() / 2.f);
			underLabel->setID("custom-level-complete-label-playlayer-underlay"_spr);
			underLabel->setZOrder(0);
			lctReplaced = true;
		}
		if (!lctReplaced) {
			toModify->setOpacity(origOpacity);
			return log::info("failed replacing sprite. customLCTMode: {} | manager->chosenMode: {}", customLCTModePlayLayer, manager->chosenMode);
		}
	}
};
