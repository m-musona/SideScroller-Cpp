#pragma once

#include <vector>
#include "GameProgCpp/Math.h"
#include <cstdint>

class Actor
{
public:
	// Used to track state of actor
	enum State
	{
		EActive,
		EPaused,
		EDead
	};

	// Constructor/ Destructor
	Actor(class Game* game);
	virtual ~Actor();

	// Update function called from Game (not overridable)
	void Update(float deltatime);

	// Updates all components attached to the actor (not overridable)
	void UpdateComponents(float deltatime);

	// Any actor-specific update code (overridable)
	virtual void UpdateActor(float deltatime);

	// Process Input function called from Game (not overridable)
	void ProcessInput(const struct InputState& state);

	// Any Actor Specific Input Code (overridable)
	virtual void ActorInput(const struct InputState& state);

	// Getters/Setters
	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
	float GetScale() const { return mScale; }
	void SetScale(float scale) { mScale = scale; mRecomputeWorldTransform = true; }
	float GetRotation() const { return mRotation; }
	void SetRotation(float rotation) { mRotation = rotation; mRecomputeWorldTransform = true; }

	void ComputeWorldTransform();
	const Matrix4& GetWorldTransform() const { return mWorldTransform; }

	Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), Math::Sin(mRotation)); }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }


	// Add/Remove components
	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

private:
	// Actor's State
	State mState;

	// Transform
	Matrix4 mWorldTransform;
	Vector2 mPosition; // Center position of actor
	float mScale; // uniforms Scale of actor (1.0f for 100%)
	float mRotation; // Rotation angle in radians
	bool mRecomputeWorldTransform;

	// Components held by this Actor
	std::vector<class Component*> mComponents;

	class Game* mGame;
};