#include "Laser.h"

#include "../Components/SpriteComponent.h"
#include "../Components/MoveComponent.h"
#include "../Game.h"
#include "../Components/CircleCollisionComponent.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
	: Actor(game),
	mDeathTimer(1.0f),
	mCircle(nullptr),
	mSprite(nullptr)
{
	// Create a sprite component
	mSprite = new SpriteComponent(this);
	mSprite->SetTexture(game->GetTexture("Assets/Laser/Laser.png"));

	// Create a move component, and set a forward speed
	MoveComponent* moveComp = new MoveComponent(this);
	moveComp->SetForwardSpeed(800.0f);

	// Create a circle component (for collision)
	mCircle = new CircleCollisionComponent(this);
	mCircle->SetRadius(11.0f);

}

Laser::~Laser()
{
	GetGame()->RemoveSprite(this->mSprite);
	GetGame()->RemoveActor(this);
}

void Laser::UpdateActor(float deltatime)
{
	// If we run out of time, laser is dead
	mDeathTimer -= deltatime;
	if (mDeathTimer <= 0.0f)
	{
		SetState(EDead);
	}
	else
	{
		// Do we intersect with an asteroid?
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCircle, *(ast->GetCircle())))
			{
				// The first asteroid we intersect with,
				// set ourselves and the asteroid to dead
				SetState(EDead);
				ast->SetState(EDead);
				break;
			}
		}
	}
}
