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
            std::string modID = mod->getID();
            if (manager->modIDToModMenu.contains(modID)) {
                manager->loadedModMenus.push_back(modID);
            }
        });
        return result;
    }
};