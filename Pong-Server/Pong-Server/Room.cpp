#include "Room.h"
#include <iostream>
#include "Foreach.h"
#include "Enumerators.h"

namespace Pong{
	Room::Room() :
		nTickInterval(sf::seconds(1 / 0.5f))
		, nTickTime(sf::Time::Zero)
		, sTickInterval(sf::seconds(1 / 50.f))
		, sTickTime(sf::Time::Zero)
		, connectedPlayers(0)
		, maxConnectedPlayers(2)
		, peers(2)
		, game(2)
		, renderer(sf::Vector2u(320, 240), "Pong - Server", &game)
		, roomThread(&Room::ExecutionThread, this)
	{
		for (int i = 0; i < peers.size(); i++)
		{
			peers[i].reset(new RemotePeer(i));
		}
		game.room = this;
		std::cout << "Created new room \n";
		//renderer.Draw();
	}

	Room::~Room() {
		waitingThreadEnd = true;
	}

	bool Room::HasEmptySlot() {
		std::cout << "Players in current room: " << connectedPlayers << " Max players allowed: " << maxConnectedPlayers << "\n";

		return connectedPlayers < maxConnectedPlayers;
	}

	sf::TcpSocket* Room::FreePlayerSocket() {
		return &peers[connectedPlayers]->socket;
	}

	void Room::OnPlayerConnected() {
		std::cout << "Player joined to room\n";

		//Send player the player number
		sf::Packet packet;
		short playerNumber = connectedPlayers;
		packet << static_cast<sf::Int32>(ServerMessage::PlayerNumber) << playerNumber;
		peers[connectedPlayers]->socket.send(packet);

		peers[connectedPlayers++]->ready = true;
		//SendSnapshot();
	}

	void Room::KickThread()
	{
		roomThread.launch();
	}

	void Room::ExecutionThread() {
		roomClock.restart();
		frameTimeClock.restart();
		SendSnapshot();

		while (!waitingThreadEnd) {
			HandleIncomingPackets();

			nTickTime += frameTimeClock.getElapsedTime();
			sTickTime += frameTimeClock.getElapsedTime();
			frameTimeClock.restart();


			//Update simulation
			if (sTickTime >= sTickInterval)
			{
				while (sTickTime >= sTickInterval) {
					game.UpdateSimulation(sTickInterval.asSeconds());
					sTickTime -= sTickInterval;
				}
			}

			if (nTickTime >= nTickInterval)
			{
				while (nTickTime >= nTickInterval) {
					nTickTime -= nTickInterval;
				}
				//Send game state
				SendSnapshot();
				printf("Sent snapshot \n");
			}

			//renderer.Draw();
			sf::sleep(sf::milliseconds(10));
		}
	}

	void Room::HandleIncomingPackets() {
		bool detectedTimeout = false;

		FOREACH(PeerPtr& peer, peers) {
			if (peer->ready) {
				sf::Packet packet;

				while (peer->socket.receive(packet) == sf::Socket::Done) {
					// Handle packet and react to it
					HandleIncomingPacket(packet, *peer, detectedTimeout);
					// Packet was indeed received, update the ping timer
					peer->lastPacketTime = Now();
					packet.clear();
				}

				if (Now() >= peer->lastPacketTime + clientTimeoutTime) {
					peer->timedOut = true;
					detectedTimeout = true;
				}
			}
		}

		//TODO: Remove this
		detectedTimeout = false;
		if (detectedTimeout) {
			HandleDisconnections();
		}
	}

	void Room::HandleIncomingPacket(sf::Packet packet, RemotePeer& packetSender, bool& detectedTimeout) {
		sf::Int32 packetType;
		packet >> packetType;

		switch (packetType) {
		case ClientMessage::Quit:
			packetSender.timedOut = true;
			detectedTimeout = true;
			break;

		case ClientMessage::Move:
			//Change simulation values
			sf::Int8 movementInput;
			packet >> movementInput;
			//std::cout
			//	<< "Received movement input of player " << packetSender.playerNumber
			//	<< " with value " << movementInput << "\n";
			game.UpdateInput(packetSender.playerNumber, movementInput);
			//Send other players 
			SendPlayerMovement(movementInput, packetSender.playerNumber);
		default:
			break;
		}
	}

	void Room::HandleDisconnections() {
		for (auto itr = peers.begin(); itr != peers.end();) {
			if ((*itr)->timedOut) {
				// Inform everyone of the disconnection, erase 
				SendToAll(sf::Packet() << static_cast<sf::Int32>(ServerMessage::PlayerDisconnected) << (*itr)->playerNumber);
				connectedPlayers--;

				itr = peers.erase(itr);

				//TODO: Fix this, Go back to a listening state if needed
				//if(connectedPlayers < maxConnectedPlayers) {
				//	peers.push_back(PeerPtr(new RemotePeer()));
				//}
			}
			else {
				++itr;
			}
		}
	}

	sf::Time Room::Now() const {
		return roomClock.getElapsedTime();
	}

	void Room::SendToAll(sf::Packet packet) {
		FOREACH(PeerPtr& peer, peers) {
			if (peer->ready){
				peer->socket.send(packet);
			}
		}
	}

	void Room::SendSnapshot() {
		SendToAll(game.GetSnapshot());
	}

	void Room::SendPlayerMovement(int playerInput, int playerNumber){
		sf::Packet packet;
		packet << static_cast<sf::Int32>(ServerMessage::PlayerMovement) 
			<< playerNumber << playerInput;
		FOREACH(PeerPtr& peer, peers) {
			if (peer->playerNumber != playerNumber && peer->ready){
				peer->socket.send(packet);
			}
		}
	}

	void Room::DeclareWinner(int winnerPlayerNumber){
		sf::Packet packet;
		packet << static_cast<sf::Int32>(ServerMessage::GameEnd)
			<< winnerPlayerNumber;
		SendToAll(packet);
	}
}