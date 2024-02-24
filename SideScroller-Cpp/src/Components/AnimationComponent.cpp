#include "AnimationComponent.h"
#include "GameProgCpp/Math.h"
#include "../Renderer/Texture.h"

AnimationComponent::AnimationComponent(Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder),
	mCurrFrame(0.0f),
	mAnimFPS(24.0f),
	mloopingAnimation(false),
	mCurrAnimation(0)
{
}

void AnimationComponent::Update(float deltatime)
{
	SpriteComponent::Update(deltatime);

	if (mAnimTextures.size() > 0)
	{
		// Update the current frame based on frame rate and delta time
		mCurrFrame += mAnimFPS * deltatime;

		if (mloopingAnimation)
		{
			// Wrap Current frame if needed
			while (mCurrFrame >= mAnimTextures.size())
			{
				mCurrFrame -= mAnimTextures.size();
			}

			// Set the current texture
			SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
		}
		else 
		{
			if (mCurrFrame <= mAnimTextures.size())
			{
				// Set the current texture
				SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
			}
			
		}
	}
}

void AnimationComponent::SetAnimationTextures(const std::vector<Texture*>& textures)
{
	// Set mAnimTextures variable to the provided textures
	mAnimTextures = textures;

	if (mAnimTextures.size() > 0)
	{
		// Reset Current Frame to zero
		mCurrFrame = 0.0f;

		SetTexture(mAnimTextures[0]);
	}
}

void AnimationComponent::SetAnimations(const std::vector<std::vector<Texture*>>& animations, std::vector<bool> isAnimationLooping)
{
	mAnimations = animations;

	// Set mAnimTextures variable to the provided textures
	mloopingAnimation = isAnimationLooping[mCurrAnimation];
	mAnimTextures = mAnimations[mCurrAnimation];

	if (mAnimTextures.size() > 0)
	{
		// Reset Current Frame to zero
		mCurrFrame = 0.0f;

		SetTexture(mAnimTextures[0]);
	}
}
