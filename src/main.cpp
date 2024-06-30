/*
#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/PlayerObject.hpp>
#elif __APPLE__
#include <Geode/modify/GJBaseGameLayer.hpp>
#endif
*/
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <regex>

using namespace geode::prelude;

std::vector<std::string> quotes;
std::vector<std::string> wETMigration;

auto fallbackString = "We've got too many players to congratulate on level completions. Beat this level again for an actual message.";

int fps = -1;

bool isCompactEndscreen;
bool isGDMO;
float compactEndscreenFallbackPosition = CCDirector::get()->getWinSize().width * 0.6f;

$on_mod(Loaded) {
	auto path = (Mod::get()->getResourcesDir() / "default.txt");
	std::ifstream file(path);
	std::string str;
	while (std::getline(file, str)) { quotes.push_back(str); }

	if (Mod::get()->getSettingValue<bool>("technoblade")) {
		auto pathRogers = (Mod::get()->getResourcesDir() / "technoblade.txt");
		std::ifstream file(pathRogers);
		std::string technoblade;
		while (std::getline(file, technoblade)) {
			quotes.push_back(technoblade);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	}

	auto oldWETMessages = (dirs::getModConfigDir() / "raydeeux.wholesomeendtexts" / "custom.txt");
	if (std::filesystem::exists(oldWETMessages) && !Mod::get()->getSavedValue<bool>("migrationFromWETSuccess")) {
		log::info("std::filesystem::exists(oldWETMessages): {}", std::filesystem::exists(oldWETMessages));
		log::info("Storing oldWETMessages now.");
		std::ifstream wETFile(oldWETMessages);
		std::string wETStr;
		while (std::getline(wETFile, wETStr)) {
			wETMigration.push_back(wETStr);
		}
		log::info("Finished storing oldWETMessages.");
	}

	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514 --erymanthus | raydeeux

	auto path3 = (Mod::get()->getConfigDir() / "custom.txt");
	if (!std::filesystem::exists(path3)) {
		if (wETMigration.empty()) {
			log::info("wETMigration was empty. Confirm \"std::filesystem::exists(oldWETMessages)\" didn't appear earlier in the logs.");
			std::string content = R"(lorem ipsum
abc def
u beat the level
gg gaming)";
			utils::file::writeString(path3, content);
		} else if (std::filesystem::exists(oldWETMessages)) {
			if (!wETMigration.empty()) {
				log::info("Starting to migrate custom messages from WholesomeEndTexts.");
				/*
				for (std::string wETCustomMessage : wETMigration) {
					// std::string stringToMigrate = wETCustomMessage;
					utils::file::writeString(path3,  fmt::format("{}\n", wETCustomMessage));
				}
				*/
				std::filesystem::copy(oldWETMessages, path3);
				log::info("Finished migrating messages from WholesomeEndTexts. Confirm nothing went terribly wrong.");
				Mod::get()->setSavedValue("migrationFromWETSuccess", true);
			} else {
				log::error("Migration failed! What happened?");
				std::string content = R"(migration failed, womp womp
migration failed, womp womp
migration failed, womp womp
migration failed, womp womp)";
			utils::file::writeString(path3, content);
			}
		}
	}
	
	if (Mod::get()->getSettingValue<bool>("custom")) {
		auto pathCustom = (Mod::get()->getConfigDir() / "custom.txt");
		std::ifstream file(pathCustom);
		std::string str;
		while (std::getline(file, str)) {
			std::string customStr = "- " + str + " -";
			quotes.push_back(customStr);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	}
}

const char* grabRandomQuote() {
	std::mt19937 randomSeed(std::random_device{}());
	std::shuffle(quotes.begin(), quotes.end(), randomSeed);
	return quotes.front().c_str();
}

class $modify(CCScheduler) {
	void update(float dt) {
		if (PlayLayer::get()) fps = (int)(CCScheduler::get()->getTimeScale() / dt) + 1;
		CCScheduler::update(dt);
	}
};

/*
int attempts;
int jumps = 0;

#ifdef GEODE_IS_WINDOWS
class $modify(PlayerObject) {
	void incrementJumps() {
		PlayerObject::incrementJumps();
		jumps += 1;
	}
};
#elif __APPLE__
class $modify(GJBaseGameLayer) {
	void toggleDualMode(GameObject* p0, bool p1, PlayerObject* p2, bool p3) {
		GJBaseGameLayer::toggleDualMode(p0, p1, p2, p3);
		if (PlayLayer::get()) {
			attempts += 1;
		} else {
			attempts = 0;
		}
	}
};
#endif

class $modify(PlayLayer) {
	void onQuit() {
		PlayLayer::onQuit(); // call the original function
		jumps = 0;
	}
	void fullReset() {
		PlayLayer::fullReset(); // call the original function
		attempts = 1;
		jumps = 0;
	}
#ifndef __APPLE__
	void updateAttempts() {
		PlayLayer::updateAttempts();
		attempts += 1;
	}
#endif
	void onEnterTransitionDidFinish() {
		PlayLayer::onEnterTransitionDidFinish();
		attempts = 1;
	}
#ifndef GEODE_IS_WINDOWS
	void incrementJumps() {
		PlayLayer::incrementJumps();
		jumps += 1;
	}
#endif
};
*/

