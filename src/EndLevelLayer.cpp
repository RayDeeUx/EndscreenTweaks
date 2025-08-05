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
		const Manager* manager = managerMacro;
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
			if (CCNode* left = getChildByIDRecursive("chain-left")) left->setVisible(false);
			if (CCNode* right = getChildByIDRecursive("chain-right")) right->setVisible(false);
		}
		if (getModBool("hideBackground")) {
			if (CCNode* bg = getChildByIDRecursive("background")) bg->setVisible(false);
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
			const auto jumpsLabel = cocos2d::CCLabelBMFont::create(("Jumps: " + std::to_string(playLayer->m_jumps)).c_str(), "goldFont.fnt");
			jumpsLabel->setScale(0.8f);
			jumpsLabel->setPosition({timeLabel->getPositionX(), timeLabel->getPositionY() + 20});
			jumpsLabel->setID("jumps-label"_spr);
			m_mainLayer->addChild(jumpsLabel);
		} else if (getModString("classicFlukedFrom") != "[Disabled]" && !isPlat && !playLayer->m_isTestMode && !playLayer->m_isPracticeMode && manager->lastFlukedPercent < 100) {
			const auto timeLabel = getChildByIDRecursive("time-label");
			const auto jumpsLabel = getChildByIDRecursive("jumps-label");
			if (!timeLabel || !jumpsLabel) return;
			jumpsLabel->setPositionY(jumpsLabel->getPositionY() + 7.0f);
			timeLabel->setPositionY(timeLabel->getPositionY() + 14.0f);
			CCLabelBMFont* flukedFromLabel = CCLabelBMFont::create(fmt::format("{}: {}%", getModString("classicFlukedFrom"), manager->lastFlukedPercent).c_str(), "goldFont.fnt");
			flukedFromLabel->setPosition(jumpsLabel->getPositionX(), timeLabel->getPositionY() - 16.0f);
			flukedFromLabel->setScale(timeLabel->getScale());
			flukedFromLabel->setID("fluked-from-label"_spr);
			m_mainLayer->addChild(flukedFromLabel);
		}
	}
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
			CCLabelBMFont* underLabel = CCLabelBMFont::create(manager->sharedReplacementLabel.c_str(), "levelCompleteFont_underlay.fnt"_spr);
			if (!newLabel) {
				lvlCompleteText->setOpacity(origOpacity);
				return log::info("tried replacing the sprite, but newly created label was null? attempted image: {}", manager->sharedReplacementLabel);
			}
			// add main font
			newLabel->setExtraKerning(5);
			newLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			lvlCompleteText->addChild(newLabel);
			newLabel->setPosition(lvlCompleteText->getContentSize() / 2.f);
			newLabel->setID("custom-level-complete-label-endlevellayer"_spr);
			newLabel->setZOrder(1);
			
			// add underlay font
			underLabel->setExtraKerning(5);
			underLabel->limitLabelWidth(380.f, 1.0f, 0.25f);
			lvlCompleteText->addChild(underLabel);
			underLabel->setPosition(lvlCompleteText->getContentSize() / 2.f);
			underLabel->setID("custom-level-complete-label-endlevellayer-underlay"_spr);
			underLabel->setZOrder(0);
		} else {
			lvlCompleteText->setOpacity(origOpacity);
			return log::info("failed. manager->chosenMode: {}", manager->chosenMode);
		}
	}
	void applyRandomQuoteAndFont(PlayLayer* playLayer, GJGameLevel* theLevel) {
		auto endTextLabel = typeinfo_cast<CCLabelBMFont*>(getChildByIDRecursive("end-text"));
		if (!endTextLabel) return;

		const int64_t fontID = Mod::get()->getSettingValue<int64_t>("customFont");
		if (fontID == -2) {
			endTextLabel->setFntFile("chatFont.fnt");
		} else if (fontID == -1) {
			endTextLabel->setFntFile("goldFont.fnt");
		} else if (fontID != 0) {
			endTextLabel->setFntFile(fmt::format("gjFont{:02d}.fnt", fontID).c_str());
		}

		Manager* manager = managerMacro;
		if (manager->isCompactEndscreen) endTextLabel->setPositionX(manager->compactEndscreenFallbackPosition);

		std::string randomString = !manager->customQuotes.empty() && getModBool("customTextsOnly") ? manager->grabRandomString(manager->customQuotes) : manager->grabRandomString();
		if (randomString.empty()) return;

		if ("Make sure to screenshot this win!" == randomString) {
			#ifdef GEODE_IS_MACOS
				randomString = "Press Command + Shift + 3 to screenshot this win!";
			#elif defined(GEODE_IS_IOS)
				randomString = "Don\'t forget to screenshot this win!";
			#elif defined(GEODE_IS_ANDROID)
				randomString = "Press the \"Volume Down\'\' and \"Power\'\' buttons to screenshot this win!";
			#elif defined(GEODE_IS_WINDOWS)
				randomString = "Press \"Win + Shift + S\'\' or \"PrtSc\'\' to screenshot this win!";
			#endif
		} else if (R"(''First try, part two!")" == randomString) {
			std::string temp = fmt::format(R"(''First try, part {}!")", playLayer->m_attempts);
			if (playLayer->m_attempts == 1) { temp = R"(''First try!")"; }
			randomString = temp;
		} else if (R"(''As you can see, my FPS is around 18 or so, which means we can definitely take this further.")" == randomString) {
			randomString = fmt::format(R"(''As you can see, my FPS is around {} or so, which means we can definitely take this further.")", CCDirector::get()->m_fFrameRate);
		} else if (R"(''If you wish to defeat me, train for another 100 years.")" == randomString) {
			randomString = fmt::format(R"(''If you wish to defeat me, train for another {} years.")", std::max(100, (playLayer->m_jumps * 100)));
		} else if ("Good luck on your statgrinding session!" == randomString && theLevel->m_stars.value() != 0) {
			if (theLevel->isPlatformer()) { randomString = "Good luck on that moongrinding session!"; }
			else { randomString = "Good luck on that stargrinding session!"; }
		}

		float scale = 0.36f * 228.f / static_cast<float>(randomString.length());
		if ("BELIEVE" == randomString) scale = 1.5f;
		else if ("endTextLabel->setString(randomString.c_str());" == randomString) scale = 0.4f;
		else if (scale > Mod::get()->getSettingValue<double>("maxScale")) scale = getModDouble("maxScale");

		endTextLabel->setScale(scale);
		endTextLabel->setWidth(336.f); // width of end screen minus 20px
		endTextLabel->setString(randomString.c_str()); // set string
		endTextLabel->setAlignment(kCCTextAlignmentCenter);
	}
	void addLoadedModsList() {
		if (!m_playLayer || m_playLayer->m_isPracticeMode || !m_listLayer) return;
		if (!getModBool("enabled") || !getModBool("showModsListButton")) return;

		CCLabelBMFont* label = CCLabelBMFont::create("Mods\nList", "bigFont.fnt");
		CCSprite* sprite = CircleButtonSprite::create(label, CircleBaseColor::Green);
		sprite->setScale(.8f);
		label->setScale(.35f);
		label->setAlignment(kCCTextAlignmentCenter);

		CCMenuItemSpriteExtra* modsListButton = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyEndLevelLayer::showModsList));

		CCMenu* modsListMenu = CCMenu::create();
		modsListMenu->setContentSize({33, 33});
		modsListMenu->setPosition(m_listLayer->getPosition()); // i know this sounds like a crackpot position but it fits perfectly (im ignoring practice mode bc who tf wanna show that)
		modsListMenu->ignoreAnchorPointForPosition(false);
		modsListMenu->addChildAtPosition(modsListButton, Anchor::Center);

		this->m_mainLayer->addChild(modsListMenu);
	}
	void showModsList(CCObject* sender) {
		Manager* manager = Manager::getSharedInstance();
		std::string formattedList;
		for (const std::string& modListEntry : manager->formattedModsListVector) {
			formattedList += modListEntry;
			formattedList += '\n';
		}
		const std::string& formattedTitle = fmt::format("{} Total ({} Loaded, {} Disabled, {} w/Problems)", manager->totalMods, manager->loadedMods, manager->disabledMods, manager->problemMods);
		MDPopup::create(formattedTitle, formattedList, "Copy", "Close", [formattedList](const bool btn2){
			if (btn2) return;
			geode::utils::clipboard::write(formattedList);
			Notification::create("Copied mods list")->show();
		})->show();
	}
	void showLayer(bool p0) {
		if (!getModBool("enabled")) return EndLevelLayer::showLayer(p0);
		EndLevelLayer::showLayer(getModBool("noTransition"));
		if (!m_playLayer || !m_playLayer->m_level) return;
		GJGameLevel* theLevel = m_playLayer->m_level;
		MyEndLevelLayer::applyHideEndLevelLayerHideBtn();
		MyEndLevelLayer::applyHideChainsBackground();
		MyEndLevelLayer::applySpaceUK();
		MyEndLevelLayer::applyPlatAttemptsAndJumpsOrFlukedFromPercent(theLevel);
		MyEndLevelLayer::addLoadedModsList();
	}
	void customSetup() {
		EndLevelLayer::customSetup();
		if (!getModBool("enabled")) return;
		managerMacro->isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		PlayLayer* playLayer = PlayLayer::get();
		if (!playLayer) return;
		if (getModBool("endTexts")) MyEndLevelLayer::applyRandomQuoteAndFont(playLayer, playLayer->m_level);
		if (getModBool("customLevelCompleteText")) MyEndLevelLayer::applyCustomLevelCompleteText(getModString("alsoReplacePlayLayerLCT"));
	}
};