#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QObject>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "TetrisEngine.h"

constexpr int BLOCK_VIEW_SIZE = 30;
constexpr int GAME_FIELD_VIEW_WIDTH = (BLOCK_VIEW_SIZE * tetris::GAME_FIELD_WIDTH);
constexpr int GAME_FIELD_VIEW_HEIGHT = (BLOCK_VIEW_SIZE * tetris::GAME_FIELD_HEIGHT);
//////////////////////////////////////////////////////////////////////////////
//rectangle area in QGraphicsScene
class CGSRect
{
public:
    CGSRect() : Parent{nullptr} {}

    void InitRect(QGraphicsScene *parent, const QRectF& rect, QColor bg_color);
    const QRectF& rect() const {return Rect;}

    void setWidth(qreal width);
    void setHeight(qreal height);

protected:
    QRectF Rect;
    QGraphicsScene* Parent{nullptr};
    QGraphicsRectItem* Background{nullptr};
};
//////////////////////////////////////////////////////////////////////////////
class CGSText : public CGSRect
{
public:
    CGSText() : CGSRect() {}

    void Init(const QFont& font, QColor color, qreal padding);
    void setText(const char* text = nullptr);

    //set Rect according to Text rect
    void UpdateWidth();
    void UpdateHeight();

protected:
    QGraphicsSimpleTextItem* Text{nullptr};
    qreal Padding{0};
};
//////////////////////////////////////////////////////////////////////////////
//current game stats
class CGSScore : public CGSRect
{
public:
    CGSScore() : CGSRect() {}

    void Init(const QFont& font, QColor color, qreal padding);
    void SetScore(int val);
    void SetSpeed(int val);
    void SetTime(int val);

private:
    QGraphicsSimpleTextItem* textScore{nullptr};
    QGraphicsSimpleTextItem* textSpeed{nullptr};
    QGraphicsSimpleTextItem* textTime{nullptr};
};
//////////////////////////////////////////////////////////////////////////////
//next shape view
class CGSShape : public CGSRect
{
public:
    CGSShape() : CGSRect() {}

    void Init();
    void setShape(const tetris::shape_t& shape);

private:
    QGraphicsRectItem* Blocks[tetris::SHAPE_BLOCKS_COUNT];
    QGraphicsRectItem* Matrix[tetris::SHAPE_MATRIX_SIZE][tetris::SHAPE_MATRIX_SIZE];
};
//////////////////////////////////////////////////////////////////////////////
//game field
class CGSGame : public CGSRect
{
public:
    CGSGame() : CGSRect() {}

    void Init();
    void Repaint();
    void setText(const char* text = nullptr);

private:
    QGraphicsRectItem* Blocks[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];
    tetris::block_t Context[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];
    QGraphicsSimpleTextItem* Text{nullptr};
};
//////////////////////////////////////////////////////////////////////////////
class CGSGrid
{
public:
    CGSGrid() {}

    void Init(QGraphicsScene* parent, const QRectF& rect, bool show_grid = true);

    void Show(bool val);
    void Show(); //switch

    bool isVisible() const {return ShowGrid;}

private:
    bool ShowGrid{true};
    QGraphicsScene* Parent{nullptr};
    QGraphicsLineItem* YLines[tetris::GAME_FIELD_HEIGHT + tetris::GAME_FIELD_WIDTH + 2];
};
//////////////////////////////////////////////////////////////////////////////
class QMainScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QMainScene(QObject *parent = nullptr);

    void Init();

    //stat text
    void SetShape(int val);
    void UpdateBest();
    void ResetStat();

    //frames
    CGSGame frameGame;
    CGSShape frameShape;
    CGSScore frameScore;
    CGSText frameBest;
    CGSText frameHelp;

    CGSGrid Grid;

private:
};
//////////////////////////////////////////////////////////////////////////////
#endif // MAINSCENE_H
