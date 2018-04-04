#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Server.h"
using namespace Pong;

int main() {
	Server server;
	server.Run();

	return 0;
}
