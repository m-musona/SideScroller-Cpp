#include "Game.h"
#include "Actor.h"
#include "Components/SpriteComponent.h"
#include "Components/BGSpriteComponent.h"
#include "Actors/Ship.h"
#include "Actors/Character.h"
#include "Actors/Asteroid.h"

#include <algorithm>

#include "SDL/SDL_image.h"
#include "GameProgCpp/Random.h"


#include "WindowSize.h"

Game::Game()
	: mWindow(nullptr), 
	mRenderer(nullptr), 
	mIsRunning(true), 
	mUpdatingActors(false),
	mTicksCount(0)
{
}

bool Game::Initialize()
{
	// Iniotialize SDL library
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	// If sdl initialization fails return false
	if (sdlResult != 0)
	{
		// SDL Log is a way to output things to the console in SDL
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	// Create window
	mWindow = SDL_CreateWindow(
		"Space Game", // Window Title
		0, // Top left x-coordinate of window
		0, // Top left y-coordinate of window
		static_cast<int>(screenWidth), // Width of window
		static_cast<int>(screenHeight), // Height of window
		0 // Flags (0 for no flags set)
	);
	// Verify that window created succsefully
	if (!mWindow)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}

	// Create renderer after creating window
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create the renderer for
		-1, // Which Graphics driver to use (-1 is let SDL decide)
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // Flags (1st one is take advantage of graphics hardware 2nd one is vsync on)
	);
	// Verify That renderer is created
	if (!mWindow)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	// Verify SDL Image is working
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize sdl image: %s", SDL_GetError());
		return false;
	}

	Random::Init();
	
	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	// Because ~Actor calls RemoveActor, use a different style loop
	UnloadData();
	IMG_Quit();
	
	// Shutdown SDL Functions
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;

	// While there still events in the queue
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// Handle different event types here

		case SDL_QUIT:
			mIsRunning = false;

			break;
		}
	}

	// Get State of Keyboard
	const Uint8* keyState = SDL_GetKeyboardState(NULL);

	// If escape is pressed, also end loop
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;

	// Process ship input
	// mShip->ProcessKeyboard(state);

	// Process character input
	// mCharacter->ProcessKeyboard(state);
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	// Delta time is the difference in ticks from the last frame (converted to seconds)
	float deltatime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// Clamp maximum delta time value
	if (deltatime > 0.05f)
	{
		deltatime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update all Actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltatime);
	}
	mUpdatingActors = false;

	// move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;

	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	// Step 1: Clear the back buffer to a color
	SDL_SetRenderDrawColor(mRenderer, 0 /* R */, 0 /* G */, 0 /* B */, 255 /* A */);
	// Clear back buffer to current draw color
	SDL_RenderClear(mRenderer);

	// Step 2: Draw the entire game scene
	// Loop over the vector of sprite components and call Draw on each
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}
	
	//Step 3: Swap the front buffer and back buffer
	SDL_RenderPresent(mRenderer);


}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, screenHeight/2));
	mShip->SetRotation(Math::PiOver2);
	mShip->SetScale(1.5f);

	// Create Asteroids
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}

	// Create Player Character
	// mCharacter = new Character(this);
	// mCharacter->SetPosition(Vector2(100.0f, 184.0f));
	// mCharacter->SetScale(1.0f);

	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));

	// Create the "far back" background
	BGSpriteComponent* background = new BGSpriteComponent(temp);
	background->SetScreenSize(Vector2(screenWidth, screenHeight));
	std::vector<SDL_Texture*> backgroundtextures = {
		GetTexture("Assets/FarBack/Farback01.png"),
		GetTexture("Assets/FarBack/Farback02.png")
	};

	background->SetBGTextures(backgroundtextures);
	background->SetScrollSpeed(-100.0f);

	// Create the closer background
	background = new BGSpriteComponent(temp, 50);
	background->SetScreenSize(Vector2(screenWidth, screenHeight));

	backgroundtextures = {
		GetTexture("Assets/Stars/Stars.png"),
		GetTexture("Assets/Stars/Stars.png")
	};
	background->SetBGTextures(backgroundtextures);
	background->SetScrollSpeed(-200.0f);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

void Game::AddActor(Actor* actor)
{
	// if updating actors need to add to pending Actors
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for ( ;
		iter != mSprites.end(); 
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

// Image loading Process
SDL_Texture* Game::GetTexture(const std::string& filename)
{
	SDL_Texture* texture = nullptr;
	// Check if the texture is already in the map?
	auto iter = mTextures.find(filename);
	if (iter != mTextures.end())
	{
		texture = iter->second;
	}
	else
	{
		// Loads an image from file
		// Returns a pointer to an SDL_Surface if successful other wise nullptr
		SDL_Surface* surface = IMG_Load(filename.c_str());

		if (!surface)
		{
			SDL_Log("Failed to load texture file: %s", filename.c_str());
			return nullptr;
		}

		// Convert SDL_Surface to an SDL_Teexture 
		// Returns a pointer to an SDL_Texture if successful other wise nullptr
		texture = SDL_CreateTextureFromSurface(mRenderer, surface);

		SDL_FreeSurface(surface);

		if (!texture)
		{
			SDL_Log("Failed to convert surface to texture for: %s", filename.c_str());
			return nullptr;
		}
		mTextures.emplace(filename.c_str(), texture);
	}
	return texture;
}

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}




