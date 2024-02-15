#include "MoveComponent.h"
#include "../Actor.h"
#include "../WindowSize.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder),
	mAngularSpeed(0.0f),
	mForwardSpeed(0.0f)
{
}

void MoveComponent::Update(float deltatime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		float rotation = mOwner->GetRotation();
		rotation += mAngularSpeed * deltatime;
		mOwner->SetRotation(rotation);
	}

	// Convert from angle to a forward vector
	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 position = mOwner->GetPosition();
		position += mOwner->GetForward() * mForwardSpeed * deltatime;

		// (Screen wrapping code only for asteroids)
		if (position.x < 0.0f) { position.x = screenWidth - 2; }
		else if (position.x > screenWidth) { position.x = 2.0f; }

		if (position.y < 0.0f) { position.y = screenHeight - 2; }
		else if (position.y > screenHeight) { position.y = 2.0f; }

		mOwner->SetPosition(position);
	}
}
