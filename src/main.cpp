#include "game.h"

using namespace oxygine;

Resources game_resources;

void ExampleInitialization() {
    // загрузка ресурсов
    game_resources.loadXML("data/resources.xml");
    game_resources.load();

    // создание игры
    spGame game = new Game();
    game->Initialization();
    getStage()->addChild(game);
}

int main (int argc, char* argv[]) {
    // структура с параметрами для движка
    core::init_desc desc;

    // задание параметров окна
    desc.title = "Chess Game";
    desc.w = 512;
    desc.h = 512;
    desc.resizable = true;

    // инициализация движка
    core::init(&desc);

    // создание сцены для игры
    Stage::instance = new Stage();
    Vector2 cur_display_size = core::getDisplaySize();
    getStage()->setSize(cur_display_size);

    ExampleInitialization();

    while (1) {
        // обновление систем движка
        bool done = core::update();

        if (done) {
            break;
        }

        Vector2 new_display_size = core::getDisplaySize();

        // обработка изменения масштаба окна
        if (new_display_size.x != cur_display_size.x || new_display_size.y != cur_display_size.y) {
            cur_display_size = new_display_size;

            getStage()->setSize(cur_display_size);

            spGame game = safeCast<Game*>(getStage()->getChild("chess_game").get());
            
            if (game) {
                game->UpdateSizeWindow(cur_display_size);
            }
        }

        // обновление всех объектов
        getStage()->update();

        // проверка готовности графической карты к отрисовке кадра
        if(core::beginRendering()) {
            // область отрисовки
            Rect viewport(0, 0, cur_display_size.x, cur_display_size.y);
            // отрисовка сцены
            getStage()->render(Color(32, 32, 32, 255), viewport);
            // вывод окадра на монитор
            core::swapDisplayBuffers();
        }
    }

    // освобождение памяти и закрытие всех систем
    core::release();
    return 0;
}