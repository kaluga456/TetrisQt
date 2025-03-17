#include <string>
#include <format>
#include "about.h"
#include "options.h"
#include "MainScene.h"
#include "TetrisEngine.h"

//sizes and colors
constexpr qreal BLOCK_SIZE = 20;
constexpr qreal GAME_FIELD_WIDTH = BLOCK_SIZE * tetris::GAME_FIELD_WIDTH;
constexpr qreal GAME_FIELD_HEIGHT = BLOCK_SIZE * tetris::GAME_FIELD_HEIGHT;

constexpr qreal  MAIN_BLOCK_PAD = 4;
constexpr qreal  MAIN_WALL_PAD = 2;
constexpr QColor MAIN_BG_COLOR = QColor{32, 32, 32};
constexpr QColor MAIN_TEXT_COLOR = QColor{200,200,200};

constexpr qreal  HORZ_DEL_SIZE = 8;
constexpr qreal  VERT_DEL_SIZE = 4;
constexpr QColor DELIMITER_COLOR = QColor{200,200,200};

constexpr qreal  STAT_WIDTH = 100;
constexpr qreal  STAT_PAD = 6;
constexpr QColor STAT_BG_COLOR = QColor{32, 32, 32};
constexpr QColor STAT_TEXT_COLOR = QColor{200,200,200};

//main text
constexpr const char* PAUSED_TEXT = "PAUSED";
constexpr const char* GAME_OVER_TEXT = "GAME OVER";

extern tetris::engine TetrisGame;

QMainScene::QMainScene(QObject *parent) : QGraphicsScene{parent}
{
}
void QMainScene::Init()
{
    //set background
    const QPen no_pen{Qt::PenStyle::NoPen};
    const QBrush bg_brush{MAIN_BG_COLOR, Qt::BrushStyle::SolidPattern};
    QRectF scene_rect{0, 0,
        MAIN_WALL_PAD + GAME_FIELD_WIDTH + MAIN_WALL_PAD + HORZ_DEL_SIZE + STAT_PAD + STAT_WIDTH,
        MAIN_WALL_PAD + GAME_FIELD_HEIGHT + MAIN_WALL_PAD};
    giBackground = addRect(scene_rect, no_pen, bg_brush);
    giBackground->setEnabled(false);
    //setBackgroundBrush(bg_brush);

    QRectF game_field_rect {MAIN_WALL_PAD, MAIN_WALL_PAD, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT};
    GameFieldFrame = {0, 0,
                            MAIN_WALL_PAD + game_field_rect.width() + MAIN_WALL_PAD,
                            MAIN_WALL_PAD + game_field_rect.height() + MAIN_WALL_PAD};

    //delimiter wall
    giVertDelimiter = addRect(QRectF{GameFieldFrame.right(),
                            MAIN_WALL_PAD,
                            HORZ_DEL_SIZE,
                            GAME_FIELD_HEIGHT - MAIN_BLOCK_PAD},
                            no_pen, QBrush{DELIMITER_COLOR, Qt::SolidPattern});
    giVertDelimiter->setEnabled(false);

    //init blocks
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            QRectF rect{game_field_rect.left() + BLOCK_SIZE * x,
                        game_field_rect.top() + BLOCK_SIZE * y,
                        BLOCK_SIZE - MAIN_BLOCK_PAD,
                        BLOCK_SIZE - MAIN_BLOCK_PAD};
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
    QFont stat_font{"Lucida Console", 10, QFont::Bold};
    textScore = addSimpleText("Score\t 9999", stat_font);
    textSpeed = addSimpleText("Speed\t 9999", stat_font);
    textTime = addSimpleText("Time\t --:--:--  ", stat_font);
    textScore->setBrush(QBrush{STAT_TEXT_COLOR});
    textSpeed->setBrush(QBrush{STAT_TEXT_COLOR});
    textTime->setBrush(QBrush{STAT_TEXT_COLOR});

    const qreal stat_left = GameFieldFrame.right() + HORZ_DEL_SIZE + STAT_PAD;
    const qreal text_height = textScore->boundingRect().height();
    textScore->setPos(QPointF{stat_left, STAT_PAD});
    textSpeed->setPos(QPointF{stat_left, STAT_PAD + text_height});
    textTime->setPos(QPointF{stat_left, STAT_PAD + 2*text_height});

    //update scene rect
    scene_rect = sceneRect();
    scene_rect.setWidth(scene_rect.width() + STAT_PAD);
    giBackground->setRect(scene_rect);

    StatFrame = QRectF{GameFieldFrame.right() + HORZ_DEL_SIZE,
                       0,
                       scene_rect.width() - (GameFieldFrame.right() + HORZ_DEL_SIZE),
                       STAT_PAD + 4 * text_height};

    giHorzDelimiter1 = addRect(QRectF{StatFrame.left() + STAT_PAD,
                                      StatFrame.height(),
                                      StatFrame.width() - 2*STAT_PAD,
                                      VERT_DEL_SIZE},
                               no_pen, QBrush{DELIMITER_COLOR, Qt::SolidPattern});
    giHorzDelimiter1->setEnabled(false);

    //TODO: best results frame
    BestFrame = QRectF{GameFieldFrame.right() + HORZ_DEL_SIZE,
                       StatFrame.height() + HORZ_DEL_SIZE,
                       StatFrame.width(),
                       0};
    QFont best_font{"Lucida Console", 8, QFont::Bold};
    textBest = addSimpleText(APP_HELP, best_font);
    textBest->setBrush(QBrush{STAT_TEXT_COLOR});
    UpdateBest();
    BestFrame.setHeight(STAT_PAD + textBest->boundingRect().height() + STAT_PAD);

    giHorzDelimiter2 = addRect(QRectF{BestFrame.left() + STAT_PAD,
                                      StatFrame.height() + VERT_DEL_SIZE + BestFrame.height(),
                                      StatFrame.width() - 2*STAT_PAD,
                                      VERT_DEL_SIZE},
                               no_pen, QBrush{DELIMITER_COLOR, Qt::SolidPattern});
    giHorzDelimiter2->setEnabled(false);

    //help frame
    HelpFrame = QRectF{GameFieldFrame.right() + HORZ_DEL_SIZE,
                       StatFrame.height() + VERT_DEL_SIZE + BestFrame.height() + VERT_DEL_SIZE,
                       StatFrame.width(),
                       scene_rect.height() - (StatFrame.height() + VERT_DEL_SIZE + BestFrame.height() + VERT_DEL_SIZE)};
    QFont help_font{"Lucida Console", 8, QFont::Bold};
    textHelp = addSimpleText(APP_HELP, help_font);
    textHelp->setPos(QPointF{HelpFrame.left() + STAT_PAD, HelpFrame.top() + STAT_PAD});
    textHelp->setBrush(QBrush{STAT_TEXT_COLOR});
}

