#pragma once

#include "Manager.hpp"

#define getModBool Mod::get()->getSettingValue<bool>
#define managerMacro Manager::getSharedInstance()
#define configDir Mod::get()->getConfigDir()

inline void addQuotes(const std::string& settingName) {
	Manager* manager = managerMacro;
	if (settingName == "custom" && getModBool(settingName)) {
		log::info("adding custom.txt quotes");
		auto pathCustomConfigDir = (configDir / "custom.txt");
		std::ifstream fileConfigDir(pathCustomConfigDir);
		std::string str;
		while (std::getline(fileConfigDir, str)) {
			if (str.starts_with("\"") && str.ends_with("\""))
				str = str.replace(0, 1, "\'\'");
			else if (str.starts_with("\'") && str.ends_with("\'"))
				str = str.replace(0, 2, "\"");
			if (!Mod::get()->getSavedValue<bool>("noHyphens")) str = fmt::format("- {} -", str);
			manager->quotes.push_back(str);
			manager->customQuotes.push_back(str);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	} else if (getModBool(settingName)) {
		log::info("adding quotes from {}", settingName);
		auto settingAsFileName = fmt::format("{}.txt", settingName);
		auto filePath = (Mod::get()->getResourcesDir() / settingAsFileName).string();
		std::ifstream fileStream(filePath);
		std::string lineOfText;
		while (std::getline(fileStream, lineOfText)) manager->quotes.push_back(lineOfText);
	}
}

inline void setupLevelCompleteTexts() {
	Manager* manager = managerMacro;
	if (!manager->levelCompleteQuotes.empty()) manager->levelCompleteQuotes.clear();
	if (!manager->customLevelCompleteQuotes.empty()) manager->customLevelCompleteQuotes.clear();
	auto pathDefaultLevelComplete = (Mod::get()->getResourcesDir() / "defaultLevelComplete.txt");
	std::ifstream defaultLCQuoteFile(pathDefaultLevelComplete);
	std::string defaultLCQuote;
	while (std::getline(defaultLCQuoteFile, defaultLCQuote)) {
		manager->levelCompleteQuotes.push_back(defaultLCQuote);
		log::info("added default levelcomplete quote: {}", defaultLCQuote);
	}
	auto pathCustomLvlCompleteQuotes = (configDir / "customLevelCompleteQuotes.txt");
	if (std::filesystem::exists(pathCustomLvlCompleteQuotes)) {
		std::ifstream customLCQuoteFile(pathCustomLvlCompleteQuotes);
		std::string customLCQuote;
		while (std::getline(customLCQuoteFile, customLCQuote)) {
			const std::string toAdd = geode::utils::string::toUpper(geode::utils::string::replace(customLCQuote, "\"", "\'\'"));
			manager->levelCompleteQuotes.push_back(toAdd);
			manager->customLevelCompleteQuotes.push_back(toAdd);
			log::info("added custom levelcomplete quote: {}", toAdd);
		}
	} else {
		std::string content = R"(insert funny text here
(yes, you can delete both lines in this text file))";
		(void) utils::file::writeString(pathCustomLvlCompleteQuotes, content);
	}
}

inline void addResourceQuotes() {
	addQuotes("default");
	addQuotes("technoblade");
	addQuotes("snl50");
}

inline void addCustomQuotesAndLevelCompleteTests() {
	addQuotes("custom");
	setupLevelCompleteTexts();
}

inline void managerReset() {
	Manager* manager = managerMacro;
	if (!manager->quotes.empty()) manager->quotes.clear();
	if (!manager->customQuotes.empty()) manager->customQuotes.clear();
	addResourceQuotes();
	addCustomQuotesAndLevelCompleteTests();
}