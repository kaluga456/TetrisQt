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

class QMainScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QMainScene(QObject *parent = nullptr);

    void Init();

    void RePaint();
    void DrawGameOver(bool val);
    void DrawPause(bool val);
    void SetText(const QString& text);

    //TODO: show next shape
    void SetShape(int val);

    //stat text
    void SetScore(int val);
    void SetSpeed(int val);
    void SetTime(int val);
    void ResetStat();

    void UpdateBest();

private:
    QGraphicsRectItem* Blocks[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];
    tetris::block_t Context[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];

    //frames
    QRectF GameFieldFrame;
    QRectF StatFrame;
    QRectF BestFrame;
    QRectF HelpFrame;

    //
    QGraphicsRectItem* giBackground{nullptr};
    QGraphicsRectItem* giVertDelimiter{nullptr};
    QGraphicsRectItem* giHorzDelimiter1{nullptr};
    QGraphicsRectItem* giHorzDelimiter2{nullptr};

    //main text
    QGraphicsSimpleTextItem* Text{nullptr};

    //
    QGraphicsSimpleTextItem* textScore{nullptr};
    QGraphicsSimpleTextItem* textSpeed{nullptr};
    QGraphicsSimpleTextItem* textTime{nullptr};
    QGraphicsSimpleTextItem* textBest{nullptr};
    QGraphicsSimpleTextItem* textHelp{nullptr};
};

#endif // MAINSCENE_H
