#include "SFML/Network.hpp"
#include "GameSimulation.h"

#pragma once
namespace Pong{
	class ServerListener
	{
	public:
		ServerListener();
		~ServerListener();
		void Start(GameSimulation* game);
		void Stop();
		sf::TcpSocket* GetSocket();

	private:
		void ExecutionThread();
		sf::TcpSocket socket;
		sf::Thread executionThread;
		bool waitingForThreadEnd = false;
	private:
		GameSimulation* gameToUpdate = nullptr;
	};
}
