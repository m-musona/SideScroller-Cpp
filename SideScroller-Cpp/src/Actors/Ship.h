#pragma once
#include "../Actor.h"

class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltatime) override;
	void ActorInput(const struct InputState& state) override;

private:
	class CircleCollisionComponent* mCircle;

	Vector2 mVelocityDir;
	Vector2 mRotationDir;
	float mSpeed;
	float mLaserCooldown;
};