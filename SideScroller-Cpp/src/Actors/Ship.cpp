#include "Ship.h"

#include "../Game.h"
#include "../Components/AnimationComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CircleCollisionComponent.h"
#include "../Renderer/Texture.h"

#include "../Systems/InputSystem.h"

#include "Laser.h"
#include "Asteroid.h"

#include "../WindowSize.h"

#include <iostream>


Ship::Ship(Game* game)
	:Actor(game),
	mLaserCooldown(0.0f), 
	mSpeed(400.0f),
	mCircle(nullptr)
{
	AnimationComponent* animSpriteComp = new AnimationComponent(this);
	animSpriteComp->SetLoopingAnimation(true);
	std::vector<Texture*> animation1 = {
		game->GetTexture("Assets/Ship/Ship01.png"),
		game->GetTexture("Assets/Ship/Ship02.png"),
		game->GetTexture("Assets/Ship/Ship03.png"),
		game->GetTexture("Assets/Ship/Ship04.png"),
	};

	std::vector<bool> isAnimLooping = { true };
	std::vector<std::vector<Texture*>> animations = { animation1 };
	animSpriteComp->SetCurrentAnimation(0);
	animSpriteComp->SetAnimations(animations, isAnimLooping);

	// Create an input component and set keys/speed
	InputComponent* inputComp = new InputComponent(this);
	inputComp->SetForwardKey(SDL_SCANCODE_W);
	inputComp->SetBackKey(SDL_SCANCODE_S);
	inputComp->SetClockwiseKey(SDL_SCANCODE_A);
	inputComp->SetCounterClockwiseKey(SDL_SCANCODE_D);
	inputComp->SetMaxForwardSpeed(300.0f);
	inputComp->SetMaxAngularSpeed(Math::TwoPi);

	// Create a circle component (for collision)
	mCircle = new CircleCollisionComponent(this);
	mCircle->SetRadius(30.0f);
}

void Ship::UpdateActor(float deltatime)
{
	mLaserCooldown -= deltatime;

	// Update position based on velocity
	Vector2 pos = GetPosition();
	pos += mVelocityDir * mSpeed * deltatime;
	SetPosition(pos);

	// Update rotation
	float angle = Math::Atan2(mRotationDir.y, mRotationDir.x);
	SetRotation(angle);

	// Do we intersect with an asteroid?
	for (auto asteroid : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(asteroid->GetCircle())))
		{
			this->SetPosition(Vector2(0, 0));
			this->SetRotation(0);
		}
	}
}

void Ship::ActorInput(const InputState& state)
{
	//if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	//{
	//	// Create a laser and set its position/rotation to mine
	//	Laser* laser = new Laser(GetGame());
	//	laser->SetPosition(GetPosition());
	//	laser->SetRotation(GetRotation());

	//	// Reset laser cooldown (half second)
	//	mLaserCooldown = 0.5f;
	//}

	if (state.Controller.GetRightTrigger() > 0.25f
		&& mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (quarter second)
		mLaserCooldown = 0.25f;
	}

	if (state.Controller.GetIsConnected())
	{
		mVelocityDir = state.Controller.GetLeftStick();
		if (!Math::NearZero(state.Controller.GetRightStick().Length()))
		{
			mRotationDir = state.Controller.GetRightStick();
		}
	}
}
