#include "Ship.h"

#include "../Game.h"
#include "../Components/AnimationComponent.h"


Ship::Ship(Game* game)
	:Actor(game),
	mRightSpeed(0.0f),
	mDownSpeed(0.0f)
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
}

void Ship::UpdateActor(float deltatime)
{
	Actor::UpdateActor(deltatime);
	// Update position based on speeds and delta time
	Vector2 pos = GetPosition();
	pos.x += mRightSpeed * deltatime;
	pos.y += mDownSpeed * deltatime;
	// Restrict position to left half of screen
	if (pos.x < 25.0f)
	{
		pos.x = 25.0f;
	}
	else if (pos.x > 500.0f)
	{
		pos.x = 500.0f;
	}
	if (pos.y < 25.0f)
	{
		pos.y = 25.0f;
	}
	else if (pos.y > 743.0f)
	{
		pos.y = 743.0f;
	}
	SetPosition(pos);
}

void Ship::ProcessKeyboard(const uint8_t* state)
{
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;
	// right/left
	if (state[SDL_SCANCODE_D])
	{
		mRightSpeed += 250.0f;
	}
	if (state[SDL_SCANCODE_A])
	{
		mRightSpeed -= 250.0f;
	}
	// up/down
	if (state[SDL_SCANCODE_S])
	{
		mDownSpeed += 300.0f;
	}
	if (state[SDL_SCANCODE_W])
	{
		mDownSpeed -= 300.0f;
	}
}
