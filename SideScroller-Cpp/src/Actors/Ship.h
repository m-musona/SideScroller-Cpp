#pragma once
#include "../Actor.h"

class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltatime) override;
	void ActorInput(const uint8_t* keyState) override;

private:
	float mLaserCooldown;
};