#include "GameRenderer.h"
#include "GameSimulation.h"
#include <iostream>

namespace Pong{
	GameRenderer::GameRenderer(sf::Vector2u windowSize, sf::String windowTitle, GameSimulation* game) :
		origoPosition(windowSize.x / 2.f, windowSize.y / 2.f + 20.f)
		, window(sf::VideoMode(windowSize.x, windowSize.y), "Pong - Server")
	{
		if (game == nullptr)
		{
			printf("NULL POINTER called game in GameRenderer");
		}
		else
		{
			gameToRender = game;
		}

		if (!font.loadFromFile("SHPinscher-Regular.otf"))
		{
			std::cout << "Couldn't load the font \n";
		}
	}


	GameRenderer::~GameRenderer()
	{
	}

	void GameRenderer::Draw()
	{
		window.clear();
		sf::String textToDraw = "";
		for each (auto var in gameToRender->GetScores())
		{
			textToDraw += std::to_string(var) + " - ";
		}
		textToDraw.erase(textToDraw.getSize() - 3, 3);

		//Draw score
		sf::Text txt;
		txt.setFont(font);
		txt.setString(textToDraw);
		txt.setCharacterSize(30);
		txt.setFillColor(sf::Color::White);
		txt.setOutlineColor(sf::Color::White);
		txt.setPosition(origoPosition + sf::Vector2f((textToDraw.getSize() * 11) / -2.f, -135));
		window.draw(txt);

		//Draw players
		for each (auto var in *gameToRender->GetPlayers())
		{
			var.texture.setPosition(origoPosition.x + var.box.x - var.box.w / 2.f,
				origoPosition.y + var.box.y - var.box.h / 2.f);
			var.texture.setSize(sf::Vector2f(var.box.w, var.box.h));
			window.draw(var.texture);

			//std::cout << "Rendering player to position ("
			//	<< (origoPosition.x + var.box.x - var.box.w / 2.f) << ", "
			//	<< (origoPosition.y + var.box.y - var.box.h / 2.f) << ") of size ("
			//	<< var.box.w << ", " << var.box.h << ")\n";
		}

		for each (auto var in *gameToRender->GetWalls())
		{
			var.texture.setPosition(origoPosition.x + var.box.x - var.box.w / 2.f,
				origoPosition.y + var.box.y - var.box.h / 2.f);
			var.texture.setSize(sf::Vector2f(var.box.w, var.box.h));
			window.draw(var.texture);
			//std::cout << "Rendering Wall to position ("
			//	<< (origoPosition.x + var.box.x - var.box.w / 2.f) << ", "
			//	<< (origoPosition.y + var.box.y - var.box.h / 2.f) << ") of size ("
			//	<< var.box.w << ", " << var.box.h << ")\n";
		}

		//Draw ball
		gameToRender->GetBall()->texture.setPosition(
			origoPosition.x + gameToRender->GetBall()->box.x - gameToRender->GetBall()->box.w / 2.f,
			origoPosition.y + gameToRender->GetBall()->box.y - gameToRender->GetBall()->box.h / 2.f);
		gameToRender->GetBall()->texture.setSize(sf::Vector2f(
			gameToRender->GetBall()->box.w,
			gameToRender->GetBall()->box.h));
		window.draw(gameToRender->GetBall()->texture);

		//std::cout << "Rendering ball to position ("
		//	<< (origoPosition.x + gameToRender->GetBall()->box.x) << ", "
		//	<< (origoPosition.y + gameToRender->GetBall()->box.y) << ") of size ("
		//	<< gameToRender->GetBall()->box.w << ", " << gameToRender->GetBall()->box.h << ")\n";
		window.display();
	}
}
