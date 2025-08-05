#include <Geode/modify/MenuLayer.hpp>
#include "Manager.hpp"

#define managerMacro Manager::getSharedInstance()
#define getModBool Mod::get()->getSettingValue<bool>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		bool result = MenuLayer::init();
		const auto mods = Loader::get()->getAllMods();
		Manager* manager = managerMacro;
		std::ranges::for_each(mods, [&](const Mod* mod) {
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

			std::string formattedProblemsList = "";
			if (std::vector<LoadProblem> problems = mod->getAllProblems(); !problems.empty()) {
				formattedProblemsList = " {";
				for (const auto [type, cause, message] : problems) {
					if (type == LoadProblem::Type::Suggestion || type == LoadProblem::Type::Recommendation) continue;
					std::string colorTag = "<c-FF0000>";
					if (type == LoadProblem::Type::Conflict || type == LoadProblem::Type::Duplicate || type == LoadProblem::Type::DisabledDependency || type == LoadProblem::Type::MissingDependency) colorTag = "<cy>";
					else if (type == LoadProblem::Type::UnsupportedVersion || type == LoadProblem::Type::UnsupportedGeodeVersion || type == LoadProblem::Type::NeedsNewerGeodeVersion) colorTag = "<c-AAAAAA>";
					formattedProblemsList = formattedProblemsList.append(fmt::format("{}{}</c>, ", colorTag, message));
				}
			   	if (!formattedProblemsList.empty() && utils::string::endsWith(formattedProblemsList, ", ")) {
					formattedProblemsList.pop_back();
					formattedProblemsList.pop_back();
			   	}
			   	formattedProblemsList = formattedProblemsList.append("}");
			}

			const std::string& formattedModListItem = fmt::format(
				"- {}{}</c> {} {} [{}]{}",
				mod->isEnabled() ? "<cg>" : "<c-FF0000>",
				mod->getName(), mod->getVersion().toVString(), formattedByline,
				modID, formattedProblemsList
			);

			manager->formattedModsListVector.push_back(formattedModListItem);
			manager->modsListFormatted = manager->modsListFormatted.append(fmt::format("{}\n", formattedModListItem));
		});
		log::info("manager->modsListFormatted:\n{}", manager->modsListFormatted);
		return result;
	}
};