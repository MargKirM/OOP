#ifndef LAB1_CONSOLE_RENDERER_H_
#define LAB1_CONSOLE_RENDERER_H_

#include "game.h"

namespace lab2 {

class ConsoleRenderer {
public:
    explicit ConsoleRenderer(const Game& game); // constructor with one argument(explicit)
    void Render() const;

private:
    char GetCellSymbol(int x, int y) const;
    const Game& game_;
};

}

#endif