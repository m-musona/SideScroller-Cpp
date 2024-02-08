#pragma once

class Component
{
public:
	// Constructor
	// (the lower the update order, the earlier the component updates)
	Component(class Actor* owner, int updateOrder = 100);

	// Destructor
	~Component();

	// Update this component by delta time
	virtual void Update(float deltatime);

	int GetUpdateOrder() const { return mUpdateOrder; }

protected:
	// Owning actor
	class Actor* mOwner;

	// Update order of Component
	int mUpdateOrder;
};