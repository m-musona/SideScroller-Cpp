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
		if (position.x < -screenWidth / 2) { position.x = screenWidth / 2 - 2; }
		else if (position.x > screenWidth/ 2) { position.x = -screenWidth / 2 + 2; }

		if (position.y < -screenHeight / 2) { position.y = screenHeight / 2 - 2; }
		else if (position.y > screenHeight / 2) { position.y = -screenHeight / 2 + 2; }

		/*if (position.x < -512.0f) { position.x = 510.0f; }
		else if (position.x > 512.0f) { position.x = -510.0f; }
		if (position.y < -384.0f) { position.y = 382.0f; }
		else if (position.y > 384.0f) { position.y = -382.0f; }*/

		mOwner->SetPosition(position);
	}
}