void QMainScene::RePaint()
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
void QMainScene::DrawGameOver(bool val)
{
    Text->setText(val ? GAME_OVER_TEXT : "");
}
void QMainScene::DrawPause(bool val)
{
    Text->setText(val ? PAUSED_TEXT : "");
}
void QMainScene::SetText(const QString& text)
{
    Text->setText(text);
    QRectF text_rect = Text->boundingRect();
    Text->setPos(QPointF{(GameFieldFrame.width() - text_rect.width()) / 2, (GameFieldFrame.height() - text_rect.height()) / 2});
}
void QMainScene::ResetStat()
{
    SetScore(0);
    SetSpeed(0);
    SetTime(0);
}
void QMainScene::SetScore(int val)
{
    textScore->setText(QString("Score\t %1").arg(val));
}
void QMainScene::SetSpeed(int val)
{
    textSpeed->setText(QString("Speed\t %1%").arg(val));
}
void QMainScene::SetTime(int val)
{
    if(0 == val)
        textTime->setText("Time\t --:--:--");

    //TODO:
}
void QMainScene::UpdateBest()
{
    std::string text = "Best score:\n";
    CBestScore::const_iterator i = Options.BestResults.beign();
    for(int index = 1; index <= CBestScore::MAX_RESULTS; ++index)
    {
        if(i != Options.BestResults.end())
        {
            constexpr const char* fs = "{:3d}{:7d}\t{:02d}.{:02d}.{:04d}\n";
            const QDate& date = i->second;
            text += std::format(fs, index, i->first, date.month(), date.day(), date.year());
            ++i;
        }
        else
        {
            constexpr const char* fs = "{:3d}{:7d}\t{:s}\n";
            text += std::format(fs, index, 0, "--.--.----");
        }
    }

    textBest->setText(text.c_str());
    textBest->setPos(BestFrame.left() + STAT_PAD, BestFrame.top() + STAT_PAD);
}
