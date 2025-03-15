#ifndef QMAINSCENE_H
#define QMAINSCENE_H

#include <QObject>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include "TetrisEngine.h"

constexpr int BLOCK_VIEW_SIZE = 28; //CGameFieldView: 336 x 672 for block size = 28
constexpr int GAME_FIELD_VIEW_WIDTH = (BLOCK_VIEW_SIZE * tetris::GAME_FIELD_WIDTH);
constexpr int GAME_FIELD_VIEW_HEIGHT = (BLOCK_VIEW_SIZE * tetris::GAME_FIELD_HEIGHT);

class QGameFieldScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QGameFieldScene(QObject *parent = nullptr);

    void Init(const QRectF& ParentRect);

    //TODO:
    void RePaint();
    void DrawGameOver(bool val);
    void DrawPause(bool val);
    void SetText(const QString& text);

private:
    qreal BlockSize{0};
    QGraphicsRectItem* Blocks[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];
    tetris::block_t Context[tetris::GAME_FIELD_WIDTH][tetris::GAME_FIELD_HEIGHT];

    //text
    bool IsGameOver{false}; //if true draw game over info
    bool IsPause{false}; //if true draw game over info
    QGraphicsSimpleTextItem* Text;
};

#endif // QMAINSCENE_H
