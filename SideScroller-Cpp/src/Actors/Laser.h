#pragma once
#include "../Actor.h"

class Laser : public Actor
{
public:
	Laser(class Game* game);
	~Laser();

	void UpdateActor(float deltatime) override;
private:
	class CircleCollisionComponent* mCircle;
	class SpriteComponent* mSprite;
	float mDeathTimer;
};