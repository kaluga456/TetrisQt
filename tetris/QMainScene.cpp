#include "QMainScene.h"
#include "TetrisEngine.h"

extern tetris::engine TetrisGame;

QGameFieldScene::QGameFieldScene(QObject *parent) : QGraphicsScene{parent}, IsGameOver{false}
{
}

void QGameFieldScene::Init(const QRectF& ParentRect)
{
    //set background
    QColor brush_color{32, 32, 32};
    QBrush brush{brush_color, Qt::BrushStyle::SolidPattern};
    setBackgroundBrush(brush);

    //block attr
    BlockSize = ParentRect.width() / tetris::GAME_FIELD_WIDTH;
    QPen block_pen{Qt::PenStyle::NoPen};

    //init blocks
    for(int x = 0; x < tetris::GAME_FIELD_WIDTH; ++x)
    {
        for(int y = 0; y < tetris::GAME_FIELD_HEIGHT; ++y)
        {
            //TEST:
            QRectF rect{BlockSize * x,  BlockSize * y, BlockSize - 5, BlockSize - 5};

            //QRectF rect{BlockSize * x,  BlockSize * y, BlockSize, BlockSize};
            QGraphicsRectItem* gi = addRect(rect, block_pen);
            gi->hide();
            gi->setEnabled(false);
            Blocks[x][y] = gi;
            Context[x][y] = tetris::BLOCK_NONE;

            //TEST:
            //gi->setRect();
        }
    }

    //TODO:
    //init text
    QFont text_font{"Lucida Console", 32, QFont::Bold};
    Text = addSimpleText("", text_font);
    Text->setBrush(QBrush{QColor{200,200,200}});

    //TODO:
    Text->setPen(QPen{QColor{0,0,0}});
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
    if(val == IsGameOver)
        return;
    IsGameOver = val;

    //TODO:
    RePaint();
}

void QGameFieldScene::DrawPause(bool val)
{
    if(val == IsPause)
        return;

    //TODO:
    RePaint();
}

void QGameFieldScene::SetText(const QString& text)
{
    Text->setText(text);

    //TODO: correct center
    //center text
    const QRectF scene_rect = sceneRect();
    QRectF text_rect = Text->boundingRect(); //item coords
    //Text->mapRectToScene(text_rect);

    //myView.mapToScene(myView.viewport()->rect().center())

    // Sets the position of the item to pos, which is in parent coordinates. For items with no parent, pos is in scene coordinates.
    // The position of the item describes its origin (local coordinate (0, 0)) in parent coordinates
    Text->setPos(QPointF{(scene_rect.width() - text_rect.width()) / 2, (scene_rect.height() - text_rect.height()) / 2});
}
