#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

namespace Pong{
	class Room;

	class GameSimulation {

	public:
		struct Box
		{
			// position of top-left corner
			float x = 0, y = 0;
			// dimensions
			float w, h;
			// velocity
			float vx, vy;
		};

		struct RectangleObject{
			RectangleObject(sf::Vector2f size);
			RectangleObject(sf::Vector2f size, sf::Vector2f velocity);

			Box box;
			sf::RectangleShape texture;
			sf::Color color;
		};


	public:
		GameSimulation(int playerCount);
		~GameSimulation();
		void UpdateInput(int player, int movementInput);
		void UpdateSimulation(float deltaTime);
		sf::Packet GetSnapshot();
		sf::Packet GetNewRoundPacket();
		std::vector<RectangleObject> *GetPlayers();
		std::vector<RectangleObject> *GetWalls();
		std::vector<unsigned int> &GetScores();
		RectangleObject *GetBall();

	private:
		void UpdatePlayerPositions(float& deltaTime);
		void UpdateBallPosition(float& deltaTime);
		//Returns percentage of time the box collided
		float SweptAABB(Box b1, Box b2, float& normalx, float& normaly, float& deltaTime);

		void CheckForScore();
		bool CheckForWinner();
		sf::Vector2f RandomBallVelocity();

		void BoxCollision(float& normalx, float& normaly, float& collisionTime, float& remainingTime, float& deltaTime, RectangleObject& var);
	
	public:
		Room* room;

	private:
		float const ballMaxPosition = 135.f;
		float const ballStartingVelocity = 50.f;
		float const playerMaxPosition = 75.0f;
		float const playerSpeed = 100.f;
		float const winningScore = 3;

		std::vector<int> playerInputs;
		std::vector<unsigned int> scores;
		std::vector<RectangleObject> players;
		std::vector<RectangleObject> walls;
		RectangleObject ball;
	};
}
