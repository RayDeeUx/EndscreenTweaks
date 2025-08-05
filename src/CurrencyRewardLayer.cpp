#include <Geode/modify/CurrencyRewardLayer.hpp>

#include "Manager.hpp"

using namespace geode::prelude;

class $modify(MyCurrencyRewardLayer, CurrencyRewardLayer) {
	/*
	THIS is apparently what i need to hook
	for hiding the CurrencyRewardLayer consistently.
	what the hell, robtop?
	*/
	void update(float p0) {
		const bool isHideEndLevelLayer = Manager::getSharedInstance()->hideEndLevelLayer;
		const EndLevelLayer* endLevelLayer = typeinfo_cast<EndLevelLayer*>(this->getParent());
		if (isHideEndLevelLayer && endLevelLayer && endLevelLayer->m_playLayer) this->setVisible(false);
		CurrencyRewardLayer::update(p0);
	}
};