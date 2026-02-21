#include "game.h"

extern oxygine::Resources game_resources;

using namespace oxygine;

Figure::Figure(FigureType type, bool is_white, int x, int y) : 
    type_(type), 
    is_white_(is_white), 
    line_x_(x), 
    line_y_(y) {}

bool Figure::IsWhite() {
    return is_white_;
}

int Figure::GetX() {
    return line_x_;
}

int Figure::GetY() {
    return line_y_;
}

void Figure::SetPos(int x, int y) {
    line_x_ = x;
    line_y_ = y;
}

FigureType Figure::GetType() {
    return type_;
}

Game::Game() {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            board_[x][y] = nullptr;
        }
    }
}

void Game::Initialization() {
    setName("chess_game");
    setSize(512, 512);

    // клик по Game, игнор остального
    setTouchChildrenEnabled(false);

    CreateBoard();

    // выделение клетки
    selection_marker_ = new ColorRectSprite();
    selection_marker_->setSize(64, 64);
    selection_marker_->setColor(Color(255, 255, 0, 150)); 
    selection_marker_->setVisible(false);
    addChild(selection_marker_);

    SetupFigures();
    addEventListener(TouchEvent::CLICK, CLOSURE(this, &Game::OneCellClick));
}

void Game::CreateBoard() {
    for (int x(0); x < 8; ++x) {
        for (int y(0); y < 8; ++y) {
            spColorRectSprite cell = new ColorRectSprite();
            cell->setSize(64, 64);
            cell->setPosition(x * 64, y * 64);
            cell->setColor((x + y) % 2 == 0 ? Color(190, 190, 180) : Color(80, 40, 5));
            cell->setPriority(-1);
            addChild(cell);
        }
    }
}

void Game::AddFigure(FigureType type, bool is_white, int x, int y) {
    spFigure figure = new Figure(type, is_white, x, y);
    std::string name = is_white ? "w_" : "b_";

    switch (type) {
        case FigureType::KING:   name += "king";   break;
        case FigureType::QUEEN:  name += "queen";  break;
        case FigureType::ROOK:   name += "rook";   break;
        case FigureType::BISHOP: name += "bishop"; break;
        case FigureType::KNIGHT: name += "knight"; break;
        case FigureType::PAWN:   name += "pawn";   break;
    }

    // ресурсы
    ResAnim* res = game_resources.getResAnim(name);
    if (res) {
        figure->setResAnim(res);
    } 
    else {
        oxygine::logs::error("Resource not found: %s", name);
    }

    figure->setAnchor(0.5f, 0.5f);
    figure->setPosition(x * 64 + 32, y * 64 + 32);

    addChild(figure);
    board_[x][y] = figure;
}

void Game::SetupFigures() {
    FigureType back_line[] = { 
        FigureType::ROOK, 
        FigureType::KNIGHT, 
        FigureType::BISHOP, 
        FigureType::QUEEN, 
        FigureType::KING, 
        FigureType::BISHOP, 
        FigureType::KNIGHT, 
        FigureType::ROOK 
    };

    // расстановка фигур на доске
    for (int x(0); x < 8; ++x) {
        AddFigure(back_line[x], true, x, 7);
        AddFigure(FigureType::PAWN, true, x, 6);

        AddFigure(FigureType::PAWN, false, x, 1);
        AddFigure(back_line[x], false, x, 0);
    }
}

void Game::OneCellClick(Event* ev) {
    TouchEvent* te = safeCast<TouchEvent*>(ev);
    
    // перевод глобальной позиции в локальную для Game
    Vector2 localPos = te->localPosition;
    
    int new_x = localPos.x / 64;
    int new_y = localPos.y / 64;

    if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) {
        return; 
    }

    if (!selected_figure_) {
        spFigure figure = board_[new_x][new_y];
        
        if (figure && figure->IsWhite() == white_turn_) {
            selected_figure_ = figure;
            
            // установка маркера на выбранную фигуру
            selection_marker_->setPosition(new_x * 64, new_y * 64);
            selection_marker_->setVisible(true);
            ShowPossibleMoves(figure);
        }
    }
    else {

        // перевыбор своей фигуры
        if (board_[new_x][new_y] && board_[new_x][new_y]->IsWhite() == white_turn_) {
            selected_figure_ = board_[new_x][new_y];
            selection_marker_->setPosition(new_x * 64, new_y * 64);
            ShowPossibleMoves(selected_figure_);
            return;
        }

        // ход
        if (IsValidMove(selected_figure_, new_x, new_y)) {
            board_[selected_figure_->GetX()][selected_figure_->GetY()] = nullptr;

            // срез фигуры противника
            if (board_[new_x][new_y]) {
                spFigure enemy = board_[new_x][new_y];
                enemy->addTween(Actor::TweenAlpha(0), 400)->detachWhenDone();
            }

            selected_figure_->SetPos(new_x, new_y);
            board_[new_x][new_y] = selected_figure_;

            // перемещение на новуб позицию
            Vector2 target_pos(new_x * 64 + 32, new_y * 64 + 32);
            selected_figure_->addTween(Actor::TweenPosition(target_pos), 300, 1, false, 0, Tween::EASE::ease_inOutSin);

            white_turn_ = !white_turn_;
        }

        selected_figure_ = nullptr;
        selection_marker_->setVisible(false);
        ClearMoveMarkers();
    }
}

