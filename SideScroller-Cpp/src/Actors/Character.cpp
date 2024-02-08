#include "Character.h"

#include "../Game.h"
#include "../Components/AnimationComponent.h"

Character::Character(Game* game)
	:Actor(game),
	mRightSpeed(0.0f),
	mDownSpeed(0.0f)
{
	AnimationComponent* animSpriteComp = new AnimationComponent(this);
	std::vector<SDL_Texture*> animation1 = {
		game->GetTexture("Assets/Character/Walking/Character01.png"),
		game->GetTexture("Assets/Character/Walking/Character02.png"),
		game->GetTexture("Assets/Character/Walking/Character03.png"),
		game->GetTexture("Assets/Character/Walking/Character04.png"),
		game->GetTexture("Assets/Character/Walking/Character05.png"),
		game->GetTexture("Assets/Character/Walking/Character06.png"),
	};
	std::vector<SDL_Texture*> animation2 = {
		game->GetTexture("Assets/Character/Jumping/Character07.png"),
		game->GetTexture("Assets/Character/Jumping/Character08.png"),
		game->GetTexture("Assets/Character/Jumping/Character09.png"),
		game->GetTexture("Assets/Character/Jumping/Character10.png"),
		game->GetTexture("Assets/Character/Jumping/Character11.png"),
		game->GetTexture("Assets/Character/Jumping/Character12.png"),
		game->GetTexture("Assets/Character/Jumping/Character13.png"),
		game->GetTexture("Assets/Character/Jumping/Character14.png"),
		game->GetTexture("Assets/Character/Jumping/Character15.png"),
	};
	std::vector<SDL_Texture*> animation3 = {
		game->GetTexture("Assets/Character/Punching/Character16.png"),
		game->GetTexture("Assets/Character/Punching/Character17.png"),
		game->GetTexture("Assets/Character/Punching/Character18.png"),
	};
	animSpriteComp->SetAnimationFPS(7.0f);
	//animSpriteComp->SetAnimationTextures(animation3);

	std::vector<bool> isAnimLooping = { true, false, false };
	std::vector<std::vector<SDL_Texture*>> animations = { animation1 , animation2, animation3 };
	animSpriteComp->SetCurrentAnimation(1);
	animSpriteComp->SetAnimations(animations, isAnimLooping);
}

void Character::UpdateActor(float deltatime)
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

void Character::ProcessKeyboard(const uint8_t* state)
{
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;
	// right/left
	if (state[SDL_SCANCODE_RIGHT])
	{
		mRightSpeed += 250.0f;
	}
	if (state[SDL_SCANCODE_LEFT])
	{
		mRightSpeed -= 250.0f;
	}
	// up/down
	if (state[SDL_SCANCODE_DOWN])
	{
		mDownSpeed += 300.0f;
	}
	if (state[SDL_SCANCODE_UP])
	{
		mDownSpeed -= 300.0f;
	}
}
