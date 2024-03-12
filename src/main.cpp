#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>

using namespace geode::prelude;

std::vector<std::string> quotes;

auto fallbackString = "We've got too many players to congratulate on level completions. Beat this level again for an actual message.";

int attempts;
int jumps = 0;
int fps = -1;

$execute {
	auto path = (Mod::get()->getResourcesDir() / "default.txt").string();
	std::ifstream file(path);
    std::string str;
    while (std::getline(file, str)) { quotes.push_back(str); }

	if (Mod::get()->getSettingValue<bool>("technoblade")) {
		auto pathRogers = (Mod::get()->getResourcesDir() / "technoblade.txt").string();
		std::ifstream file(pathRogers);
		std::string technoblade;
		while (std::getline(file, technoblade)) {
			std::string tThePig = "\"" + technoblade + "\"";
			quotes.push_back(tThePig);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	}


	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514 --erymanthus | raydeeux

	auto path3 = (Mod::get()->getConfigDir() / "custom.txt").string();
	if (!std::filesystem::exists(path3)) {
		std::string content = R"(lorem ipsum
abc def
u beat the level
gg gaming)";
		utils::file::writeString(path3, content);
	}
	
    if (Mod::get()->getSettingValue<bool>("custom")) {
		auto pathCustom = (Mod::get()->getConfigDir() / "custom.txt").string();
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
		if (GameManager::sharedState()->getPlayLayer() != nullptr) fps = (int)(CCScheduler::get()->getTimeScale() / dt) + 1;
		CCScheduler::update(dt);
	}
};

#ifndef GEODE_IS_MACOS
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
	void updateAttempts() {
		PlayLayer::updateAttempts();
		attempts += 1;
	}
	void onEnterTransitionDidFinish() {
		PlayLayer::onEnterTransitionDidFinish();
		attempts = 1;
	}
	void incrementJumps() {
		PlayLayer::incrementJumps();
		jumps += 1;
	}
};
#endif

