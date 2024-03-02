#include "Game.h"
#include "Actor.h"
#include "Components/SpriteComponent.h"
#include "Components/BGSpriteComponent.h"
#include "Actors/Ship.h"
// #include "Actors/Character.h"
#include "Actors/Asteroid.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Texture.h"

#include "Systems/InputSystem.h"

#include <algorithm>
#include <iostream>

#include "SDL/SDL_image.h"
#include "GameProgCpp/Random.h"
#include "glad/glad.h"


#include "WindowSize.h"

Game::Game()
	: mWindow(nullptr), 
	mIsRunning(true), 
	mUpdatingActors(false),
	mSpriteShader(nullptr),
	mTicksCount(0)
{
}

bool Game::Initialize()
{
	// Iniotialize SDL library
	int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

	// If sdl initialization fails return false
	if (sdlResult != 0)
	{
		// SDL Log is a way to output things to the console in SDL
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Use Core Opengl profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Specify version 4.6
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Force OpenGL to use hardware acceleration(Run on GPU)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create window
	mWindow = SDL_CreateWindow(
		"Space Game", // Window Title
		0, // Top left x-coordinate of window
		0, // Top left y-coordinate of window
		static_cast<int>(screenWidth), // Width of window
		static_cast<int>(screenHeight), // Height of window
		SDL_WINDOW_OPENGL // Window for opengl usage
	);
	// Verify that window created succsefully
	if (!mWindow)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}

	// Initialize input system
	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("Failed to initialize input system");
		return false;
	}

	// Create Opengle context
	mContext = SDL_GL_CreateContext(mWindow);
	// Check if context failed to be made
	if (mContext == NULL)
	{
		SDL_Log("Failed to Create Context: %s", SDL_GetError());
		return false;
	}

	// Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		SDL_Log("Failed to initialize Glad: %s", SDL_GetError());
		return false;
	}

	std::cout << std::left << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
	std::cout << std::left << "OpenGL Shading Language Version: " << (char *)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << std::left << "OpenGL Vendor: " << (char *)glGetString(GL_VENDOR) << std::endl;
	std::cout << std::left << "OpenGL Renderer: " << (char *)glGetString(GL_RENDERER) << std::endl;

	// Set OpenGL View Port
	// int ww, wh;
	// SDL_GetWindowSize(mWindow, &ww, &wh);
	// glViewport(0,0, ww, wh);

	// Make Sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders");
		return false;
	}

	// Create quad for drawing Sprites
	CreateSpriteVerts();

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
	
	// Shutdown SDL Functions
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	mInputSystem->PrepareForUpdate();

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

			case SDL_MOUSEWHEEL:
				mInputSystem->ProcessEvent(event);
				break;

			default:
				break;
		}
	}

	mInputSystem->Update();

	const InputState& state = mInputSystem->GetState();

	// If escape is pressed, also end loop
	if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE)
		== EReleased)
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
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
		pending->ComputeWorldTransform();

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
	
	// Set clear color to black
	glClearColor(0.86f /* R */, 0.86f /* G */, 0.86f /* B */, 1.0f /* A */);
	// Clear back buffer to current draw color
	glClear(GL_COLOR_BUFFER_BIT);

	// Step 2: Draw the entire game scene
	
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,			// srcFactor is srcAlpha
		GL_ONE_MINUS_SRC_ALPHA	// dstFactor is 1 - srcAlpha
	);
	// Set sprite shader and vertex array objects active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();

	// Loop over the vector of sprite components and call Draw on each
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}
	
	//Step 3: Swap the front buffer and back buffer
	SDL_GL_SwapWindow(mWindow);


}

bool Game::LoadShaders()
{
	mSpriteShader = new Shader();

	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();

	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(screenWidth, screenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
	return true;
}

void Game::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f,  0.5f, 0.f, 0.f, 0.f, // top left
		 0.5f,  0.5f, 0.f, 1.f, 0.f, // top right
		 0.5f, -0.5f, 0.f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	// mShip->SetPosition(Vector2(100.0f, screenHeight/2));
	mShip->SetRotation(Math::PiOver2);
	// mShip->SetScale(1.5f);

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
	background->SetScreenSize(Vector2(screenWidth * 2, screenHeight * 2));
	std::vector<Texture*> backgroundtextures = {
		GetTexture("Assets/FarBack/Farback01.png"),
		GetTexture("Assets/FarBack/Farback02.png")
	};

	background->SetBGTextures(backgroundtextures);
	background->SetScrollSpeed(-100.0f);

	// Create the closer background
	background = new BGSpriteComponent(temp, 50);
	background->SetScreenSize(Vector2(screenWidth * 2 , screenHeight * 2));

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
		i.second->Unload();
		delete i.second;
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
Texture* Game::GetTexture(const std::string& filename)
{
	Texture* texture = nullptr;
	// Check if the texture is already in the map?
	auto iter = mTextures.find(filename);
	if (iter != mTextures.end())
	{
		texture = iter->second;
	}
	else
	{
		texture = new Texture();

		// Loads an image from file
		if (texture->Load(filename))
		{
			mTextures.emplace(filename, texture);
		}
		else
		{
			delete texture;
			texture = nullptr;
		}
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