#pragma once

#include "SDL/SDL.h"

#include <vector>
#include <unordered_map>
#include <string>
#include "GameProgCpp/Math.h"

class Game
{
public:
	Game();

	// Initialize the Game
	bool Initialize();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	class Texture* GetTexture(const std::string& filename);

	// Game-specific (add/remove asteroid)
	void AddAsteroid(class Asteroid* ast);
	void RemoveAsteroid(class Asteroid* ast);
	std::vector<class Asteroid*>& GetAsteroids() { return mAsteroids; }

private:

	// Helper Functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	bool LoadShaders();
	void CreateSpriteVerts();
	void LoadData();
	void UnloadData();

	// Map of textures loaded
	std::unordered_map<std::string, class Texture*> mTextures;

	// All actors in the game
	std::vector<class Actor*> mActors;
	// Pending actors
	std::vector<class Actor*> mPendingActors;

	// All the sprites drawn
	std::vector<class SpriteComponent*> mSprites;

	// Sprite shader
	class Shader* mSpriteShader;
	// Sprite vertex array
	class VertexArray* mSpriteVerts;

	// Window Created by SDL
	SDL_Window* mWindow;
	// OpenGL Context
	SDL_GLContext mContext;

	Uint32 mTicksCount;

	class InputSystem* mInputSystem;

	// Are we updating actors
	bool mUpdatingActors;
	// Game should continue to run
	bool mIsRunning;

	// Game-Specific
	class Ship* mShip; // Player Ship
	// class Character* mCharacter; // Player Character
	std::vector<class Asteroid*> mAsteroids;
};