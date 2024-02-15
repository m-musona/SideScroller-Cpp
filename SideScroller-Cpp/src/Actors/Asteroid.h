#pragma once
#include "../Actor.h"

class Asteroid : public Actor
{
public:
	Asteroid(class Game* game);
	~Asteroid();

	class CircleCollisionComponent* GetCircle() { return mCircle; }
private:
	class CircleCollisionComponent* mCircle;
};