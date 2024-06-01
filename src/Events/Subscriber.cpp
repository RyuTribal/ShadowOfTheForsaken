#include "pch.h"
#include "Subscriber.h"
#include "Core/Game.h"

namespace SOF {
	Subscriber::~Subscriber()
	{
		if (m_ID != 0) {
			Game::Get()->RevokeSubscription(m_ID);
		}
	}

	void Subscriber::Subscribe(std::function<void(Event&)> callback)
	{
		m_ID = Game::Get()->SubscribeOnEvents(callback);
	}
}
