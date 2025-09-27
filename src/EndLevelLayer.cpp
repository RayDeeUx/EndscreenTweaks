#include <Geode/modify/EndLevelLayer.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include "Manager.hpp"

#define managerMacro Manager::getSharedInstance()
#define getModBool Mod::get()->getSettingValue<bool>
#define getModInt Mod::get()->getSettingValue<int64_t>
#define getModDouble Mod::get()->getSettingValue<double>
#define getModString Mod::get()->getSettingValue<std::string>

using namespace geode::prelude;

class $modify(MyEndLevelLayer, EndLevelLayer) {
	static CCActionInterval* getEaseTypeForCustomScaleAnimation(CCActionInterval* action, const std::string& modStringSetting, const float easingRate) {
		if (!action) return nullptr;
		const std::string& easeType = utils::string::toLower(modStringSetting);

		if (easeType == "none (linear)" || easeType == "none" || easeType == "linear") return action;

		if (easeType == "ease in") return CCEaseIn::create(action, easingRate);
		if (easeType == "ease out") return CCEaseOut::create(action, easingRate);
		if (easeType == "ease in out") return CCEaseInOut::create(action, easingRate);

		if (easeType == "back in") return CCEaseBackIn::create(action);
		if (easeType == "back out") return CCEaseBackOut::create(action);
		if (easeType == "back in out") return CCEaseBackInOut::create(action);

		if (easeType == "bounce in") return CCEaseBounceIn::create(action);
		if (easeType == "bounce out") return CCEaseBounceOut::create(action);
		if (easeType == "bounce in out") return CCEaseBounceInOut::create(action);

		if (easeType == "elastic in") return CCEaseElasticIn::create(action, easingRate);
		if (easeType == "elastic out") return CCEaseElasticOut::create(action, easingRate);
		if (easeType == "elastic in out") return CCEaseElasticInOut::create(action, easingRate);

		if (easeType == "exponential in") return CCEaseExponentialIn::create(action);
		if (easeType == "exponential out") return CCEaseExponentialOut::create(action);
		if (easeType == "exponential in out") return CCEaseExponentialInOut::create(action);

		if (easeType == "sine in") return CCEaseSineIn::create(action);
		if (easeType == "sine out") return CCEaseSineOut::create(action);
		if (easeType == "sine in out") return CCEaseSineInOut::create(action);

		return CCEaseBounceOut::create(action);
	}
	void applyEditedTransitionsInitialFallDown() {
		if (!managerMacro->shouldEditTransition || !m_mainLayer) return;

		this->stopAllActions();
		this->m_mainLayer->stopAllActions();

		this->setOpacity(0);
		this->setCascadeOpacityEnabled(false);
		this->m_mainLayer->setPosition({this->getPositionX(), 320.f});

		const float duration = std::clamp<float>(getModDouble("initialFallDownDuration"), 0.f, 2.f);
		CCFadeTo* fadeToAction = CCFadeTo::create(duration, std::clamp<int>(getModInt("backdropOpacity"), 0, 255));
		CCSequence* fadeSequence = CCSequence::create(fadeToAction, nullptr);
		fadeSequence->setTag(12341);
		this->runAction(fadeSequence);

		CCMoveTo* moveToAction = CCMoveTo::create(duration, {0, 5.f});
		CCActionInterval* easedMoveToAction = MyEndLevelLayer::getEaseTypeForCustomScaleAnimation(
			moveToAction, getModString("initialFallDownEasingType"),
			std::clamp<float>(getModDouble("initialFallDownEasingRate"), .1f, 4.f)
		);
		CCCallFunc* callFunctn = CCCallFunc::create(this, callfunc_selector(EndLevelLayer::enterAnimFinished));
		CCSequence* moveDownSequence = CCSequence::create(easedMoveToAction, callFunctn, nullptr);
		moveDownSequence->setTag(12341);
		this->m_mainLayer->runAction(moveDownSequence);
	}
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
		if (!m_mainLayer) return;
		if (getModBool("hideChains")) {
			if (CCNode* left = m_mainLayer->getChildByID("chain-left")) static_cast<CCSprite*>(left)->setOpacity(0);
			if (CCNode* right = m_mainLayer->getChildByID("chain-right")) static_cast<CCSprite*>(right)->setOpacity(0);
		}
		if (getModBool("hideBackground")) {
			if (CCNode* bg = m_mainLayer->getChildByID("background")) {
				GJListLayer* background = static_cast<GJListLayer*>(bg);
				background->setCascadeOpacityEnabled(false);
				background->setOpacity(0);
				if (CCNode* left = bg->getChildByID("left-border")) static_cast<CCSprite*>(left)->setOpacity(0);
				if (CCNode* right = bg->getChildByID("right-border")) static_cast<CCSprite*>(right)->setOpacity(0);
				if (CCNode* top = bg->getChildByID("top-border")) static_cast<CCSprite*>(top)->setOpacity(0);
				if (CCNode* bottom = bg->getChildByID("bottom-border")) static_cast<CCSprite*>(bottom)->setOpacity(0);
				if (CCNode* title = bg->getChildByID("title")) static_cast<CCLabelBMFont*>(title)->setOpacity(0);
			}
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
			CCLabelBMFont* attemptsLabel = CCLabelBMFont::create(("Attempts: " + std::to_string(playLayer->m_attempts)).c_str(), "goldFont.fnt");
			attemptsLabel->setScale(0.8f);
			attemptsLabel->setPosition(timeLabel->getPositionX(), timeLabel->getPositionY() + 40);
			attemptsLabel->setID("attempts-label"_spr);
			m_mainLayer->addChild(attemptsLabel);
			CCLabelBMFont* jumpsLabel = CCLabelBMFont::create(("Jumps: " + std::to_string(playLayer->m_jumps)).c_str(), "goldFont.fnt");
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
		CCLabelBMFont* endTextLabel = typeinfo_cast<CCLabelBMFont*>(getChildByIDRecursive("end-text"));
		if (!endTextLabel) return;

		const int64_t fontID = Mod::get()->getSettingValue<int64_t>("customFont");
		if (fontID == -2) {
			endTextLabel->setFntFile("chatFont.fnt");
		} else if (fontID == -1) {
			endTextLabel->setFntFile("goldFont.fnt");
		} else if (fontID != 0) {
			endTextLabel->setFntFile(fmt::format("gjFont{:02d}.fnt", fontID).c_str());
		}

		const Manager* manager = managerMacro;
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
		modsListMenu->ignoreAnchorPointForPosition(false);
		modsListMenu->setPosition(m_listLayer->getPosition()); // i know this sounds like a crackpot position but it fits perfectly (im ignoring practice mode bc who tf wanna show that)
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
		const std::string& formattedTitle = fmt::format("{} Total ({} Loaded, {} Disabled, of which {} w/Problems)", manager->totalMods, manager->loadedMods, manager->disabledMods, manager->problemMods);
		MDPopup::create(formattedTitle, formattedList, "Copy", "Close", [formattedTitle, formattedList](const bool btn2){
			if (btn2) return;
			geode::utils::clipboard::write(fmt::format("{}\n{}", formattedTitle, formattedList));
			Notification::create("Copied mods list")->show();
		})->show();
	}
	void showLayer(bool p0) {
		if (!getModBool("enabled")) return EndLevelLayer::showLayer(p0);
		EndLevelLayer::showLayer(getModBool("noTransition"));
		if (!m_playLayer || !m_playLayer->m_level) return;
		MyEndLevelLayer::applyEditedTransitionsInitialFallDown();
		MyEndLevelLayer::applyHideEndLevelLayerHideBtn();
		MyEndLevelLayer::applyHideChainsBackground();
		MyEndLevelLayer::applySpaceUK();
		MyEndLevelLayer::applyPlatAttemptsAndJumpsOrFlukedFromPercent(m_playLayer->m_level);
		MyEndLevelLayer::addLoadedModsList();
	}
	void customSetup() {
		if (this->m_fastMenu || getModBool("noTransition")) managerMacro->shouldEditTransition = false;
		else managerMacro->shouldEditTransition = getModBool("editTransition");
		EndLevelLayer::customSetup();
		if (!getModBool("enabled")) return;
		// managerMacro->isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		if (!m_playLayer || !m_playLayer->m_level) return;
		if (getModBool("endTexts")) MyEndLevelLayer::applyRandomQuoteAndFont(m_playLayer, m_playLayer->m_level);
		if (getModBool("customLevelCompleteText")) MyEndLevelLayer::applyCustomLevelCompleteText(getModString("alsoReplacePlayLayerLCT"));
	}
	void onHideLayer(CCObject* sender) {
		if (!getModBool("enabled") || !managerMacro->shouldEditTransition) return EndLevelLayer::onHideLayer(sender);
	}
};