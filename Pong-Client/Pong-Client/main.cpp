#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/FileInputStream.hpp>
#include "Enumerators.h"
#include "ServerListener.h"
#include "GameSimulation.h"
#include <iostream>
#include "GameRenderer.h"
#include <fstream>
#include <stdio.h>

using namespace Pong;

bool isMovingUp = false;
bool isMovingDown = false;

void SendMovementPacket(sf::Int8 value, sf::TcpSocket *socket){
	sf::Packet packet;
	packet << static_cast<sf::Int32>(ClientMessage::Move);
	packet << value;
	socket->send(packet);
	//std::cout << "Sent packet with value " << value << "\n";
}

void handlePlayerInput(sf::Keyboard::Key key, bool isPressed, sf::TcpSocket *socket, GameSimulation& game)
{
	bool wasMovementInput = false;
	if (key == sf::Keyboard::W)
	{
		if (isMovingUp == isPressed)
			return;
		wasMovementInput = true;
		isMovingUp = isPressed;
	}
	else if (key == sf::Keyboard::S)
	{
		if (isMovingDown == isPressed)
			return;
		wasMovementInput = true;
		isMovingDown = isPressed;
	}

	if (wasMovementInput)
	{
		if (isMovingUp == isMovingDown)
		{
			SendMovementPacket(0, socket);
			game.UpdateInput(0);
		}
		else
		{
			SendMovementPacket((isMovingUp ? -1 : 1), socket);
			game.UpdateInput((isMovingUp ? -1 : 1));
		}
	}
}

int main()
{
	ServerListener sl;
	sf::TcpSocket* socket = sl.GetSocket();
	GameSimulation game(2);
	GameRenderer renderer(sf::Vector2u(320, 240), "Pong - Client", &game);
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	//Read IP from file

	std::string server_ip;
	std::ifstream file;
	file.open("config.txt", std::ios::in);
	if (!file.is_open()){
		printf("Can't open file!");
	}else {
		printf("Opened file \n");
		std::getline(file, server_ip);
	}
	file.close();

	sf::Socket::Status status = socket->connect(server_ip, 53000);
	{
		std::string print = "Trying to connect to '" + server_ip + "' \n";
		printf(print.c_str());
	}

	if (status != sf::Socket::Done)
	{
		printf("Connection failure! \n");
	}
	else
	{
		printf("Connection successful! \n");
	}

	sl.Start(&game);

	while (renderer.window.isOpen())
	{
		sf::Event event;
		while (renderer.window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				handlePlayerInput(event.key.code, true, socket, game);
				break;
			case sf::Event::KeyReleased:
				handlePlayerInput(event.key.code, false, socket, game);
				break;
			case sf::Event::Closed:
				renderer.window.close();
				break;
			}
			if (event.type == sf::Event::Closed)
				renderer.window.close();
		}

		timeSinceLastUpdate += clock.restart();
		//std::cout << timeSinceLastUpdate.asSeconds() << "\n";
		game.UpdateSimulation(timeSinceLastUpdate.asSeconds());
		timeSinceLastUpdate = sf::seconds(0.f);

		renderer.Draw();
		sf::sleep(sf::seconds(1.0 / 120.0));
	}

	return 0;
}
