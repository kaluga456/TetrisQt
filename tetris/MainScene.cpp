#include "MainScene.h"
#include "TetrisEngine.h"

//sizes
constexpr qreal BLOCK_SIZE = 20;
constexpr qreal GAME_FIELD_WIDTH = BLOCK_SIZE * tetris::GAME_FIELD_WIDTH;
constexpr qreal GAME_FIELD_HEIGHT = BLOCK_SIZE * tetris::GAME_FIELD_HEIGHT;

constexpr qreal  MAIN_BLOCK_PAD = 4;
constexpr qreal  MAIN_WALL_PAD = 2;
constexpr QColor MAIN_BG_COLOR = QColor{32, 32, 32};
constexpr QColor MAIN_TEXT_COLOR = QColor{200,200,200};

constexpr qreal  MAIN_WALL_SIZE = 8;
constexpr QColor MAIN_WALL_COLOR = QColor{200,200,200};

constexpr qreal  STAT_WIDTH = 100;
constexpr qreal  STAT_PAD = 4;
constexpr QColor STAT_BG_COLOR = QColor{32, 32, 32};
constexpr QColor STAT_TEXT_COLOR = QColor{200,200,200};

//main text
constexpr const char* PAUSED_TEXT = "PAUSED";
constexpr const char* GAME_OVER_TEXT = "GAME OVER";

extern tetris::engine TetrisGame;

QGameFieldScene::QGameFieldScene(QObject *parent) : QGraphicsScene{parent}, IsGameOver{false}
{
}

void QGameFieldScene::Init()
{
    //set background
    const QPen no_pen{Qt::PenStyle::NoPen};
    const QBrush bg_brush{MAIN_BG_COLOR, Qt::BrushStyle::SolidPattern};
    QRectF scene_rect{0, 0,
        MAIN_WALL_PAD + GAME_FIELD_WIDTH + MAIN_WALL_PAD + MAIN_WALL_SIZE + STAT_PAD + STAT_WIDTH,
        MAIN_WALL_PAD + GAME_FIELD_HEIGHT + MAIN_WALL_PAD};
    giBackground = addRect(scene_rect, no_pen, bg_brush);
    //setBackgroundBrush(bg_brush);

    QRectF game_field_rect {MAIN_WALL_PAD, MAIN_WALL_PAD, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT};
    game_field_frame = {0, 0,
                            MAIN_WALL_PAD + game_field_rect.width() + MAIN_WALL_PAD,
                            MAIN_WALL_PAD + game_field_rect.height() + MAIN_WALL_PAD};

    //delimiter wall
    giWall = addRect(QRectF{game_field_frame.right(),
                            MAIN_WALL_PAD,
                            MAIN_WALL_SIZE,
                            GAME_FIELD_HEIGHT - MAIN_BLOCK_PAD},
                            no_pen, QBrush{MAIN_WALL_COLOR, Qt::SolidPattern});

    //init blocks
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            QRectF rect{game_field_rect.left() + BLOCK_SIZE * x,  game_field_rect.top() + BLOCK_SIZE * y, BLOCK_SIZE - MAIN_BLOCK_PAD, BLOCK_SIZE - MAIN_BLOCK_PAD};
            QGraphicsRectItem* gi = addRect(rect, no_pen);
            gi->hide();
            gi->setEnabled(false);
            Blocks[x][y] = gi;
            Context[x][y] = tetris::BLOCK_NONE;
        }
    }

    //init text
    QFont text_font{"Lucida Console", 20, QFont::Bold};
    Text = addSimpleText("", text_font);
    Text->setBrush(QBrush{MAIN_TEXT_COLOR});
    Text->setPen(QPen{Qt::black});

    //stat view
    QFont stat_font{"Lucida Console", 12, QFont::Bold};
    textScore = addSimpleText("Score\t 9999", stat_font);
    textSpeed = addSimpleText("Speed\t 9999", stat_font);
    textTime = addSimpleText("Time\t --:--:--  ", stat_font);
    textScore->setBrush(QBrush{STAT_TEXT_COLOR});
    textSpeed->setBrush(QBrush{STAT_TEXT_COLOR});
    textTime->setBrush(QBrush{STAT_TEXT_COLOR});

    const qreal stat_left = game_field_frame.right() + MAIN_WALL_SIZE + STAT_PAD;
    const qreal text_height = textScore->boundingRect().height();
    textScore->setPos(QPointF{stat_left, STAT_PAD});
    textSpeed->setPos(QPointF{stat_left, STAT_PAD + text_height});
    textTime->setPos(QPointF{stat_left, STAT_PAD + 2 * text_height});

    giBackground->setRect(sceneRect());
}

void QGameFieldScene::RePaint()
{
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            const tetris::block_t context = TetrisGame.get_block(x, y);
            if(context == Context[x][y])
                continue;

            QGraphicsRectItem* gi = Blocks[x][y];

            if(context == tetris::BLOCK_NONE)
            {
                gi->hide();
            }
            else
            {
                QBrush block_brush{QColor{context}, Qt::BrushStyle::SolidPattern};
                gi->setBrush(block_brush);
                gi->show();
            }
            Context[x][y] = context;
        }
    }
}
void QGameFieldScene::DrawGameOver(bool val)
{
    Text->setText(val ? GAME_OVER_TEXT : "");
}

void QGameFieldScene::DrawPause(bool val)
{
    Text->setText(val ? PAUSED_TEXT : "");
}
void QGameFieldScene::SetText(const QString& text)
{
    //TEST:
    Text->setText(GAME_OVER_TEXT);
    //Text->setText(text);

    QRectF text_rect = Text->boundingRect(); //item coords
    Text->setPos(QPointF{(game_field_frame.width() - text_rect.width()) / 2, (game_field_frame.height() - text_rect.height()) / 2});
}
