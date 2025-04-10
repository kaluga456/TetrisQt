#include <string>
#include <format>
#include <QtAssert>
#include <QTextStream>
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
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////
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
    constexpr const char* fs_def =  "Time\t  --:--:--";
    constexpr const char* fs =      "Time\t  {:02d}:{:02d}:{:02d}";
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
//////////////////////////////////////////////////////////////////////////////
static void GetShapeRect(const tetris::shape_matrix_t& matrix, QRect& rect)
{
    rect.setLeft(tetris::SHAPE_MATRIX_SIZE - 1);
    rect.setTop(tetris::SHAPE_MATRIX_SIZE - 1);
    rect.setRight(0);
    rect.setBottom(0);
    for(int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
        {
            if(tetris::BLOCK_NONE == matrix[x][y])
                continue;

            if(rect.left() > x)
                rect.setLeft(x);
            if(rect.top() > y)
                rect.setTop(y);
            if(rect.right() < x)
                rect.setRight(x);
            if(rect.bottom() < y)
                rect.setBottom(y);
        }
    }
}
void CGSShape::Init()
{
    const qreal block_size = BLOCK_SIZE;

    const QPen no_pen{Qt::PenStyle::NoPen};
    for(int block_index = 0; block_index < tetris::SHAPE_BLOCKS_COUNT; ++block_index)
    {
        QRectF rect{Rect.left(),
                    Rect.top(),
                    block_size,
                    block_size};
        const int pad = 2;
        rect.adjust(pad, pad, -pad, -pad);

        Blocks[block_index] = Parent->addRect(rect, no_pen);
        Blocks[block_index]->hide();
    }
}
void CGSShape::setShape(const tetris::shape_t &shape)
{
    const qreal padding = 5;
    const qreal block_size = BLOCK_SIZE;

    int shape_width = 0;
    int shape_height = 0;
    int block_index = 0;

    const tetris::shape_matrix_t& matrix = *shape.get_matrix();

    QRect shape_rect;
    GetShapeRect(matrix, shape_rect);
    const qreal left = (Rect.width() - (block_size * shape_rect.width())) / 2 - shape_rect.left() * block_size;
    const qreal top = (Rect.height() - (block_size * shape_rect.height())) / 2;
    for(int x = 0; x < tetris::SHAPE_MATRIX_SIZE; ++x)
    {
        for(int y = 0; y < tetris::SHAPE_MATRIX_SIZE; ++y)
        {
            if(tetris::BLOCK_NONE == matrix[x][y])
                continue;

            Q_ASSERT(block_index < tetris::SHAPE_BLOCKS_COUNT);
            QGraphicsRectItem* gi = Blocks[block_index++];
            gi->setPos({left + x * block_size, top + y * block_size});
            gi->setBrush(QBrush{shape.get_block_type()});
            gi->show();
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
void CGSGame::Init()
{
    constexpr qreal BLOCK_PADDING = 2;
    const QPen no_pen{Qt::PenStyle::NoPen};

    //init blocks
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            QRectF rect{Rect.left() + BLOCK_SIZE * x,
                        Rect.top() + BLOCK_SIZE * y,
                        BLOCK_SIZE,
                        BLOCK_SIZE};
            rect.adjust(BLOCK_PADDING, BLOCK_PADDING, -BLOCK_PADDING, -BLOCK_PADDING);

            QGraphicsRectItem* gi = Parent->addRect(rect, no_pen);
            gi->hide();
            gi->setZValue(2);
            Blocks[x][y] = gi;
            Context[x][y] = tetris::BLOCK_NONE;
        }
    }

    //init text
    QFont text_font{"Trebuchet MS", 24, QFont::Bold};
    Text = Parent->addSimpleText("", text_font);
    Text->setBrush(QBrush{MAIN_TEXT_COLOR});
    Text->setPen(QPen{Qt::black});
    Text->setZValue(4);
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
void CGSGrid::Init(QGraphicsScene *parent, const QRectF &rect, bool show_grid /*= true*/)
{
    Parent = parent;
    ShowGrid = show_grid;

    const qreal cell_size = rect.width() / tetris::GAME_FIELD_WIDTH;
    QPen pen{QColor(80,80,80)};
    for(int y = 0; y < tetris::GAME_FIELD_WIDTH + 1; ++y)
    {
        QLineF line{rect.left() + y * cell_size, rect.top(), y * cell_size, rect.bottom()};
        YLines[y] = Parent->addLine(line, pen);
        if(false == ShowGrid)
            YLines[y]->hide();
        YLines[y]->setZValue(1);
    }
    for(int x = 0; x < tetris::GAME_FIELD_HEIGHT + 1; ++x)
    {
        QLineF line{rect.left(), x * cell_size, rect.right(), x * cell_size};
        YLines[tetris::GAME_FIELD_WIDTH + 1 + x] = Parent->addLine(line, pen);
        if(false == ShowGrid)
            YLines[tetris::GAME_FIELD_WIDTH + 1 + x]->hide();
        YLines[tetris::GAME_FIELD_WIDTH + 1 + x]->setZValue(1);
    }
}
void CGSGrid::Show(bool val)
{
    if(ShowGrid == val)
        return;
    ShowGrid = val;
    for(int y = 0; y < tetris::GAME_FIELD_WIDTH + 1; ++y)
        YLines[y]->setVisible(ShowGrid);
    for(int x = 0; x < tetris::GAME_FIELD_HEIGHT + 1; ++x)
        YLines[tetris::GAME_FIELD_WIDTH + 1 + x]->setVisible(ShowGrid);
}
void CGSGrid::Show()
{
    Show(!ShowGrid);
}
//////////////////////////////////////////////////////////////////////////////
QMainScene::QMainScene(QObject *parent) : QGraphicsScene{parent}
{
}
void QMainScene::Init()
{
    QRectF game_field_rect {0, 0, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT};
    frameGame.InitRect(this, game_field_rect, MAIN_BG_COLOR);
    frameGame.Init();

    Grid.Init(this, frameGame.rect(), Options.ShowGrid);

    //shape view
    QRectF rect = {frameGame.rect().right() + HORZ_DEL_SIZE, 0, STAT_WIDTH, 100};
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
    frameHelp.setHeight(frameGame.rect().bottom() - rect.top());
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
//////////////////////////////////////////////////////////////////////////////
