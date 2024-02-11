#include "Ship.h"

#include "../Game.h"
#include "../Components/AnimationComponent.h"
#include "../Components/InputComponent.h"
#include "Laser.h"


Ship::Ship(Game* game)
	:Actor(game),
	mLaserCooldown(0.0f)
{
	AnimationComponent* animSpriteComp = new AnimationComponent(this);
	animSpriteComp->SetLoopingAnimation(true);
	std::vector<SDL_Texture*> animation1 = {
		game->GetTexture("Assets/Ship/Ship01.png"),
		game->GetTexture("Assets/Ship/Ship02.png"),
		game->GetTexture("Assets/Ship/Ship03.png"),
		game->GetTexture("Assets/Ship/Ship04.png"),
	};

	std::vector<bool> isAnimLooping = { true };
	std::vector<std::vector<SDL_Texture*>> animations = { animation1 };
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
}

void Ship::UpdateActor(float deltatime)
{
	mLaserCooldown -= deltatime;
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}
