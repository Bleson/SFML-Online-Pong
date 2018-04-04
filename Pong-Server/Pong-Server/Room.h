#pragma once
#include <SFML/Network.hpp>
#include <memory>
#include <vector>
#include <list>
#include "GameSimulation.h"
#include <SFML/Graphics.hpp>
#include "GameRenderer.h"
#include <SFML/System/Thread.hpp>

namespace Pong{
	class Room
	{
	private:
		struct RemotePeer {
			RemotePeer(int playerNumber) {
				socket.setBlocking(false);
				ready = false;
				this->playerNumber = playerNumber;
			}

			sf::TcpSocket	socket;
			sf::Time		lastPacketTime;
			int				playerNumber;
			bool			ready;
			bool			timedOut;
		};

		typedef std::unique_ptr<RemotePeer> PeerPtr;

	public:
		Room();
		~Room();
		bool HasEmptySlot();
		sf::TcpSocket* FreePlayerSocket();
		void OnPlayerConnected();
		void KickThread();
		void SendToAll(sf::Packet packet);
		void DeclareWinner(int winnerPlayerNumber);

	private:
		sf::Time Now() const;
		void ExecutionThread();
		void HandleIncomingPacket(sf::Packet packet, RemotePeer& receivingPeer, bool& detectedTimeout);
		void HandleIncomingPackets();
		void HandleDisconnections();
		void SendSnapshot();
		void SendPlayerMovement(int playerInput, int playerNumber);

	private:
		sf::Thread					roomThread;
		std::vector<PeerPtr>		peers;

		GameSimulation				game;

		std::size_t					connectedPlayers;
		std::size_t					maxConnectedPlayers;


		sf::Clock					roomClock;
		sf::Time					clientTimeoutTime;

		sf::Time					nTickInterval;
		sf::Time					sTickInterval;

		//Network update time
		sf::Time					nTickTime;
		sf::Clock					frameTimeClock;

		//Simulation time
		sf::Time					sTickTime;

		//Rendering
		GameRenderer				renderer;
	public:
		bool						waitingThreadEnd;

	};
}
