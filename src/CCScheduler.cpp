#include <Geode/modify/CCScheduler.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

class $modify(MyCCScheduler, CCScheduler) {
	void update(float dt) {
		Manager::getSharedInstance()->fps = static_cast<int>(getTimeScale() / dt) + 1;
		CCScheduler::update(dt);
	}
};