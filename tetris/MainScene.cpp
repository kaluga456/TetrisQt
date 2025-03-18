#include <string>
#include <format>
#include <QtAssert>
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

constexpr qreal  STAT_WIDTH = 170;
constexpr qreal  STAT_PAD = 6;
constexpr QColor STAT_BG_COLOR = QColor{32, 32, 32};
constexpr QColor STAT_TEXT_COLOR = QColor{200,200,200};

extern tetris::engine TetrisGame;

static void get_shape_size(const tetris::shape_matrix_t& matrix, int& width, int& height)
{
    width = 0;
    height = 0;

    for(int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
    {
        int count_y = 0;
        bool has_x = false;
        count_y = 0;
        for(int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
        {
            if(tetris::BLOCK_NONE == matrix[x][y])
                continue;

            if(false == has_x)
            {
                has_x = true;
                ++width;
            }
            ++count_y;
        }

        if(height < count_y)
            height = count_y;
    }
}

void CGSRect::InitRect(QGraphicsScene *parent, const QRectF &rect, QColor bg_color)
{
    Q_ASSERT(parent);
    Parent = parent;
    Rect = rect;

    const QPen no_pen{Qt::PenStyle::NoPen};
    const QBrush bg_brush{bg_color, Qt::BrushStyle::SolidPattern};
    Background = Parent->addRect(Rect, no_pen, bg_brush);
}
void CGSRect::setWidth(qreal width)
{
    Rect.setWidth(width);
    Background->setRect(Rect);
}
void CGSRect::setHeight(qreal height)
{
    Rect.setHeight(height);
    Background->setRect(Rect);
}

void CGSText::Init(const QFont& font, QColor color, qreal padding)
{
    Q_ASSERT(nullptr == Text);
    if(Text) return;
    Text = Parent->addSimpleText("", font);

    QBrush brush{color, Qt::BrushStyle::SolidPattern};
    Text->setBrush(brush);
    Text->setPos(QPointF{Rect.left() + padding, Rect.top() + padding});
    Padding = padding;
}
void CGSText::setText(const char *text /*= nullptr*/)
{
    Q_ASSERT(Text);
    Text->setText(text ? text : "");
}
void CGSText::UpdateWidth()
{
    Q_ASSERT(Text);
    setWidth(Padding + Text->boundingRect().width() + Padding);
}
void CGSText::UpdateHeight()
{
    Q_ASSERT(Text);
    setHeight(Padding + Text->boundingRect().height() + Padding);
}

void CGSScore::Init(const QFont &font, QColor color, qreal padding)
{
    Q_ASSERT(nullptr == textScore);
    if(textScore) return;

    QBrush brush{color, Qt::BrushStyle::SolidPattern};
    textScore = Parent->addSimpleText("Score", font);
    textSpeed = Parent->addSimpleText("", font);
    textTime = Parent->addSimpleText("", font);
    textTime->setBrush(brush);
    textScore->setBrush(brush);
    textSpeed->setBrush(brush);

    const qreal text_height = textScore->boundingRect().height();
    const qreal line_height = text_height * 1.5;
    const qreal top = padding + (line_height - text_height) / 2;
    textScore->setPos(QPointF{Rect.left() + padding, Rect.top() + top});
    textSpeed->setPos(QPointF{Rect.left() + padding, Rect.top() + top + line_height});
    textTime->setPos(QPointF{Rect.left() + padding, Rect.top() + top + 2 * line_height});

    setHeight(2*padding + 3*line_height);
}

void CGSScore::SetScore(int val)
{
    Q_ASSERT(textScore);
    constexpr const char* fs =      "Score\t  {:8d}\n";
    std::string text = std::format(fs, val);
    textScore->setText(text.c_str());
}
void CGSScore::SetSpeed(int val)
{
    Q_ASSERT(textSpeed);
    constexpr const char* fs =      "Speed\t  {:7d}%\n";
    std::string text = std::format(fs, val);
    textSpeed->setText(text.c_str());
}
void CGSScore::SetTime(int val)
{
    Q_ASSERT(textTime);

    std::string text;
    constexpr const char* fs_def =  "Time\t  --:--:--\n";
    constexpr const char* fs =      "Time\t  {:02d}:{:02d}:{:02d}\n";
    if(0 == val)
    {
        textTime->setText(fs_def);
        return;
    }

    const int secs = val / 1000;
    const int hour = secs / 3600;
    const int min = (secs / 60) % 60;
    const int sec = secs % 60;
    text = std::format(fs, hour, min, sec);
    textTime->setText(text.c_str());
}

void CGSShape::Init()
{
    const qreal padding = 5;
    const qreal shape_size = Rect.height() - 2 * padding;
    const qreal block_size = shape_size / tetris::SHAPE_MATRIX_SIZE;

    const QPen no_pen{Qt::PenStyle::NoPen};
    for(int block_index = 0; block_index < tetris::SHAPE_BLOCKS_COUNT; ++block_index)
    {
        QRectF rect{Rect.left(), Rect.top(), block_size - MAIN_BLOCK_PAD, block_size - MAIN_BLOCK_PAD};
        QGraphicsRectItem* gi = Parent->addRect(rect, no_pen);
        gi->hide();
        Blocks[block_index] = gi;
    }
}
void CGSShape::setShape(const tetris::shape_t &shape)
{
    const qreal padding = 5;
    const qreal shape_size = Rect.height() - 2 * padding;
    const qreal block_size = shape_size / tetris::SHAPE_MATRIX_SIZE;

    int shape_width = 0;
    int shape_height = 0;
    int block_index = 0;

    const tetris::shape_matrix_t& matrix = *shape.get_matrix();
    get_shape_size(matrix, shape_width, shape_height);

    const qreal left = (Rect.width() - (block_size * shape_width)) / 2;
    const qreal top = (Rect.height() - (block_size * shape_height)) / 2;

    for(int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
        {
            tetris::block_t block = matrix[x][y];
            if(tetris::BLOCK_NONE == block)
                continue;

            Q_ASSERT(block_index < tetris::SHAPE_BLOCKS_COUNT);
            QGraphicsRectItem* gi = Blocks[block_index++];

            //TODO: WTF???
            //gi->setPos({Rect.left() + padding + x * block_size, Rect.top() + padding + y * block_size});
            gi->setPos({left + x * block_size, top + y * block_size});

            gi->setBrush(QBrush{shape.get_block_type()});
            gi->show();
        }
    }
}

void CGSGame::Init()
{
    const QPen no_pen{Qt::PenStyle::NoPen};

    //init blocks
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            QRectF rect{MAIN_WALL_PAD + Rect.left() + BLOCK_SIZE * x,
                        Rect.top() + BLOCK_SIZE * y,
                        BLOCK_SIZE - MAIN_BLOCK_PAD,
                        BLOCK_SIZE - MAIN_BLOCK_PAD};
            QGraphicsRectItem* gi = Parent->addRect(rect, no_pen);
            gi->hide();
            Blocks[x][y] = gi;
            Context[x][y] = tetris::BLOCK_NONE;
        }
    }

    //init text
    QFont text_font{"Lucida Console", 20, QFont::Bold};
    Text = Parent->addSimpleText("", text_font);
    Text->setBrush(QBrush{MAIN_TEXT_COLOR});
    Text->setPen(QPen{Qt::black});
}
void CGSGame::Repaint()
{
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            const tetris::block_t context = TetrisGame.get_block(x, y);
            if(context == Context[x][y])
                continue;

            QGraphicsRectItem* gi = Blocks[x][y];
            Context[x][y] = context;
            if(context == tetris::BLOCK_NONE)
            {
                gi->hide();
                continue;
            }
            QBrush block_brush{QColor{context}, Qt::BrushStyle::SolidPattern};
            gi->setBrush(block_brush);
            gi->show();
        }
    }
}
void CGSGame::setText(const char* text /*= nullptr*/)
{
    Text->setText(text);
    QRectF text_rect = Text->boundingRect();
    Text->setPos(QPointF{(Rect.width() - text_rect.width()) / 2, (Rect.height() - text_rect.height()) / 2});
}

