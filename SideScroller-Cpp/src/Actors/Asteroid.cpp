#include "Asteroid.h"

#include "../Components/SpriteComponent.h"
#include "../Components/MoveComponent.h"
#include "../Game.h"
#include "../Components/CircleCollisionComponent.h"

#include "GameProgCpp/Random.h"
#include "../WindowSize.h"

Asteroid::Asteroid(Game* game)
	:Actor(game),
	mCircle(nullptr)
{
	// Initialize to random position/orientation
	Vector2 randPos = Random::GetVector(Vector2::Zero, Vector2(screenWidth, screenHeight));
	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// Create a sprite component and texture
	SpriteComponent* spriteComp = new SpriteComponent(this);
	spriteComp->SetTexture(game->GetTexture("Assets/Asteroid/Asteroid.png"));

	// Create Move Component and set a forward vector
	MoveComponent* moveComp = new MoveComponent(this);
	moveComp->SetForwardSpeed(150.0f);

	// Create a circle component (for collision)
	mCircle = new CircleCollisionComponent(this);
	mCircle->SetRadius(40.0f);

	// Add to mAsteroids in game
	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}
