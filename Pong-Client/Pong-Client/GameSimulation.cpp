#include "GameSimulation.h"
#include "Enumerators.h"
#include <iostream>
#include <limits>
#include <math.h>

namespace Pong{
	GameSimulation::GameSimulation(int playerCount) :
		ball(sf::Vector2f(5.f, 5.f), sf::Vector2f(0,0))
	{
		playerInputs.empty();
		players.empty();
		walls.empty();
		scores.empty();

		for (int i = 0; i < playerCount; i++)
		{
			playerInputs.push_back(0);
			players.push_back(RectangleObject(sf::Vector2f(5.f, 20.f)));
			scores.push_back(0);
		}
		players[0].box.x = -ballMaxPosition + 15;
		players[1].box.x = ballMaxPosition - 15;
		players[0].box.y = 0;
		players[1].box.y = 0;

		walls.push_back(RectangleObject(sf::Vector2f(275.f, 5.f)));
		walls.push_back(RectangleObject(sf::Vector2f(275.f, 5.f)));
		walls[0].box.y = 90;
		walls[1].box.y = -90;
	}

	GameSimulation::~GameSimulation() {

	}
	void GameSimulation::UpdateInput(int movementInput)
	{
		playerInputs[playerNumber] = movementInput;
	}

	void GameSimulation::UpdateInput(int player, int movementInput)
	{
		playerInputs[player] = movementInput;
	}

	void GameSimulation::UpdateSimulation(float deltaTime)
	{
		UpdatePlayerPositions(deltaTime);
		UpdateBallPosition(deltaTime);
		//CheckForScore();
	}

	void GameSimulation::SetPlayerNumber(int playerNumber)
	{
		this->playerNumber = playerNumber;
	}

	int& GameSimulation::GetPlayerNumber()
	{
		return playerNumber;
	}

	void GameSimulation::EndGame(int winner)
	{
		ball.box.vx = 0;
		ball.box.vy = 0;
		this->winner = winner;
	}

	sf::Packet GameSimulation::GetSnapshot()
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(ServerMessage::Snapshot)
			<< ball.box.x << ball.box.y
			<< ball.box.vx << ball.box.vy;

