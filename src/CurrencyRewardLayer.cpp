#include <Geode/modify/CurrencyRewardLayer.hpp>

using namespace geode::prelude;

class $modify(MyCurrencyRewardLayer, CurrencyRewardLayer) {
	/*
	THIS is apparently what i need to hook
	for hiding the CurrencyRewardLayer consistently.
	what the hell, robtop?
	*/
	void update(float p0) {
		bool isHideEndLevelLayer = Mod::get()->getSettingValue<bool>("hideEndLevelLayer");
		bool isChildOfEndLevelLayer = typeinfo_cast<EndLevelLayer*>(this->getParent());
		if (isHideEndLevelLayer && isChildOfEndLevelLayer) this->setVisible(false);
		CurrencyRewardLayer::update(p0);
	}
};