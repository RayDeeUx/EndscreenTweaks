#include <fstream>
#include "Manager.hpp"
#include "Settings.hpp"

#define getModBool Mod::get()->getSettingValue<bool>
#define managerMacro Manager::getSharedInstance()
#define configDir Mod::get()->getConfigDir()

using namespace geode::prelude;

$on_mod(Loaded) {
	Manager* manager = managerMacro;
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	if (!std::filesystem::exists((configDir / R"(levelCompleteImages)"))) {
		std::filesystem::create_directory(configDir / R"(levelCompleteImages)");
	}
	if (!std::filesystem::exists((configDir / R"(disabled_level_complete_images)"))) {
		std::filesystem::create_directory(configDir / R"(disabled_level_complete_images)");
	}
	auto pathDefault = (Mod::get()->getResourcesDir() / "default.txt");
	std::ifstream file(pathDefault);
	std::string placeHolder;
	while (std::getline(file, placeHolder)) {
		manager->quotes.push_back(placeHolder);
	}

	if (getModBool("technoblade")) {
		auto pathTechnoblade = (Mod::get()->getResourcesDir() / "technoblade.txt");
		std::ifstream fileTechnoblade(pathTechnoblade);
		std::string technoblade;
		while (std::getline(fileTechnoblade, technoblade)) {
			manager->quotes.push_back(technoblade);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	}

	auto oldWETMessages = (dirs::getModConfigDir() / "raydeeux.wholesomeendtexts" / "custom.txt");
	if (std::filesystem::exists(oldWETMessages) && !Mod::get()->getSavedValue<bool>("migrationFromWETSuccess")) {
		log::info("std::filesystem::exists(oldWETMessages): {}", std::filesystem::exists(oldWETMessages));
		log::info("Storing oldWETMessages now.");
		std::ifstream wETFile(oldWETMessages);
		std::string wETStr;
		while (std::getline(wETFile, wETStr)) {
			manager->wETMigration.push_back(wETStr);
			log::info("{}", wETStr);
		}
		log::info("Finished storing oldWETMessages.");
	}

	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514 --erymanthus | raydeeux

	auto pathCustom = (configDir / "custom.txt");
	if (!std::filesystem::exists(pathCustom)) {
		log::info("customtxt does not exist. wETMigration.empty(): {} | std::filesystem::exists(oldWETMessages): {}", manager->wETMigration.empty(), std::filesystem::exists(oldWETMessages));
		if (manager->wETMigration.empty()) {
			log::info("wETMigration was empty. Confirm \"std::filesystem::exists(oldWETMessages)\" didn't appear earlier in the logs.");
			std::string content = R"(lorem ipsum
abc def
u beat the level
gg gaming
[this text file was brought to you by endscreentweaks. if you're seeing this in the config directory for wholesomeendtexts, something has gone terribly TERRIBLY wrong-simply ping @erymanthus in the geode sdk discord server and do not panic!])";
			(void) utils::file::writeString(pathCustom, content);
		} else if (std::filesystem::exists(oldWETMessages)) {
			if (!manager->wETMigration.empty()) {
				log::info("Migrating custom messages from WholesomeEndTexts. Buckle up!");
				/*
				for (std::string wETCustomMessage : wETMigration) {
					// std::string stringToMigrate = wETCustomMessage;
					utils::file::writeString(pathCustom,  fmt::format("{}\n", wETCustomMessage));
				}
				*/
				std::filesystem::copy(oldWETMessages, pathCustom);
				log::info("Finished migrating messages from WholesomeEndTexts. Confirm nothing went terribly wrong.");
				Mod::get()->setSavedValue("migrationFromWETSuccess", true);
			} else {
				log::error("Migration failed! What happened?");
				std::string content = R"(migration failed, womp womp
migration failed, womp womp
migration failed, womp womp
migration failed, womp womp)";
				(void) utils::file::writeString(pathCustom, content);
			}
		}
	}

	if (getModBool("custom")) {
		auto pathCustomConfigDir = (configDir / "custom.txt");
		std::ifstream fileConfigDir(pathCustomConfigDir);
		std::string str;
		while (std::getline(fileConfigDir, str)) {
			if (str.starts_with("\"") && str.ends_with("\"")) {
				str = str.replace(0, 1, "\'\'");
			} else if (str.starts_with("\'") && str.ends_with("\'")) {
				str = str.replace(0, 2, "\"");
			}
			if (!Mod::get()->getSavedValue<bool>("noHyphens")) {
				str = fmt::format("- {} -", str);
			}
			manager->quotes.push_back(str);
			manager->customQuotes.push_back(str);
		} // technically i can write two one-time use boolean variables to allow people to toggle these things on and off as they please without the quotes adding themselves multiple times into the vector, but i'd rather add the "restart required" barrier just to be extra safe
	}

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