#pragma once
#include "SpriteComponent.h"
#include <vector>

class AnimationComponent : public SpriteComponent
{
public:
	AnimationComponent(class Actor* owner, int drawOrder = 100);

	// Update animation every frame (overriden from component)
	void Update(float deltatime) override;

	// Set the textures used for animation
	void SetAnimationTextures(const std::vector<SDL_Texture*>& textures);

	// Set the textures used for animation
	void SetAnimations(const std::vector<std::vector<SDL_Texture*>>& animations, std::vector<bool> isAnimationLooping);

	// Set/Get the animation FPS
	float GetAnimationFPS() const { return mAnimFPS; }
	void SetAnimationFPS(float fps) { mAnimFPS = fps; }

	// Set/Get the Current Animation Index
	int GetCurrentAnimation() const { return mCurrAnimation; }
	void SetCurrentAnimation(int currentAnim) { mCurrAnimation = currentAnim; }

	// Set if Animation is looping
	void SetLoopingAnimation(bool isLooping) { mloopingAnimation = isLooping; }
private:
	// All textures in animation
	std::vector<SDL_Texture*> mAnimTextures;
	// All Animation
	std::vector<std::vector<SDL_Texture*>> mAnimations;

	// Is Animation Looping
	bool mloopingAnimation;

	// Current frame displayed
	int mCurrAnimation;

	// Current frame displayed
	float mCurrFrame;

	// Animation frame rate
	float mAnimFPS;
};