class $modify(MyEndLevelLayer, EndLevelLayer) {
	/*
	static void onModify(auto & self)
	{
		// i wanted to have compat with relative's endscreen text but better safe than sorry :)
		self.setHookPriority("EndLevelLayer::showLayer", INT32_MAX - 1);
		self.setHookPriority("EndLevelLayer::customSetup", INT32_MAX - 1);
	}
	*/
	void showLayer(bool p0) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) {
			EndLevelLayer::showLayer(p0);
			return;
		}
		isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		isGDMO = Loader::get()->isModLoaded("maxnu.gd_mega_overlay");
		EndLevelLayer::showLayer(Mod::get()->getSettingValue<bool>("noTransition"));
		auto theLevel = PlayLayer::get()->m_level;
		if (Mod::get()->getSettingValue<bool>("hideChains")) {
			getChildByIDRecursive("chain-left")->setVisible(false);
			getChildByIDRecursive("chain-right")->setVisible(false);
		}
		if (Mod::get()->getSettingValue<bool>("hideBackground")) { getChildByIDRecursive("background")->setVisible(false); }
		if (Mod::get()->getSettingValue<bool>("hideHideEndscreen") && Loader::get()->isModLoaded("absolllute.megahack") && getChildByIDRecursive("absolllute.megahack/hide-endscreen")) {
			getChildByIDRecursive("hide-button")->setVisible(false);
		}
		if (Mod::get()->getSettingValue<bool>("spaceUK")) {
			auto levelCompleteText = getChildByIDRecursive("level-complete-text");
			if (levelCompleteText == nullptr) levelCompleteText = getChildByIDRecursive("practice-complete-text"); // grab practice mode complete text as fallback node
			if (isCompactEndscreen) {
				levelCompleteText->setVisible(true);
				levelCompleteText->setPositionX(compactEndscreenFallbackPosition);
			}
			levelCompleteText->setScale(0.8f * (940.f / 1004.f)); // original scale of this node is 0.8 according to logs. hardcoding it here in case other mods decide to scale it to whatever else
		}
		if (Mod::get()->getSettingValue<bool>("platAttemptsAndJumps") && theLevel->isPlatformer()) {
			auto mainLayer = getChildByID("main-layer");
			if (mainLayer == nullptr) return;
			auto playLayer = PlayLayer::get();
			if (playLayer == nullptr) return;
			auto timeLabel = getChildByIDRecursive("time-label");
			auto pointsLabel = getChildByIDRecursive("points-label");
			if (!isCompactEndscreen) timeLabel->setPositionY(timeLabel->getPositionY() - 20);
			if (pointsLabel) pointsLabel->setPositionY(timeLabel->getPositionY() - 18);
			auto attemptsLabel = cocos2d::CCLabelBMFont::create(("Attempts: " + std::to_string(playLayer->m_attempts)).c_str(), "goldFont.fnt");
			attemptsLabel->setScale(0.8f);
			attemptsLabel->setPositionX(timeLabel->getPositionX());
			attemptsLabel->setPositionY(timeLabel->getPositionY() + 40);
			attemptsLabel->setID("attempts-label"_spr);
			mainLayer->addChild(attemptsLabel);
			mainLayer->updateLayout();
			auto jumpsLabel = cocos2d::CCLabelBMFont::create(("Jumps: " + std::to_string(playLayer->m_jumps)).c_str(), "goldFont.fnt");
			jumpsLabel->setScale(0.8f);
			jumpsLabel->setPositionX(timeLabel->getPositionX());
			jumpsLabel->setPositionY(timeLabel->getPositionY() + 20);
			jumpsLabel->setID("jumps-label"_spr);
			mainLayer->addChild(jumpsLabel);
			mainLayer->updateLayout();
		}
		if (isGDMO && theLevel->m_coins == 0 && Loader::get()->getLoadedMod("maxnu.gd_mega_overlay")->getSavedValue<bool>("level/endlevellayerinfo/enabled")) {
			/* 
				gdmo does this silly thing where they add children without giving them node IDs and i need to release this mod ASAP so please forgive me for using getobjectatindex but getchildoftype doesnt work for this use case because everything in endscreen layer is a child of some other cclayer smh
				auto mainLayer = getChildByID("main-layer");
				if (mainLayer == nullptr) return;
				auto mainLayerChildren = mainLayer->getChildren();
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
			*/
			// backup plan starts below
			float windowWidth = getChildByIDRecursive("background")->getContentSize().width;
			float windowHeight = CCDirector::get()->getWinSize().height;
			float offset = getChildByIDRecursive("background")->getPositionX();
			auto starContainer = getChildByIDRecursive("star-container");
			if (starContainer == nullptr) starContainer = getChildByIDRecursive("moon-container");
			float gdmoHeight = windowHeight * (285.f / 320.f);
			float gdmoTwentyFivePercentX = (windowWidth * .25f) + offset;
			float gdmoFiftyPercentX = (windowWidth * .5f) + offset;
			float gdmoSeventyFivePercentX = (windowWidth * .75f) + offset;
			if (starContainer) {
				if (theLevel->m_stars.value() == 1) starContainer->setPositionX(gdmoFiftyPercentX);
				else starContainer->setPositionX(gdmoTwentyFivePercentX);
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
	void customSetup() {
		EndLevelLayer::customSetup();
		if (!Mod::get()->getSettingValue<bool>("enabled")) { return; }
		isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
		auto playLayer = PlayLayer::get();
		if (playLayer == nullptr) { return; }
		auto theLevel = playLayer->m_level;
		if (auto completeMessage = typeinfo_cast<TextArea*>(getChildByIDRecursive("complete-message"))) {
			// ensure that no one's up to any funny business by hardcoding the scale and contents of vanilla complete messages 
			completeMessage->setScale(0.55f);
			if (getChildByIDRecursive("level-complete-text")) {
				if (theLevel->m_isVerifiedRaw) { completeMessage->setString("Level Verified!"); }
				else { completeMessage->setString("You cannot verify a level if it\nhas a start pos."); }
			} else {
				if (theLevel->m_levelID.value() == 0) { completeMessage->setString("Complete the level in normal mode\nto verify it!"); }
				else { completeMessage->setString("Well done... Now try to complete\nit without any checkpoints!"); }
			}
			if (isCompactEndscreen) completeMessage->setPositionX(compactEndscreenFallbackPosition);
			return;
		}
		
		auto randomString = grabRandomQuote();
		
		if (auto endText = getChildByIDRecursive("end-text")) {
			auto endTextLabel = typeinfo_cast<CCLabelBMFont*>(endText);
			
			if (strcmp("Make sure to screenshot this win!", randomString) == 0) {
				#ifdef GEODE_IS_MACOS
					randomString = "Press Command + Shift + 3 to screenshot this win!";
				#endif
				#ifdef GEODE_IS_ANDROID
					randomString = "Press the \"Volume Down\'\' and \"Power\'\' buttons to screenshot this win!";
				#endif
				#ifdef GEODE_IS_WINDOWS
					randomString = "Press \"Win + Shift + S\'\' or \"PrtSc\'\' to screenshot this win!";
				#endif
			} else if (strcmp("\"First try, part two!\"", randomString) == 0) {
				std::string temp = fmt::format("\'\'First try, part {}!\"", playLayer->m_attempts);
				if (playLayer->m_attempts == 1) { temp = "\"First try!\""; }
				randomString = temp.c_str();
			} else if (strcmp("\'\'As you can see, my FPS is around 18 or so, which means we can definitely take this further.\"", randomString) == 0) {
				randomString = fmt::format("\'\'As you can see, my FPS is around {} or so, which means we can definitely take this further.\"", fps).c_str();
			} else if (strcmp("\'\'If you wish to defeat me, train for another 100 years.\"", randomString) == 0) {
				randomString = fmt::format("\'\'If you wish to defeat me, train for another {} years.\"", std::max(100, (playLayer->m_jumps * 100))).c_str();
			} else if (strcmp("Good luck on that statgrinding session!", randomString) == 0 && theLevel->m_stars.value() != 0) {
				if (theLevel->isPlatformer()) { randomString = "Good luck on that moongrinding session!"; }
				else { randomString = "Good luck on that stargrinding session!"; }
			}
		
			float scale = 0.36f * 228.f / strlen(randomString);
			if (strcmp("BELIEVE", randomString) == 0) { scale = 1.5f; }
			else if (strcmp("endTextLabel->setString(randomString.c_str(), true);", randomString) == 0) { scale = 0.4f;}
			else if (scale > Mod::get()->getSettingValue<double>("maxScale")) { scale = Mod::get()->getSettingValue<double>("maxScale"); }

			endTextLabel->setScale(scale);
			endTextLabel->setWidth(336.f); // width of end screen minus 20px
			
			endTextLabel->setString(randomString, true); // set string
			endTextLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter); // center text

			if (isCompactEndscreen) endTextLabel->setPositionX(compactEndscreenFallbackPosition);
			if (strcmp("", randomString) == 0) { endTextLabel->setString(fallbackString, true); } // fallback string
		}
	}
};
