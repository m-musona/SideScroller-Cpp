#include "BGSpriteComponent.h"
#include "../Actor.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"

BGSpriteComponent::BGSpriteComponent(class Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder),
	mScrollSpeed(0.0f)
{
}

void BGSpriteComponent::Update(float deltatime)
{
	SpriteComponent::Update(deltatime);
	for (auto& bg : mBGTextures)
	{
		// Update the x offset
		bg.mOffset.x += mScrollSpeed * deltatime;

		// If This is completely off the screen, reset to the right of the last bg texture
		if (bg.mOffset.x < -mScreenSize.x)
		{
			bg.mOffset.x = (mBGTextures.size() - 1) * mScreenSize.x - 1;
		}
	}
}

void BGSpriteComponent::Draw(Shader* shader)
{
	// Draw each background texture
	for (auto background : mBGTextures)
	{
		//SDL_Rect r;

		//// Assume screen dimensions
		//r.w = static_cast<int>(mScreenSize.x);
		//r.h = static_cast<int>(mScreenSize.y);

		//// Center the rectangle around the position of the owner
		//r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2 + background.mOffset.x);
		//r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2 + background.mOffset.y);

		//// Draw 
		//SDL_RenderCopy(renderer, background.mTexture, nullptr, &r);

		if(background.mTexture)
		{
			// Scale the quad by the width/height of texture
			Matrix4 scaleMat = Matrix4::CreateScale(
				static_cast<float>(mScreenSize.x),
				static_cast<float>(mScreenSize.y),
				1.0f);

			Matrix4 world = scaleMat * mOwner->GetWorldTransform();

			// Since all sprites use the same shader/vertices,
			// the game first sets them active before any sprite draws

			// Set world transform
			shader->SetMatrixUniform("uWorldTransform", world);

			// Set current texture
			background.mTexture->SetActive();

			// Draw quad
			glDrawElements(
				GL_TRIANGLES,		// Type of polygon/primitive to draw
				6,					// Number of indices in index buffer
				GL_UNSIGNED_INT,	// Type of each index
				nullptr				// Usually nullptr
			);
		}
		
	}
}

void BGSpriteComponent::SetBGTextures(const std::vector<Texture*>& textures)
{
	int count = 0;
	for (auto texture : textures)
	{
		BGTexture temp;
		temp.mTexture = texture;

		// Each texture is screen width offset
		temp.mOffset.x = count * mScreenSize.x;
		temp.mOffset.y = 0;

		mBGTextures.emplace_back(temp);

		count++;
	}
}
