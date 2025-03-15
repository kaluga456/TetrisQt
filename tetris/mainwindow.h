#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QPointF>
#include <QTimer>

#include "random.h"
#include "TetrisEngine.h"
#include "QMainScene.h"

//game state
enum : int
{
    GS_NO_GAME = 0,
    GS_RUNNING,
    GS_PAUSED
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public tetris::shape_generator_t
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //callbacks
    void generate(tetris::shape_t& shape) override;

    //user events
    void keyPressEvent(QKeyEvent* key_event) override;
    void uePause();

    //user events for game field
    void ueNewGame();
    void ProcessResult(int engine_result);

    //test
    void OnTest1();
    void OnTest2();

    //timer events

public slots:
    void teTickTimer();

private:
    Ui::MainWindow *ui;

    //random
    app::random_value<int, 0, tetris::SHAPE_TYPE_COUNT - 1> RandomShape;
    app::random_value<int, 100, 255> RandomColor;

    //game logic
    QGameFieldScene* GameFieldScene;

    //timers
    QTimer* timerTick;

    //game state
    int GameState{GS_NO_GAME};
    bool IsRunning() const {return GS_RUNNING == GameState;}
    bool IsGame() const {return GS_RUNNING == GameState || GS_PAUSED == GameState;}
    void SwitchState(int new_state);

    //
    void StartNewGame();
    void EndGame();
    void Pause(bool val);
    bool QueryEndGame() const;
};
#endif // MAINWINDOW_H
