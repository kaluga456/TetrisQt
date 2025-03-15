#include <QtDebug>
#include <QColor>
#include "about.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern MainWindow* MainWnd;
tetris::engine TetrisGame;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , GameState{GS_NO_GAME}
{
    ui->setupUi(this);

    setWindowTitle(APP_FULL_NAME);

    //ui events
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::OnAbout);

    //init logic
    TetrisGame.set_generator(this);

    //init controls
    ui->GameFieldView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->GameFieldView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    //init scene
    GameFieldScene = new QGameFieldScene(this);
    ui->GameFieldView->setScene(GameFieldScene);

    QRectF scene_rect = ui->GameFieldView->frameRect();
    GameFieldScene->Init(scene_rect);

    //init timers
    timerTick = new QTimer(this);
    connect(timerTick, &QTimer::timeout, this, &MainWindow::teTickTimer);
}

MainWindow::~MainWindow()
{
    delete timerTick;
    delete GameFieldScene;
    delete ui;
}
void MainWindow::generate(tetris::shape_t& shape)
{
    const QRgb color{qRgb(RandomColor(),RandomColor(),RandomColor())};

    //TEST:
    //shape.reset(tetris::SHAPE_TYPE_T, color);
    shape.reset(RandomShape(), color);
}
void MainWindow::keyPressEvent(QKeyEvent *key_event)
{
    //const bool is_repeat = key_event->isAutoRepeat();

    switch(key_event->key())
    {
    case Qt::Key::Key_Return:
        ueNewGame();
        break;
    case Qt::Key::Key_Escape:
        uePause();
        break;

    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        if(IsRunning())
            ProcessResult(TetrisGame.rotate_left());
        break;

    case Qt::Key::Key_Shift:
        if(IsRunning())
            ProcessResult(TetrisGame.rotate_right());
        break;

    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        if(IsRunning())
            ProcessResult(TetrisGame.move_left());
        break;

    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        if(IsRunning())
            ProcessResult(TetrisGame.move_right());
        break;

    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        if(IsRunning())
            ProcessResult(TetrisGame.move_down());
        break;

    case Qt::Key::Key_Space:
        if(IsRunning())
            ProcessResult(TetrisGame.drop());
        break;

    case Qt::Key::Key_1:
        OnTest1();
        break;
    case Qt::Key::Key_2:
        OnTest2();
        break;
    }
}

void MainWindow::OnTest1()
{
    //QString format{"%3 %1 | %2 %1\n"};
    //QString str = format.arg(pos.x(), pos.y(), pos.rx(), pos.ry());

    //const qreal block_size = scene_rect.width() / GAME_FIELD_WIDTH;
    //ui->GameFieldView->setSceneRect(scene_rect);
    //qDebug() << scene_rect.x() << ":" << scene_rect.y() << ":" <<  scene_rect.width() << ":" <<  scene_rect.height();

    TetrisGame.new_game();
}
void MainWindow::OnTest2()
{
    const int result = TetrisGame.move_down();
    if(result != tetris::RESULT_NONE)
        GameFieldScene->RePaint();
}

void MainWindow::OnAbout()
{
    QMessageBox mb;
    mb.setIcon(QMessageBox::Information);
    mb.setWindowTitle(APP_NAME);

    mb.setText(APP_FULL_NAME
        "\n\n"
        "Controls:\n"
        "Move shape:\tASD or arrow keys\n"
        "Rotate shape:\tW or UP\n"
        "Drop shape:\tSPACE\n");

    mb.setStandardButtons(QMessageBox::Ok);
    mb.exec();
}

void MainWindow::ueNewGame()
{
    if(IsGame())
    {
        if(false == QueryEndGame())
            return;
        StartNewGame();
    }
    SwitchState(GS_RUNNING);
}

void MainWindow::uePause()
{
    if(GS_RUNNING == GameState)
        SwitchState(GS_PAUSED);
    else if(GS_PAUSED == GameState)
        SwitchState(GS_RUNNING);
}

void MainWindow::teTickTimer()
{
    if(false == IsRunning())
        return;
    ProcessResult(TetrisGame.move_down());
}
void MainWindow::ProcessResult(int engine_result)
{
    //TODO:
    switch(engine_result)
    {
    case tetris::RESULT_NONE:
        return;
    case tetris::RESULT_SHAPE:
        break;
    case tetris::RESULT_CHANGED:
        break;
    case tetris::RESULT_GAME_OVER:
        SwitchState(GS_NO_GAME);
        break;
    default:
        Q_ASSERT(0);
        return;
    }
    GameFieldScene->RePaint();
}
void MainWindow::SwitchState(int new_state)
{
    if(new_state == GameState)
        return;

    switch(GameState)
    {
    case GS_NO_GAME:
    {
        switch(new_state)
        {
        case GS_RUNNING:
            StartNewGame();
            break;
        case GS_PAUSED:
            break;
        }
        break;
    }
    case GS_RUNNING:
    {
        switch(GameState)
        {
        case GS_NO_GAME:
            EndGame();
            break;
        case GS_PAUSED:
            Pause(true);
            break;
        }
        break;
    }
    case GS_PAUSED:
    {
        switch(GameState)
        {
        case GS_NO_GAME:
            EndGame();
            break;
        case GS_RUNNING:
            Pause(false);
            break;
        }
        break;
    }
    }

    GameState = new_state;
}

void MainWindow::StartNewGame()
{
    GameFieldScene->SetText("");
    TetrisGame.new_game();
    GameFieldScene->RePaint();
    timerTick->start(1000);
}
void MainWindow::EndGame()
{
    GameFieldScene->SetText("GAME OVER");
    timerTick->stop();
}
void MainWindow::Pause(bool val)
{
    if(false == IsGame())
        return;

    if(GS_RUNNING == GameState)
    {
        if(false == val)
            return;
        GameFieldScene->SetText("PAUSED");
    }
    else if(GS_PAUSED == GameState)
    {
        if(true == val)
            return;
        GameFieldScene->SetText("");
    }
}

bool MainWindow::QueryEndGame() const
{
    if(false == IsGame())
        return false;

    QMessageBox mb;
    mb.setIcon(QMessageBox::Warning);
    mb.setWindowTitle(APP_NAME);
    mb.setText("End current game?");
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Ok);
    return QMessageBox::Ok == mb.exec();
}

