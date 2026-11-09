// #include <iostream>

// #include "console_renderer.h"
// #include "game.h"

// int main() {
// 	using lab1::ConsoleRenderer;
// 	using lab1::Direction;
// 	using lab1::Game;

// 	Game game(10, 10);
// 	game.ConfigureDemo();

// 	ConsoleRenderer renderer(game);

// 	std::cout << "Initial state:\n";
// 	renderer.Render();

// 	std::cout << "\nMove right:\n";
// 	for (int i = 0; i < 6; ++i) {
// 		if (!game.GetPlayer().IsAlive() || game.IsLevelComplete()) break;
// 		std::cout << "\n--- Step " << (i + 1) << " ---\n";
// 		game.Step(Direction::kRight);
// 		renderer.Render();
// 	}

// 	for (int i = 0; i < 6; ++i) {
// 		if (!game.GetPlayer().IsAlive() || game.IsLevelComplete()) break;
// 		std::cout << "\n--- Step " << (i + 1) << " ---\n";
// 		std::cout << "\nMove down:\n";
// 		game.Step(Direction::kDown);
// 		renderer.Render();
// 	}

// 	return 0;
// }


#include <iostream>
#include "console_renderer.h"
#include "game.h"

int main() {
    using namespace lab1;

    Game game(15, 15);
    game.ConfigureDemo();

    ConsoleRenderer renderer(game);

    while (game.GetPlayer().IsAlive() && !game.IsLevelComplete()) {
        renderer.Render();
        std::cout << "\nХод (w/a/s/d, q — выход): ";
        char c;
        std::cin >> c;

        Direction dir;
        switch (c) {
            case 'w': dir = Direction::kUp;    break;
            case 's': dir = Direction::kDown;  break;
            case 'a': dir = Direction::kLeft;  break;
            case 'd': dir = Direction::kRight; break;
            case 'q': return 0;
            default:  continue;
        }
        game.Step(dir);
    }

    renderer.Render();
    std::cout << (game.GetPlayer().IsAlive()
                    ? "Уровень пройден!\n"
                    : "Игрок погиб.\n");
}