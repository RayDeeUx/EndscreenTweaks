#include <Geode/modify/MenuLayer.hpp>
#include "Manager.hpp"

#define managerMacro Manager::getSharedInstance()
#define getModBool Mod::get()->getSettingValue<bool>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		const auto mods = Loader::get()->getAllMods();
		Manager* manager = managerMacro;
		manager->totalMods = mods.size();
		std::ranges::for_each(mods, [&](const Mod* mod) {
			std::optional<LoadProblem> problem = mod->getLoadProblem();
			if (!problem && mod->isLoaded()) manager->loadedMods += 1;
			else if (!mod->isLoaded() && !problem) manager->disabledMods += 1;

			const std::string& modID = mod->getID();

			if (manager->modIDToModMenu.contains(modID)) manager->loadedModMenus.push_back(modID);

			std::string formattedByline = "by ";
			for (const std::string& developer : mod->getDevelopers()) {
				formattedByline = formattedByline.append(fmt::format("{}, ", developer));
			}
			if (!formattedByline.empty() && utils::string::endsWith(formattedByline, ", ")) {
				formattedByline.pop_back();
				formattedByline.pop_back();
			}

			std::string formattedProblemsList;
			if (problem && problem->isProblemTheUserShouldCareAbout()) {
				manager->problemMods += 1;
				formattedProblemsList = fmt::format(" {{<co>{}</c>}}", problem->message);
			}

			const std::string& formattedModListItem = fmt::format(
				"- {}{}</c> {} {} [{}]{}",
				mod->isLoaded() ? "<c-FFFFFF>" : !problem ? "<c-AAAAAA>" : "<c-FF0000>",
				mod->getName(), mod->getVersion().toVString(), formattedByline,
				modID, formattedProblemsList
			);

			manager->formattedModsListVector.push_back(formattedModListItem);
		});

		return true;
	}
};