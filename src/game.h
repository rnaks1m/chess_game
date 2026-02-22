#pragma once

#include "oxygine-framework.h"
#include "oxygine/core/oxygine.h"
#include "oxygine/tween/Tween.h"
#include "oxygine/actor/ColorRectSprite.h"

enum class FigureType {
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN
};

DECLARE_SMART(Figure, spFigure);
class Figure : public oxygine::Sprite {
public:
    Figure(FigureType type, bool is_white, int x, int y);
    bool IsWhite();
    int GetX();
    int GetY();
    FigureType GetType();

    void SetPos(int x, int y);

private:
    FigureType type_;
    bool is_white_;
    int line_x_;
    int line_y_;
};

DECLARE_SMART(Game, spGame);
class Game : public oxygine::Actor {
public:
    Game();

    void Initialization();
    void UpdateSizeWindow(const oxygine::Vector2& new_size);

private:
    void CreateBoard();
    void SetupFigures();
    void OneCellClick(oxygine::Event* ev);

    void AddFigure(FigureType type, bool is_white, int x, int y);
    bool IsValidMove(spFigure figure, int x, int y);
    void ShowPossibleMoves(spFigure figure);
    void ClearMoveMarkers();

    spFigure board_[8][8];
    spFigure selected_figure_ = nullptr;
    bool white_turn_ = true;
    oxygine::spColorRectSprite selection_marker_ = nullptr;
    std::vector<oxygine::spColorRectSprite> move_markers_;
};