		for each (auto var in players)
		{
			packet << var.box.y;
		}
		return packet;
	}

	std::vector<GameSimulation::RectangleObject> * GameSimulation::GetPlayers()
	{
		return &players;
	}

	std::vector<Pong::GameSimulation::RectangleObject> * GameSimulation::GetWalls()
	{
		return &walls;
	}

	std::vector<unsigned int> & GameSimulation::GetScores()
	{
		return scores;
	}

	int& GameSimulation::GetWinner()
	{
		return winner;
	}

	GameSimulation::RectangleObject * GameSimulation::GetBall()
	{
		return &ball;
	}

	void GameSimulation::UpdatePlayerPositions(float& deltaTime)
	{
		for (int i = 0; i < players.size(); i++)
		{
			float newPos = players[i].box.y + deltaTime * (float)playerInputs[i] * playerSpeed;
			if (newPos > playerMaxPosition)
			{
				newPos = playerMaxPosition;
			}
			else if (newPos < -playerMaxPosition)
			{
				newPos = -playerMaxPosition;
			}
			players[i].box.y = newPos;
			//std::cout << "Player's new position: " << newPos << "\n";
		}
	}

	void GameSimulation::UpdateBallPosition(float& deltaTime)
	{
		float normalx, normaly, collisionTime, remainingTime;
		for each (auto var in walls)
		{
			BoxCollision(normalx, normaly, collisionTime, remainingTime, deltaTime, var);
		}

		for each (auto var in players)
		{
			BoxCollision(normalx, normaly, collisionTime, remainingTime, deltaTime, var);
		}
		ball.box.x += ball.box.vx * deltaTime;
		ball.box.y += ball.box.vy * deltaTime;

		//std::cout << "Ball velocity: " << ball.box.vx << ", " << ball.box.vy << "\n";
	}

	float GameSimulation::SweptAABB(Box b1, Box b2, float& normalx, float& normaly, float& deltaTime)
	{
		b2.y -= b2.h / 2.f;
		b1.y -= b1.h / 2.f;
		b2.x -= b2.w / 2.f;
		b1.x -= b1.w / 2.f;

		float xInvEntry, yInvEntry;
		float xInvExit, yInvExit;
		// find the distance between the objects on the near and far sides for both x and y
		if (b1.vx > 0.0f)
		{
			xInvEntry = b2.x - (b1.x + b1.w);
			xInvExit = (b2.x + b2.w) - b1.x;
		}
		else
		{
			xInvEntry = (b2.x + b2.w) - b1.x;
			xInvExit = b2.x - (b1.x + b1.w);
		}
		if (b1.vy > 0.0f)
		{
			yInvEntry = b2.y - (b1.y + b1.h);
			yInvExit = (b2.y + b2.h) - b1.y;
		}
		else
		{
			yInvEntry = (b2.y + b2.h) - b1.y;
			yInvExit = b2.y - (b1.y + b1.h);
		}

		// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
		float xEntry, yEntry;
		float xExit, yExit;
		if (b1.vx == 0.0f)
		{
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			xEntry = xInvEntry / (b1.vx  * deltaTime);
			xExit = xInvExit / (b1.vx  * deltaTime);
		}
		if (b1.vy == 0.0f)
		{
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}
		else
		{
			yEntry = yInvEntry / (b1.vy  * deltaTime);
			yExit = yInvExit / (b1.vy  * deltaTime);
		}

		// find the earliest/latest times of collision
		float entryTime = std::max(xEntry, yEntry);
		float exitTime = std::min(xExit, yExit);

		// if there was no collision
		if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
		{
			normalx = 0.0f;
			normaly = 0.0f;
			return 1.0f;
		}
		else // if there was a collision
		{
			// calculate normal of collided surface
			if (xEntry > yEntry)
			{
				if (xInvEntry < 0.0f)
				{
					normalx = 1.0f;
					normaly = 0.0f;
				}
				else
				{
					normalx = -1.0f;
					normaly = 0.0f;
				}
			}
			else
			{
				if (yInvEntry < 0.0f)
				{
					normalx = 0.0f;
					normaly = 1.0f;
				}
				else
				{
					normalx = 0.0f;
					normaly = -1.0f;
				}
			}
			// return the time of collision
			//std::cout << "Collision, entryTime " << entryTime << "\n";
			return entryTime;
		}
	}

	void GameSimulation::CheckForScore()
	{
		if (ball.box.x >= ballMaxPosition)
		{
			scores[0]++;
		}
		else if (ball.box.x <= -ballMaxPosition)
		{
			scores[1]++;
		}
		else
		{
			return;
		}
		//Send message of score change to players
		//Reset ball position
		ball.box.x = 0;
		ball.box.y = 0;

		sf::Vector2f ballVelocity = RandomBallVelocity();
		ball.box.vx = ballVelocity.x;
		ball.box.vy = ballVelocity.y;
	}

	sf::Vector2f GameSimulation::RandomBallVelocity()
	{
		sf::Vector2f velocity;
		do
		{
			velocity = sf::Vector2f((rand() % 100 + 1), (rand() % 100 + 1));
			//velocity = sf::Vector2f(1.f, 0.03f);
		} while (velocity.y > velocity.x);

		float length = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
		if (length != 0)
			velocity = sf::Vector2f(velocity.x / length, velocity.y / length);
		velocity *= ballStartingVelocity;

		if (rand() > 0.5f)
		{
			velocity.x *= -1.f;
		}
		if (rand() > 0.5f)
		{
			velocity.y *= -1.f;
		}

		return sf::Vector2f(0,0);
	}

	void GameSimulation::BoxCollision(float& normalx, float& normaly, float& collisionTime, float& remainingTime, float& deltaTime, RectangleObject& var)
	{
		collisionTime = SweptAABB(ball.box, var.box, normalx, normaly, deltaTime);
		if (collisionTime == 1.f)
		{
			return;
		}
		collisionTime *= deltaTime;
		ball.box.x += ball.box.vx * collisionTime * deltaTime;
		ball.box.y += ball.box.vy * collisionTime * deltaTime;
		remainingTime = 1.f - collisionTime;

		// deflect
		ball.box.vx *= remainingTime;
		ball.box.vy *= remainingTime;
		if (abs(normalx) > 0.0001f)
			ball.box.vx = -ball.box.vx * 1.1f;;
		if (abs(normaly) > 0.0001f)
			ball.box.vy = -ball.box.vy * 1.1f;;
	}

	GameSimulation::RectangleObject::RectangleObject(sf::Vector2f size)
	{
		box.w = size.x;
		box.h = size.y;
	}

	GameSimulation::RectangleObject::RectangleObject(sf::Vector2f size, sf::Vector2f velocity)
	{
		box.w = size.x;
		box.h = size.y;
		box.vx = velocity.x;
		box.vy = velocity.y;
	}
}