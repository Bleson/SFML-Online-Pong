#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "GameSimulation.h"
#include "SFML/System/String.hpp"

namespace Pong{
	class GameRenderer
	{

	public:
		GameRenderer(sf::Vector2u windowSize, sf::String windowTitle, GameSimulation* game);
		~GameRenderer();
		void Draw();

	private:
		void DrawPlayers();
	public:
		sf::RenderWindow window;
	private:
		GameSimulation *gameToRender;
		sf::Vector2f origoPosition;
		sf::Font font;
		const sf::String winText = "YOU WON!";
		const sf::String loseText = "YOU LOST!";

	};
}