QMainScene::QMainScene(QObject *parent) : QGraphicsScene{parent}
{
}
void QMainScene::Init()
{
    QRectF game_field_rect {MAIN_WALL_PAD, MAIN_WALL_PAD, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT};
    QRectF rect = {0, 0,
                    MAIN_WALL_PAD + game_field_rect.width() + MAIN_WALL_PAD,
                    MAIN_WALL_PAD + game_field_rect.height() + MAIN_WALL_PAD};
    frameGame.InitRect(this, rect, MAIN_BG_COLOR);
    frameGame.Init();

    //shape view
    rect = {frameGame.rect().right() + HORZ_DEL_SIZE, 0, STAT_WIDTH, 100};
    frameShape.InitRect(this, rect, MAIN_BG_COLOR);
    frameShape.Init();

    //score view
    QFont stat_font{"Lucida Console", 8, QFont::Bold};
    rect = QRectF{frameGame.rect().right() + HORZ_DEL_SIZE, frameShape.rect().bottom() + VERT_DEL_SIZE, frameShape.rect().width(), 0};
    frameScore.InitRect(this, rect, MAIN_BG_COLOR);
    frameScore.Init(stat_font, STAT_TEXT_COLOR, STAT_PAD);

    //best results frame
    rect = QRectF{frameScore.rect().left(), frameScore.rect().bottom() + VERT_DEL_SIZE, frameScore.rect().width(), 0};
    frameBest.InitRect(this, rect, MAIN_BG_COLOR);
    frameBest.Init(stat_font, STAT_TEXT_COLOR, STAT_PAD);
    UpdateBest();
    frameBest.UpdateHeight();

    //help frame
    rect = QRectF{frameScore.rect().left(), frameBest.rect().bottom() + VERT_DEL_SIZE, frameScore.rect().width(), 0};
    frameHelp.InitRect(this, rect, MAIN_BG_COLOR);
    frameHelp.Init(stat_font, STAT_TEXT_COLOR, STAT_PAD);
    frameHelp.setText(APP_HELP);
    frameHelp.UpdateHeight();
}
void QMainScene::ResetStat()
{
    frameScore.SetScore(0);
    frameScore.SetSpeed(0);
    frameScore.SetTime(0);
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
    frameBest.setText(text.c_str());
}


