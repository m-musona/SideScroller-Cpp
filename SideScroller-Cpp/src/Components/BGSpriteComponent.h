#pragma once
#include "SpriteComponent.h"
#include <vector>
#include "GameProgCpp/Math.h"

class BGSpriteComponent : public SpriteComponent
{
public:
	// Set draw order to default to lower (so it's in the background)
	BGSpriteComponent(class Actor* owner, int drawOrder = 10);

	// Update/draw overriden from parent
	void Update(float deltatime) override;
	void Draw(class Shader* renderer) override;

	// Set the textures used for the background
	void SetBGTextures(const std::vector<class Texture*>& textures);

	// Get/Set screen size and scroll speed
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }
	float GetScrollSpeed() const { return mScrollSpeed; }

private:
	// Struct to encapsulate each BG image and its offset
	struct BGTexture
	{
		class Texture* mTexture;
		Vector2 mOffset;
	};

	std::vector<BGTexture> mBGTextures;
	Vector2 mScreenSize;
	float mScrollSpeed;
};
