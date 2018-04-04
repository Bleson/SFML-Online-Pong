#include "ServerListener.h"
#include <iostream>
#include "Enumerators.h"
#include <vector>
#include "GameSimulation.h"

namespace Pong{
	ServerListener::ServerListener() :
		executionThread(&ServerListener::ExecutionThread, this)
	{
	}


	ServerListener::~ServerListener()
	{
	}

	void ServerListener::Start(GameSimulation* game)
	{
		if (game == nullptr)
		{
			printf("ListenerThread received nullptr! \n");
			return;
		}
		gameToUpdate = game;
		executionThread.launch();
	}

	void ServerListener::Stop(){

	}

	void ServerListener::ExecutionThread(){
		sf::Packet packet;

		while (!waitingForThreadEnd)
		{
			while (socket.receive(packet) == sf::Socket::Done) {
				// Handle packet and react to it
				sf::Int32 packetType;
				packet >> packetType;
				std::cout << "Handling package type " << packetType << " \n";

				sf::Vector2f ballPosition;
				sf::Vector2f ballVelocity;
				float playerPos1;
				float playerPos2;
				std::vector<GameSimulation::RectangleObject> players = *gameToUpdate->GetPlayers();
				std::vector<GameSimulation::RectangleObject>* playersLocation = gameToUpdate->GetPlayers();
				std::vector<unsigned int> scores;

				switch (packetType)
				{
				case ServerMessage::Snapshot:
					std::cout << "Handling snapshot package... \n";

					packet >> ballPosition.x;
					packet >> ballPosition.y;
					packet >> ballVelocity.x;
					packet >> ballVelocity.y;
					packet >> playerPos1;
					packet >> playerPos2;
					//std::cout <<
					//	"Ball position: " << ballPosition.x <<
					//	", " << ballPosition.y <<
					//	" Player 0 position: " << playerPos1 <<
					//	" Player 1 position: " << playerPos2 <<
					//	"\n";
					//Update game here
					gameToUpdate->GetBall()->box.x = ballPosition.x;
					gameToUpdate->GetBall()->box.y = ballPosition.y;
					gameToUpdate->GetBall()->box.vx = ballVelocity.x;
					gameToUpdate->GetBall()->box.vy = ballVelocity.y;
					
					players[0].box.y = playerPos1;
					players[1].box.y = playerPos2;
					*playersLocation = players;
					break;
				case ServerMessage::RoundEnd:
					packet >> gameToUpdate->GetBall()->box.x;
					packet >> gameToUpdate->GetBall()->box.y;
					packet >> gameToUpdate->GetBall()->box.vx;
					packet >> gameToUpdate->GetBall()->box.vy;

					unsigned int i;
					packet >> i;
					scores.push_back(i);
					packet >> i;
					scores.push_back(i);

					gameToUpdate->GetScores() = scores;
					break;
				case ServerMessage::PlayerNumber:
					short s;
					packet >> s;
					gameToUpdate->SetPlayerNumber(s);
					break;
				case ServerMessage::PlayerMovement:
					int playerNumber;
					int playerInput;
					packet >> playerNumber;
					packet >> playerInput;
					gameToUpdate->UpdateInput(playerNumber, playerInput);
					break;
				case ServerMessage::GameEnd:
					int winner;
					packet >> winner;
					gameToUpdate->EndGame(winner);
					break;
				default:
					break;
				}

				packet.clear();
			}
		}
	}

	sf::TcpSocket* ServerListener::GetSocket(){
		return &socket;
	}

}
