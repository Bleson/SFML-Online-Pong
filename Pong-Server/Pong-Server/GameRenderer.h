#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "GameSimulation.h"

namespace Pong{
	class GameRenderer
	{

	public:
		GameRenderer(sf::Vector2u windowSize, sf::String windowTitle, GameSimulation* game);
		~GameRenderer();
		void Draw();

	private:
		void DrawPlayers();
	private:
		GameSimulation *gameToRender;
		sf::Vector2f origoPosition;
		sf::RenderWindow window;
		sf::Font font;
	};
}
