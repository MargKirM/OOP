#include <iostream>
#include "console_renderer.h"
#include "game.h"

int main() {
    using namespace lab2;

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