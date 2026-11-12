#ifndef LAB1_CONSOLE_RENDERER_H_
#define LAB1_CONSOLE_RENDERER_H_

#include "game.h"

namespace lab2 {


enum class RenderMode {
    kNormal,            
    kHighlightEnemies,   
    kHighlightWalls,
    kHighlightStatusCells  // cells with status
};

class ConsoleRenderer {
public:
    explicit ConsoleRenderer(const Game& game); // constructor with one argument(explicit)
    void Render() const;

    AbilityKind ChooseAbility(const Robot& robot, bool rank_up) const;

    void SetMode(RenderMode mode) { mode_ = mode; }
    RenderMode GetMode() const { return mode_; }
    void ToggleMode();   // переключение режима по кругу

private:
    char GetCellSymbol(int x, int y) const;
    bool ShouldHighlight(int x, int y) const;
    const Game& game_;
    RenderMode mode_ = RenderMode::kNormal;
};

}

#endif