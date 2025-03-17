#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QPointF>
#include <QTimer>
#include <QElapsedTimer>

#include "random.h"
#include "TetrisEngine.h"
#include "options.h"
#include "MainScene.h"

extern COptions Options;

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

    void generate(tetris::shape_t& shape) override;


    //user events
    void keyPressEvent(QKeyEvent* key_event) override;
    void keyReleaseEvent(QKeyEvent* key_event) override;
    void closeEvent(QCloseEvent *event) override;
    void uePause();
    void ueNewGame();

    void ProcessResult(int engine_result);

    //test
    void OnTest1();
    void OnTest2();

private slots:
    void OnNewGame();
    void OnExit();
    void OnAbout();
    void teTickTimer();
    void teKeyTimer();

private:
    Ui::MainWindow *ui;

    //random value generators
    app::random_value<int, 0, tetris::SHAPE_TYPE_COUNT - 1> RandomShape;
    app::random_value<int, 100, 255> RandomColor;

    //game view
    QMainScene* MainScene;

    //timers
    QTimer* timerTick;
    QElapsedTimer* timerElapsed;

    //key timers
    class QKeyTimer : public QTimer
    {
    public:
        QKeyTimer(QObject *parent = nullptr) : QTimer(parent) {}

        bool Start()
        {
            if(isActive())
                return false;
            start(Options.KeySpeed);
            return true;
        }
    };
    QKeyTimer* timerMoveLeft{nullptr};
    QKeyTimer* timerMoveRight{nullptr};
    QKeyTimer* timerMoveDown{nullptr};

    //game state
    enum : int
    {
        GS_NO_GAME = 0,
        GS_RUNNING,
        GS_PAUSED
    };
    int GameState{GS_NO_GAME};
    bool IsRunning() const {return GS_RUNNING == GameState;}
    bool IsGame() const {return GS_RUNNING == GameState || GS_PAUSED == GameState;}
    void SwitchState(int new_state);
    void StartNewGame();
    void EndGame();
    void Pause(bool val);
    bool QueryEndGame();
};
#endif // MAINWINDOW_H
