#include <Geode/modify/EndLevelLayer.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include "Manager.hpp"

#define managerMacro Manager::getSharedInstance()
#define getModBool Mod::get()->getSettingValue<bool>
#define getModDouble Mod::get()->getSettingValue<double>
#define getModString Mod::get()->getSettingValue<std::string>

using namespace geode::prelude;

class $modify(MyEndLevelLayer, EndLevelLayer) {
	CCSprite* getHideButtonSprite() {
		return typeinfo_cast<CCSprite*>(getChildByIDRecursive("hide-button")->getChildren()->objectAtIndex(0));
	}
	void toggleMainLayerVisibility(cocos2d::CCObject* sender) {
		if (!getModBool("hideEndLevelLayer")) return;
		if (m_mainLayer) {
			m_mainLayer->setVisible(!m_mainLayer->isVisible());
			if (m_mainLayer->isVisible()) this->setOpacity(managerMacro->originalOpacity);
			else this->setOpacity(0);
		}
		if (const auto hideButtonSprite = MyEndLevelLayer::getHideButtonSprite()) {
			if (getModBool("hideHideEndscreen")) {
				hideButtonSprite->setVisible(!hideButtonSprite->isVisible());
			}
		}
		if (const auto hideELLSprite = getChildByIDRecursive("hide-endlevellayer-sprite"_spr)) {
			if (getModBool("hideHideEndscreen")) {
				hideELLSprite->setVisible(!hideELLSprite->isVisible());
			}
		}
		/*
		auto endLevelLayerChildren = CCArrayExt<CCNode*>(this->getChildren());
		for (CCNode* node : endLevelLayerChildren) {
			if (auto currencyLayer = typeinfo_cast<CurrencyRewardLayer*>(node)) {
				currencyLayer->setVisible(!currencyLayer->isVisible());
			}
		}
		*/
	}
	void applySpaceUK() {
		if (!getModBool("spaceUK")) return;
		Manager* manager = managerMacro;
		auto levelCompleteText = getChildByIDRecursive("level-complete-text");
		if (!levelCompleteText) levelCompleteText = getChildByIDRecursive("practice-complete-text"); // grab practice mode complete text as fallback node
		if (!levelCompleteText) return;
		if (manager->isCompactEndscreen) {
			levelCompleteText->setVisible(true);
			levelCompleteText->setPositionX(manager->compactEndscreenFallbackPosition);
		}
		levelCompleteText->setScale(0.8f * (940.f / 1004.f)); // original scale of this node is 0.8 according to logs. hardcoding it here in case other mods decide to scale it to whatever else
	}
	void applyHideEndLevelLayerHideBtn() {
		const auto hideLayerMenu = typeinfo_cast<CCMenu*>(getChildByIDRecursive("hide-layer-menu"));
		const auto hideButtonSprite = MyEndLevelLayer::getHideButtonSprite();
		const auto hideButtonButton = hideButtonSprite->getParent();
		if (!hideLayerMenu || !m_mainLayer || !hideButtonSprite || !hideButtonButton) return;
		if (getModBool("hideHideEndscreen") || getModBool("hideEndLevelLayer")) hideButtonSprite->setVisible(false);
		// float desiredSpriteYPosition = hideButtonButton->getPositionY() - hideButtonButton->getContentHeight();
		if (getModBool("hideEndLevelLayer")) {
			Manager::getSharedInstance()->originalOpacity = this->getOpacity();
			this->setOpacity(0);
			m_mainLayer->setVisible(false);
			auto hideELLSprite = CCSprite::create("btn.png"_spr);
			hideELLSprite->setScaleX(hideButtonSprite->getScaleX());
			hideELLSprite->setScaleY(hideButtonSprite->getScaleY());
			hideELLSprite->setID("hide-endlevellayer-sprite"_spr);
			hideELLSprite->setVisible(MyEndLevelLayer::getHideButtonSprite()->isVisible());
			auto hideELLBtn = CCMenuItemSpriteExtra::create(hideELLSprite, this, menu_selector(MyEndLevelLayer::toggleMainLayerVisibility));
			hideELLBtn->setScale(hideButtonButton->getScale());
			hideELLBtn->setPositionY(hideButtonButton->getPositionY() - hideButtonButton->getContentHeight());
			hideELLBtn->setID("hide-endlevellayer-button"_spr);
			hideLayerMenu->addChild(hideELLBtn);
			hideLayerMenu->updateLayout(); // in case there is a layout in future node IDs updates
		}
	}
	void applyHideChainsBackground() {
		if (getModBool("hideChains")) {
			if (const auto left = getChildByIDRecursive("chain-left")) left->setVisible(false);
			if (auto right = getChildByIDRecursive("chain-right")) right->setVisible(false);
		}
		if (getModBool("hideBackground")) {
			if (const auto bg = getChildByIDRecursive("background")) bg->setVisible(false);
		}
	}
	void applyPlatAttemptsAndJumpsOrFlukedFromPercent(GJGameLevel* theLevel) {
		Manager* manager = managerMacro;
		manager->isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		const auto playLayer = PlayLayer::get();
		if (!m_mainLayer|| !playLayer) return;
		const bool isPlat = theLevel->isPlatformer();
		if (getModBool("platAttemptsAndJumps") && isPlat) {
			const auto timeLabel = getChildByIDRecursive("time-label");
			if (!timeLabel) return;
			const auto pointsLabel = getChildByIDRecursive("points-label");
			if (!manager->isCompactEndscreen) timeLabel->setPositionY(timeLabel->getPositionY() - 20);
			if (pointsLabel) pointsLabel->setPositionY(timeLabel->getPositionY() - 18);
			const auto attemptsLabel = cocos2d::CCLabelBMFont::create(("Attempts: " + std::to_string(playLayer->m_attempts)).c_str(), "goldFont.fnt");
			attemptsLabel->setScale(0.8f);
			attemptsLabel->setPosition(timeLabel->getPositionX(), timeLabel->getPositionY() + 40);
			attemptsLabel->setID("attempts-label"_spr);
			m_mainLayer->addChild(attemptsLabel);
			m_mainLayer->updateLayout();
			const auto jumpsLabel = cocos2d::CCLabelBMFont::create(("Jumps: " + std::to_string(playLayer->m_jumps)).c_str(), "goldFont.fnt");
			jumpsLabel->setScale(0.8f);
			jumpsLabel->setPosition({timeLabel->getPositionX(), timeLabel->getPositionY() + 20});
			jumpsLabel->setID("jumps-label"_spr);
			m_mainLayer->addChild(jumpsLabel);
			m_mainLayer->updateLayout();
		} else if (getModString("classicFlukedFrom") != "[Disabled]" && !isPlat && !playLayer->m_isTestMode && !playLayer->m_isPracticeMode && manager->lastFlukedPercent < 100) {
			const auto timeLabel = getChildByIDRecursive("time-label");
			const auto jumpsLabel = getChildByIDRecursive("jumps-label");
			if (!timeLabel || !jumpsLabel) return;
			jumpsLabel->setPositionY(jumpsLabel->getPositionY() + 7.0f);
			timeLabel->setPositionY(timeLabel->getPositionY() + 14.0f);
			auto flukedFromLabel = cocos2d::CCLabelBMFont::create(fmt::format("{}: {}%", getModString("classicFlukedFrom"), manager->lastFlukedPercent).c_str(), "goldFont.fnt");
			flukedFromLabel->setPosition(jumpsLabel->getPositionX(), timeLabel->getPositionY() - 16.0f);
			flukedFromLabel->setScale(timeLabel->getScale());
			flukedFromLabel->setID("fluked-from-label"_spr);
			m_mainLayer->addChild(flukedFromLabel);
			m_mainLayer->updateLayout();
		}
	}
	/*
	void applyGDMOCompatShowLayer(GJGameLevel* theLevel) {
		isGDMO = Loader::get()->isModLoaded("maxnu.gd_mega_overlay");
		if (isGDMO && theLevel->m_coins == 0 && Loader::get()->getLoadedMod("maxnu.gd_mega_overlay")->getSavedValue<bool>("level/endlevellayerinfo/enabled")) {
			//
				gdmo does this silly thing where they add children without giving them node IDs and i need to release this mod ASAP so please forgive me for using getobjectatindex but getchildoftype doesnt work for this use case because everything in endscreen layer is a child of some other cclayer smh
				if (!m_mainLayer) return;
				auto mainLayerChildren = m_mainLayer->getChildren();
				auto attemptsLabel = getChildByIDRecursive("attempts-label");
				auto jumpsLabel = getChildByIDRecursive("jumps-label");
				if (attemptsLabel == nullptr || jumpsLabel == nullptr) {
					log::info("uhoh! couldnt find labels");
					attemptsLabel = getChildByIDRecursive("attempts-label"_spr);
					jumpsLabel = getChildByIDRecursive("jumps-label"_spr);
				}
				auto iHopeThisIsGDMONoclipAccuracyLabel = typeinfo_cast<CCNode*>(mainLayerChildren->objectAtIndex(3));
				auto iHopeThisIsGDMONoclipDeathLabel = typeinfo_cast<CCNode*>(mainLayerChildren->objectAtIndex(4));
				if (iHopeThisIsGDMONoclipAccuracyLabel == nullptr || iHopeThisIsGDMONoclipDeathLabel == nullptr) {
					return;
				}
				if (strcmp(iHopeThisIsGDMONoclipAccuracyLabel->getID().c_str(), "") != 0 || strcmp(iHopeThisIsGDMONoclipDeathLabel->getID().c_str(), "") != 0) {
					return;
				}
				iHopeThisIsGDMONoclipAccuracyLabel->setPositionY(attemptsLabel->getPositionY());
				iHopeThisIsGDMONoclipDeathLabel->setPositionY(jumpsLabel->getPositionY());
			//
			// backup plan starts below
			float windowWidth = getChildByIDRecursive("background")->getContentSize().width;
			float windowHeight = CCDirector::get()->getWinSize().height;
			float offset = getChildByIDRecursive("background")->getPositionX();
			auto starContainer = getChildByIDRecursive("star-container");
			if (starContainer == nullptr) { starContainer = getChildByIDRecursive("moon-container"); }
			float gdmoHeight = windowHeight * (285.f / 320.f);
			float gdmoTwentyFivePercentX = (windowWidth * .25f) + offset;
			float gdmoFiftyPercentX = (windowWidth * .5f) + offset;
			float gdmoSeventyFivePercentX = (windowWidth * .75f) + offset;
			if (starContainer) {
				if (theLevel->m_stars.value() == 1) { starContainer->setPositionX(gdmoFiftyPercentX); }
				else { starContainer->setPositionX(gdmoTwentyFivePercentX); }
				starContainer->setPositionY(gdmoHeight);
			}
			if (auto orbContainer = getChildByIDRecursive("orb-container")) {
				orbContainer->setPositionY(gdmoHeight);
				if (auto diamondContainer = getChildByIDRecursive("diamond-container")) {
					diamondContainer->setPositionX(gdmoSeventyFivePercentX);
					diamondContainer->setPositionY(gdmoHeight);
					orbContainer->setPositionX(gdmoFiftyPercentX);
				} else {
					orbContainer->setPositionX(gdmoSeventyFivePercentX);
				}
			}
		}
	}
	*/
	void applyCustomLevelCompleteText(const std::string_view matchPlayLayer) {
		if (!getModBool("customLevelCompleteText") || !getModBool("enabled") || !m_playLayer) return;
		const auto lvlCompleteText = typeinfo_cast<CCSprite*>(getChildByIDRecursive("level-complete-text"));
		if (!lvlCompleteText) return;
		Manager* manager = managerMacro;
		auto origOpacity = lvlCompleteText->getOpacity();
		lvlCompleteText->setOpacity(0);
		if (matchPlayLayer == "Separate From EndLevelLayer") manager->generateNewSprites(getModString("customLCTMode"), m_playLayer);
		if (manager->chosenMode == "Images" && !manager->sharedReplacementSprite.empty()) {
			CCSprite* newSprite = CCSprite::create(manager->sharedReplacementSprite.c_str());
			if (!newSprite) {
				lvlCompleteText->setOpacity(origOpacity);
				return log::info("tried replacing the sprite, but newly created sprite was null? attempted image: {}", manager->sharedReplacementSprite);
			}
			lvlCompleteText->addChild(newSprite);
			lvlCompleteText->updateLayout();
			newSprite->setPosition(lvlCompleteText->getContentSize() / 2.f);
			newSprite->setID("custom-level-complete-sprite-endlevellayer"_spr);
			if (getModBool("scaleCustomLevelCompleteImages")) {
				const float contentHeight = newSprite->getContentHeight();
				if (contentHeight < 36.5f) newSprite->setScale(36.5f / contentHeight);
				else if (contentHeight > 50.f) newSprite->setScaleY(50.f / contentHeight);
				const float contentWidth = newSprite->getContentWidth();
				if (contentWidth * newSprite->getScaleX() > 400.f) newSprite->setScaleX(400.f / contentWidth);
			}
		} else if (manager->chosenMode == "Oxygene One" && !manager->sharedReplacementLabel.empty()) {
			CCLabelBMFont* newLabel = CCLabelBMFont::create(manager->sharedReplacementLabel.c_str(), "levelCompleteFont.fnt"_spr);
			if (!newLabel) {
				lvlCompleteText->setOpacity(origOpacity);
				return log::info("tried replacing the sprite, but newly created label was null? attempted image: {}", manager->sharedReplacementLabel);
			}
			newLabel->setExtraKerning(5);
			newLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			lvlCompleteText->addChild(newLabel);
			lvlCompleteText->updateLayout();
			newLabel->setPosition(lvlCompleteText->getContentSize() / 2.f);
			newLabel->setID("custom-level-complete-label-endlevellayer"_spr);
		} else {
			lvlCompleteText->setOpacity(origOpacity);
			return log::info("failed. manager->chosenMode: {}", manager->chosenMode);
		}
	}
	void applyRandomQuoteAndFont(PlayLayer* playLayer, GJGameLevel* theLevel) {
		auto endTextLabel = typeinfo_cast<CCLabelBMFont*>(getChildByIDRecursive("end-text"));
		if (!endTextLabel) return;
		Manager* manager = managerMacro;
		std::string randomString = manager->grabRandomString();
		if (!manager->customQuotes.empty() && getModBool("customTextsOnly")) randomString = manager->grabRandomString(manager->customQuotes);
		if ("Make sure to screenshot this win!" == randomString) {
			#ifdef GEODE_IS_MACOS
				randomString = "Press Command + Shift + 3 to screenshot this win!";
			#endif
			#ifdef GEODE_IS_ANDROID
				randomString = "Press the \"Volume Down\'\' and \"Power\'\' buttons to screenshot this win!";
			#endif
			#ifdef GEODE_IS_WINDOWS
				randomString = "Press \"Win + Shift + S\'\' or \"PrtSc\'\' to screenshot this win!";
			#endif
		} else if (R"(''First try, part two!")" == randomString) {
			std::string temp = fmt::format(R"(''First try, part {}!")", playLayer->m_attempts);
			if (playLayer->m_attempts == 1) { temp = R"(''First try!")"; }
			randomString = temp;
		} else if (R"(''As you can see, my FPS is around 18 or so, which means we can definitely take this further.")" == randomString) {
			randomString = fmt::format(R"(''As you can see, my FPS is around {} or so, which means we can definitely take this further.")", manager->fps);
		} else if (R"(''If you wish to defeat me, train for another 100 years.")" == randomString) {
			randomString = fmt::format(R"(''If you wish to defeat me, train for another {} years.")", std::max(100, (playLayer->m_jumps * 100)));
		} else if ("Good luck on your statgrinding session!" == randomString && theLevel->m_stars.value() != 0) {
			if (theLevel->isPlatformer()) { randomString = "Good luck on that moongrinding session!"; }
			else { randomString = "Good luck on that stargrinding session!"; }
		}

		float scale = 0.36f * 228.f / strlen(randomString.c_str());
		if ("BELIEVE" == randomString) scale = 1.5f;
		else if ("endTextLabel->setString(randomString.c_str(), true);" == randomString) scale = 0.4f;
		else if (scale > Mod::get()->getSettingValue<double>("maxScale")) scale = getModDouble("maxScale");

		endTextLabel->setScale(scale);
		endTextLabel->setWidth(336.f); // width of end screen minus 20px

		endTextLabel->setString(randomString.c_str(), true); // set string

		int64_t fontID = Mod::get()->getSettingValue<int64_t>("customFont");
		if (fontID == -2) {
			endTextLabel->setFntFile("chatFont.fnt");
		} else if (fontID == -1) {
			endTextLabel->setFntFile("goldFont.fnt");
		} else if (fontID != 0) {
			endTextLabel->setFntFile(fmt::format("gjFont{:02d}.fnt", fontID).c_str());
		}

		endTextLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter); // center text

		if (manager->isCompactEndscreen) endTextLabel->setPositionX(manager->compactEndscreenFallbackPosition);
		if (randomString.empty()) endTextLabel->setString(manager->fallbackString.c_str(), true); // fallback string
	}
	void showLayer(bool p0) {
		if (!getModBool("enabled")) return EndLevelLayer::showLayer(p0);
		EndLevelLayer::showLayer(getModBool("noTransition"));
		if (auto playLayer = PlayLayer::get()) {
			auto theLevel = playLayer->m_level;
			MyEndLevelLayer::applyHideEndLevelLayerHideBtn();
			MyEndLevelLayer::applyHideChainsBackground();
			MyEndLevelLayer::applySpaceUK();
			MyEndLevelLayer::applyPlatAttemptsAndJumpsOrFlukedFromPercent(theLevel);
			// MyEndLevelLayer::applyGDMOCompatShowLayer(theLevel);
		}
	}
	void customSetup() {
		EndLevelLayer::customSetup();
		if (!getModBool("enabled")) return;
		managerMacro->isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		auto playLayer = PlayLayer::get();
		if (!playLayer) return;
		if (getModBool("endTexts")) MyEndLevelLayer::applyRandomQuoteAndFont(playLayer, playLayer->m_level);
		if (getModBool("customLevelCompleteText")) MyEndLevelLayer::applyCustomLevelCompleteText(getModString("alsoReplacePlayLayerLCT"));
	}
};