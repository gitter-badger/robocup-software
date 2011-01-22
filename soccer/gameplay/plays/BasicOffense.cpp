#include <boost/foreach.hpp>
#include "BasicOffense.hpp"

using namespace std;
using namespace Geometry2d;

REGISTER_PLAY_CATEGORY(Gameplay::Plays::BasicOffense, "Playing")

Gameplay::Plays::BasicOffense::BasicOffense(GameplayModule *gameplay):
	Play(gameplay),
	_leftFullback(gameplay, Behaviors::Fullback::Left),
	_rightFullback(gameplay, Behaviors::Fullback::Right),
	_kicker(gameplay),
	_support(gameplay)
{
	_leftFullback.otherFullbacks.insert(&_rightFullback);
	_rightFullback.otherFullbacks.insert(&_leftFullback);
}

float Gameplay::Plays::BasicOffense::score ( Gameplay::GameplayModule* gameplay )
{
	// only run if we are playing and not in a restart
	bool refApplicable = gameplay->state()->gameState.playing();
	return refApplicable ? 0 : INFINITY;
}

bool Gameplay::Plays::BasicOffense::run()
{
	// handle assignments
	set<OurRobot *> available = _gameplay->playRobots();

	// defense first - closest to goal
	assignNearest(_leftFullback.robot, available, Geometry2d::Point());
	assignNearest(_rightFullback.robot, available, Geometry2d::Point());

	// choose offense, we want closest robot to ball to be striker
	// TODO: more logic for switching with hysteresis
	assignNearest(_kicker.robot, available, ball().pos);
	assignNearest(_support.robot, available, ball().pos);

	// manually reset any kickers so they keep kicking
	if (_kicker.done())
		_kicker.restart();

	// pick as a mark target the furthest back opposing robot
	OpponentRobot* bestOpp;
	float bestDist = Field_Length;
	BOOST_FOREACH(OpponentRobot* opp, state()->opp)
	{
		if (opp->pos.y < bestDist) {
			bestDist = opp->pos.y;
			bestOpp = opp;
		}
	}
	_support.markRobot(bestOpp);

	// execute behaviors
	_kicker.run();
	_support.run();
	_leftFullback.run();
	_rightFullback.run();
	
	return true;
}
