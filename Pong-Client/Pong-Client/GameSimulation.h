#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

namespace Pong{
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
		void UpdateInput(int movementInput);
		void UpdateSimulation(float deltaTime);
		void SetPlayerNumber(int playerNumber);
		int& GetPlayerNumber();
		void EndGame(int winner);
		sf::Packet GetSnapshot();
		std::vector<RectangleObject> *GetPlayers();
		std::vector<RectangleObject> *GetWalls();
		std::vector<unsigned int> &GetScores();
		int& GetWinner();
		RectangleObject *GetBall();
		void UpdateBallPosition(float& deltaTime);

	private:
		void UpdatePlayerPositions(float& deltaTime);
		//Returns percentage of time the box collided
		float SweptAABB(Box b1, Box b2, float& normalx, float& normaly, float& deltaTime);

		void CheckForScore();
		sf::Vector2f RandomBallVelocity();

		void BoxCollision(float& normalx, float& normaly, float& collisionTime, float& remainingTime, float& deltaTime, RectangleObject& var);
	private:
		int winner = -1;
		float const ballMaxPosition = 135.f;
		float const ballStartingVelocity = 50.f;
		float const playerMaxPosition = 75.0f;
		float const playerSpeed = 100.f;
		int playerNumber = -1;
		std::vector<int> playerInputs;
		std::vector<unsigned int> scores;
		std::vector<RectangleObject> players;
		std::vector<RectangleObject> walls;
		RectangleObject ball;
	};
}
