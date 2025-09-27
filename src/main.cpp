#include <fstream>
#include "Manager.hpp"
#include "Settings.hpp"
#include "boilerplate.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
	Mod::get()->setLoggingEnabled(Mod::get()->getSettingValue<bool>("logging"));
	Manager* manager = managerMacro;
	manager->hideEndLevelLayer = Mod::get()->getSettingValue<bool>("hideEndLevelLayer");
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	(void) Mod::get()->registerCustomSettingType("refresh", &MyButtonSettingV3::parse);
	if (!std::filesystem::exists((configDir / R"(levelCompleteImages)"))) {
		std::filesystem::create_directory(configDir / R"(levelCompleteImages)");
	}
	if (!std::filesystem::exists((configDir / R"(disabled_level_complete_images)"))) {
		std::filesystem::create_directory(configDir / R"(disabled_level_complete_images)");
	}

	addResourceQuotes();

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
[make sure to edit this file!])";
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

	addCustomQuotesAndLevelCompleteTests();

	listenForSettingChanges("default", [](bool) {
		managerReset();
	});

	listenForSettingChanges("technoblade", [](bool) {
		managerReset();
	});

	listenForSettingChanges("snl50", [](bool) {
		managerReset();
	});

	listenForSettingChanges("custom", [](bool) {
		managerReset();
	});

	listenForSettingChanges("hideEndLevelLayer", [](const bool hideEndLevelLayerNew) {
		Manager::getSharedInstance()->hideEndLevelLayer = hideEndLevelLayerNew;
	});
}