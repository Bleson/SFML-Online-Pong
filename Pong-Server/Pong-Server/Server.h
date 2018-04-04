#pragma once
#include <list>
#include <SFML/Network.hpp>
#include "Room.h"
#include <map>
#include <vector>

namespace Pong{
	class Server {

	public:
		Server();
		~Server();
		void GetNewRoom();
		void Run();
		void HandleNewConnections();

	typedef std::unique_ptr<Room> RoomPtr;

	private:
		const unsigned short masterPort = 53000;
		unsigned short latestPortUsed = masterPort;
		sf::TcpListener masterListener;
		std::vector<RoomPtr> rooms;
		int currentRoom;
	};
}