bool Game::IsValidMove(spFigure figure, int new_x, int new_y) {
    int cur_x = figure->GetX();
    int cur_y = figure->GetY();
    int dx = abs(new_x - cur_x);
    int dy = abs(new_y - cur_y);

    // запрет хода на ту же клетку
    if (cur_x == new_x && cur_y == new_y) {
        return false;
    }

    // запрет среза своей фигуры
    if (board_[new_x][new_y] && board_[new_x][new_y]->IsWhite() == figure->IsWhite()) {
        return false;
    }

    switch (figure->GetType()) {
        case FigureType::PAWN: {
            int dir = figure->IsWhite() ? -1 : 1;

            // ход пешкой вперед
            if (new_x == cur_x && !board_[new_x][new_y]) {

                // на 1 клетку
                if (new_y == cur_y + dir) {
                    return true;
                }

                int start_pos = figure->IsWhite() ? 6 : 1;

                // на 2 клетки со стартовой позиции
                if (cur_y == start_pos && new_y == cur_y + 2 * dir && !board_[cur_x][cur_y + dir]) {
                    return true;
                }
            }

            // срез фигуры пешкой
            if (dx == 1 && new_y == cur_y + dir && board_[new_x][new_y]) {
                return true;
            }

            return false;
        }

        case FigureType::KING:
            return dx <= 1 && dy <= 1;
        case FigureType::QUEEN:
            if (dx != dy && cur_x != new_x && cur_y != new_y) {
                return false;
            }
            break;
        case FigureType::KNIGHT:
            return (dx == 1 && dy == 2) || (dy == 1 && dx == 2);
        case FigureType::ROOK:
            if (cur_x != new_x && cur_y != new_y) {
                return false;
            }
            break;
        case FigureType::BISHOP:
            if (dx != dy) {
                return false;
            }
            break;
    }

    // выбор направления и минимального шага
    int step_x = (new_x > cur_x) ? 1 : (new_x < cur_x ? -1 : 0);
    int step_y = (new_y > cur_y) ? 1 : (new_y < cur_y ? -1 : 0);

    // следующая минимальная позиция при движении
    int next_pos_x = cur_x + step_x;
    int next_pos_y = cur_y + step_y;

    // проверка на фигуры на пути движения для фигур с длинным перемещением (слон, ладья, ферзь)
    while (next_pos_x != new_x || next_pos_y != new_y) {      
        // ограничение хода при наличии фигура на пути движения
        if (board_[next_pos_x][next_pos_y]) {
            return false;
        }
        next_pos_x += step_x;
        next_pos_y += step_y;
    }
    return true;
}

void Game::ShowPossibleMoves(spFigure figure) {
    ClearMoveMarkers();

    for (int x(0); x < 8; ++x) {
        for (int y(0); y < 8; ++y) {

            // отображение маркеров для возможных ходов
            if (IsValidMove(figure, x, y)) {
                spColorRectSprite marker = new ColorRectSprite();
                marker->setSize(20, 20);
                marker->setAnchor(0.5f, 0.5f);
                marker->setPosition(x * 64 + 32, y * 64 + 32);
                marker->setColor(Color(0, 255, 0, 100));
                marker->setPriority(1);
                addChild(marker);
                move_markers_.push_back(marker);
            }
        }
    }
}

void Game::ClearMoveMarkers() {
    for (auto marker : move_markers_) {
        marker->detach();
    }
    move_markers_.clear();
}

// масштабирование игры при изменении размера окна
void Game::UpdateSizeWindow(const Vector2& new_size) {
    float scale = std::min(new_size.x / 512.0f, new_size.y / 512.0f);
    setScale(scale);
    setPosition((new_size.x - 512.0f * scale) / 2, (new_size.y - 512.0f * scale) / 2);
}