class $modify(MyEndLevelLayer, EndLevelLayer) {
	bool isCompactEndscreen = Loader::get()->isModLoaded("suntle.compactendscreen");
	float compactEndscreenFallbackPosition = CCDirector::get()->getWinSize().width * 0.6f;
	void showLayer(bool p0) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) {
			EndLevelLayer::showLayer(p0);
			return;
		}
		EndLevelLayer::showLayer(Mod::get()->getSettingValue<bool>("noTransition"));
		auto theLevel = GameManager::sharedState()->getPlayLayer()->m_level;
		if (Mod::get()->getSettingValue<bool>("hideChains")) {
			getChildByIDRecursive("chain-left")->setVisible(false);
			getChildByIDRecursive("chain-right")->setVisible(false);
		}
		if (Mod::get()->getSettingValue<bool>("hideBackground")) { getChildByIDRecursive("background")->setVisible(false); }
		if (Mod::get()->getSettingValue<bool>("hideHideEndscreen") && Loader::get()->isModLoaded("absolllute.megahack") && getChildByIDRecursive("absolllute.megahack/hide-endscreen")) {
			typeinfo_cast<CCSprite*>(getChildByIDRecursive("absolllute.megahack/hide-endscreen")->getChildren()->objectAtIndex(0))->setVisible(false); // hide the sprite, not the whole button. otherwise unclickable
		}
		if (Mod::get()->getSettingValue<bool>("spaceUK")) {
			auto levelCompleteText = getChildByIDRecursive("level-complete-text");
			if (levelCompleteText == nullptr) levelCompleteText = getChildByIDRecursive("practice-complete-text"); // grab practice mode complete text as fallback node
			if (m_fields->isCompactEndscreen) {
				levelCompleteText->setVisible(true);
				levelCompleteText->setPositionX(m_fields->compactEndscreenFallbackPosition);
			}
			levelCompleteText->setScale(0.8f * (940.f / 1004.f)); // original scale of this node is 0.8 according to logs. hardcoding it here in case other mods decide to scale it to whatever else
		}
		if (Mod::get()->getSettingValue<bool>("platAttemptsAndJumps") && theLevel->isPlatformer()) {
			auto mainLayer = getChildByID("main-layer");
			if (mainLayer == nullptr) return;
			auto timeLabel = getChildByIDRecursive("time-label");
			auto pointsLabel = getChildByIDRecursive("points-label");
			if (!m_fields->isCompactEndscreen) timeLabel->setPositionY(timeLabel->getPositionY() - 20);
			if (!m_fields->isCompactEndscreen && pointsLabel) pointsLabel->setPositionY(pointsLabel->getPositionY() - 14);
			auto attemptsLabel = cocos2d::CCLabelBMFont::create(("Attempts: " + std::to_string(attempts)).c_str(), "goldFont.fnt");
			attemptsLabel->setScale(0.8f);
			attemptsLabel->setPositionX(timeLabel->getPositionX());
			attemptsLabel->setPositionY(timeLabel->getPositionY() + 40);
			attemptsLabel->setID("attempts-label"_spr);
			mainLayer->addChild(attemptsLabel);
			mainLayer->updateLayout();
			auto jumpsLabel = cocos2d::CCLabelBMFont::create(("Jumps: " + std::to_string(jumps)).c_str(), "goldFont.fnt");
			jumpsLabel->setScale(0.8f);
			jumpsLabel->setPositionX(timeLabel->getPositionX());
			jumpsLabel->setPositionY(timeLabel->getPositionY() + 20);
			jumpsLabel->setID("jumps-label"_spr);
			mainLayer->addChild(jumpsLabel);
			mainLayer->updateLayout();
		}
	}
	void customSetup() {
		EndLevelLayer::customSetup();
		if (!Mod::get()->getSettingValue<bool>("enabled")) { return; }
		auto theLevel = GameManager::sharedState()->getPlayLayer()->m_level;
		if (auto completeMessage = typeinfo_cast<TextArea*>(getChildByIDRecursive("complete-message"))) {
			// ensure that no one's up to any funny business by hardcoding the scale and contents of vanilla complete messages 
			completeMessage->setScale(0.55f);
			if (getChildByIDRecursive("level-complete-text")) { completeMessage->setString("Level Verified!"); }
			else { completeMessage->setString("Complete the level in normal mode\nto verify it!"); }
			if (m_fields->isCompactEndscreen) completeMessage->setPositionX(m_fields->compactEndscreenFallbackPosition);
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
					randomString = "Press the \"Volume Down\" and \"Power\" buttons to screenshot this win!";
				#endif
				#ifdef GEODE_IS_WINDOWS
					randomString = "Press \"Win + Shift + S\" or \"PrtSc\" to screenshot this win!";
				#endif
			} else if (strcmp("\"First try, part two!\"", randomString) == 0) {
				#ifndef GEODE_IS_MACOS
					std::string temp = "\"First try, part " + std::to_string(attempts) + "!\"";
					if (attempts == 1) temp = "\"First try!\"";
					randomString = temp.c_str();
				#endif
			} else if (strcmp("\"As you can see, my FPS is around 18 or so, which means we can definitely take this further.\"", randomString) == 0) {
				#ifndef GEODE_IS_MACOS
					std::string temp = "\"As you can see, my FPS is around " + std::to_string(fps) + " or so, which means we can definitely take this further.\"";
					randomString = temp.c_str();
				#endif
			} else if (strcmp("\"If you wish to defeat me, train for another 100 years.\"", randomString) == 0) {
				#ifndef GEODE_IS_MACOS
					int forEndString = (jumps * 100);
					if (jumps == 0) forEndString = 100;
					std::string temp = "\"If you wish to defeat me, train for another " + std::to_string(forEndString) + " years.\"";
					randomString = temp.c_str();
				#endif
			}

            endTextLabel->setString(randomString, true); // set string
            endTextLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter); // center text

            float scale = 0.36f * (228.f / strlen(randomString));
			if (strcmp("BELIEVE", randomString) == 0) scale = 1.5f;
			else if (strcmp("endTextLabel->setString(randomString.c_str(), true);", randomString) == 0) scale = 0.4f;
			else if (scale > 0.5f) scale = 0.5f;
			endTextLabel->setScale(scale);
            endTextLabel->setWidth(336.f); // width of end screen minus 20px

			if (m_fields->isCompactEndscreen) endTextLabel->setPositionX(m_fields->compactEndscreenFallbackPosition);
			if (strcmp("", randomString) == 0) { endTextLabel->setString(fallbackString, true); } // fallback string
        }
	}
};