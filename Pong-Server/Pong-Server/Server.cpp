#include "Server.h"
#include <string>
#include <iostream>

namespace Pong{
	Server::Server() :
		rooms(5)
	{
		for (int i = 0; i < 5; i++)
		{
			for (int i = 0; i < rooms.size(); i++)
			{
				rooms[i].reset(new Room());
			}
		}
		currentRoom = 0;
	}

	Server::~Server()
	{
	}

	void Server::GetNewRoom(){
		currentRoom++;
	}

	void Server::Run(){
		//Open master port
		if (masterListener.listen(masterPort) == sf::Socket::Done)
		{
			printf("Master listener started. \n");
		}

		while (true)
		{
			if (!rooms[currentRoom]->HasEmptySlot())
			{
				GetNewRoom();
			}
			HandleNewConnections();
			// When room has no more empty slots, start game
			rooms[currentRoom]->KickThread();
			printf("Starting room execution...\n");

		}
	}

	void Server::HandleNewConnections() {
		while (rooms[currentRoom]->HasEmptySlot()) {
			//Add player to room
			sf::TcpSocket* client = rooms[currentRoom]->FreePlayerSocket();
			if (client == nullptr)
			{
				printf("null pointer!");
			}
			else
			{
				std::cout << "Local port: " << client->getLocalPort()
					<< " Client port: " << client->getRemotePort()
					<< "\n";
			}
			if (masterListener.accept(*client) == sf::Socket::Done) {
				//Currently this thread will just run the game
				rooms[currentRoom]->OnPlayerConnected();
				std::cout << "PLAYER CONNECTED - Local port: " << client->getLocalPort()
					<< " Client port: " << client->getRemotePort()
					<< "\n";
			}
		}
	}
}