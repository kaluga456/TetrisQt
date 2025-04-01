#include <QtDebug>
#include <QColor>
#include <QSettings>
#include "about.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

extern MainWindow* MainWnd;
tetris::engine TetrisGame;

constexpr int MAIN_PADDING = 5;
constexpr const char* PAUSED_TEXT = "PAUSED";
constexpr const char* GAME_OVER_TEXT = "GAME OVER";

//debug flags
#ifdef _DEBUG
//#define DEBUG_NO_TICKS
//#define DEBUG_SHAPE
#ifdef DEBUG_SHAPE
class debug_shape_generator : public tetris::shape_generator_t
{
public:
    void generate(tetris::shape_t& shape) {shape.reset(tetris::SHAPE_TYPE_I, 0x808080);}
};
static debug_shape_generator debug_generator;
#endif //DEBUG_SHAPE
#endif //_DEBUG

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , GameState{GS_NO_GAME}
{
    Options.Load();

    ui->setupUi(this);

    //main window
    ui->statusbar->setVisible(false);
    setStatusBar(nullptr);
    setWindowIcon(QIcon(":/res/AppIcon.ico"));
    setWindowTitle(APP_FULL_NAME);
    const Qt::WindowFlags wnd_flags = windowFlags();
    setWindowFlags(wnd_flags | Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    //ui events
    connect(ui->actionNewGame, &QAction::triggered, this, &MainWindow::OnNewGame);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::OnExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::OnAbout);

    //init logic
#ifdef DEBUG_SHAPE
    TetrisGame.set_generator(&debug_generator);
#else
    TetrisGame.set_generator(this);
#endif

    //init controls
    ui->GameFieldView->setEnabled(false);

    //QRectF view_rect;
    QRectF scene_rect;

    //init main scene
    MainScene = new QMainScene(this);
    ui->GameFieldView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->GameFieldView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->GameFieldView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->GameFieldView->setScene(MainScene);
    MainScene->Init();
    MainScene->ResetStat();
    MainScene->UpdateBest();

    //main scene pos
    scene_rect = ui->GameFieldView->sceneRect();
    ui->GameFieldView->move(MAIN_PADDING, MAIN_PADDING);
    ui->GameFieldView->resize(static_cast<int>(scene_rect.width()), static_cast<int>(scene_rect.height()));

    //TODO: how to calculate client rect???
    //set main window geometry
    QRect rect_client = centralWidget()->geometry();
    //const int frame_diff_x = rect_frame.width() - rect_client.width();
    const int frame_diff_y = /*rect_frame.height() - */rect_client.height();
    move(Options.LayoutLeft, Options.LayoutTop);
    resize(
        MAIN_PADDING + ui->GameFieldView->frameRect().right() + MAIN_PADDING,
        frame_diff_y + MAIN_PADDING + ui->GameFieldView->frameRect().height() + MAIN_PADDING
        );

    //init timers
    timerTick = new QTimer(this);
    timerClock = new QTimer(this);
    connect(timerTick, &QTimer::timeout, this, &MainWindow::teTickTimer);
    connect(timerClock, &QTimer::timeout, this, &MainWindow::teClockTimer);
    timerClock->start(1000);

    timerMoveLeft = new QKeyTimer(this);
    timerMoveRight = new QKeyTimer(this);
    timerMoveDown = new QKeyTimer(this);
    connect(timerMoveLeft, &QTimer::timeout, this, &MainWindow::teKeyTimer);
    connect(timerMoveRight, &QTimer::timeout, this, &MainWindow::teKeyTimer);
    connect(timerMoveDown, &QTimer::timeout, this, &MainWindow::teKeyTimer);
}
MainWindow::~MainWindow()
{
    delete timerClock;
    delete timerTick;

    delete timerMoveDown;
    delete timerMoveRight;
    delete timerMoveLeft;

    delete MainScene;
    delete ui;
}
void MainWindow::generate(tetris::shape_t& shape)
{
    const QRgb color{qRgb(RandomColor(),RandomColor(),RandomColor())};
    shape.reset(RandomShape(), color);
}
void MainWindow::keyPressEvent(QKeyEvent *key_event)
{
    //TODO: how to use this?
    //const bool is_repeat = key_event->isAutoRepeat();

    switch(key_event->key())
    {
    case Qt::Key::Key_Return:
        ueNewGame();
        break;
    case Qt::Key::Key_Escape:
        uePause();
        break;

    //shape rotation
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        if(IsRunning())
            ProcessResult(TetrisGame.rotate_left());
        break;
    case Qt::Key::Key_Shift:
        if(IsRunning())
            ProcessResult(TetrisGame.rotate_right());
        break;

    //shape movement
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        if(false == IsRunning())
            break;
        if(timerMoveLeft->Start())
            ProcessResult(TetrisGame.move_left());
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        if(false == IsRunning())
            break;
        if(timerMoveRight->Start())
            ProcessResult(TetrisGame.move_right());
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        if(false == IsRunning())
            break;
        if(timerMoveDown->Start())
            ProcessResult(TetrisGame.move_down());
        break;

    case Qt::Key::Key_Space:
        if(IsRunning())
            ProcessResult(TetrisGame.drop());
        break;

    //show/hide grid
    case Qt::Key_G:
        MainScene->Grid.Show();
        break;

    //TEST:
    case Qt::Key::Key_1:
        OnTest1();
        break;
    case Qt::Key::Key_2:
        OnTest2();
        break;
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *key_event)
{
    switch(key_event->key())
    {
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        timerMoveLeft->stop();
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        timerMoveRight->stop();
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        timerMoveDown->stop();
        break;
    }
}
void MainWindow::OnTest1()
{
}
void MainWindow::OnTest2()
{
}
void MainWindow::OnNewGame()
{
    SwitchState(GS_RUNNING);
}
void MainWindow::OnAbout()
{
    QMessageBox mb;
    mb.setIcon(QMessageBox::Information);
    mb.setWindowTitle(APP_NAME);
    mb.setText(APP_FULL_NAME "\n" APP_URL "\n\n" APP_HELP);
    mb.setStandardButtons(QMessageBox::Ok);
    mb.exec();
}
void MainWindow::ueNewGame()
{
    if(IsGame())
    {
        if(false == QueryEndGame())
            return;
        EndGame();
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

void MainWindow::teClockTimer()
{
    if(GS_RUNNING == GameState)
        MainScene->frameScore.SetTime(TimeCounter.time());
}
void MainWindow::teKeyTimer()
{
    if(false == IsRunning())
        return;
    int result{tetris::RESULT_NONE};
    if(timerMoveLeft->isActive())
        result = TetrisGame.move_left();
    if(timerMoveRight->isActive())
        result = TetrisGame.move_right();
    if(timerMoveDown->isActive())
        result = TetrisGame.move_down();
    ProcessResult(result);
}
void MainWindow::ProcessResult(int engine_result)
{
    switch(engine_result)
    {
    case tetris::RESULT_NONE:
        return;
    case tetris::RESULT_SHAPE:
        MainScene->frameShape.setShape(TetrisGame.get_next_shape());
        MainScene->frameScore.SetScore(TetrisGame.get_score());
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
    MainScene->frameGame.Repaint();
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
        switch(new_state)
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
        switch(new_state)
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
    default:
        Q_ASSERT(0);
    }

    GameState = new_state;
}

void MainWindow::StartNewGame()
{
    TetrisGame.new_game();
    TimeCounter.Start();

    MainScene->frameShape.setShape(TetrisGame.get_next_shape());

    MainScene->frameScore.SetScore(TetrisGame.get_score());
    MainScene->frameScore.SetSpeed(0);
    MainScene->frameScore.SetTime(TimeCounter.time());

    MainScene->frameGame.setText();
    MainScene->frameGame.Repaint();

#ifndef DEBUG_NO_TICKS
    timerTick->start(1000);
#endif
}
void MainWindow::EndGame()
{
    MainScene->frameGame.setText(GAME_OVER_TEXT);
    timerTick->stop();

    const int result = TetrisGame.get_score();
    MainScene->frameScore.SetScore(result);
    if(Options.BestResults.add(result))
        MainScene->UpdateBest();
}
void MainWindow::Pause(bool val)
{
    if(false == IsGame())
        return;

    if(GS_RUNNING == GameState)
    {
        if(false == val)
            return;
        GameState = GS_PAUSED;
        MainScene->frameGame.setText(PAUSED_TEXT);
        TimeCounter.Pause();
    }
    else if(GS_PAUSED == GameState)
    {
        if(true == val)
            return;
        GameState = GS_RUNNING;
        MainScene->frameGame.setText();
        TimeCounter.Pause(false);
    }
}
void MainWindow::OnExit()
{
    closeEvent(nullptr);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(IsGame() && false == QueryEndGame())
    {
        if(event) event->ignore();
        return;
    }

    QPoint p = pos();
    Options.LayoutLeft = p.x();
    Options.LayoutTop = p.y();
    Options.ShowGrid = MainScene->Grid.isVisible();
    Options.BestResults.add(TetrisGame.get_score());
    Options.Save();

    if(event)
        QWidget::closeEvent(event);
    else
        QApplication::exit(0);
}
bool MainWindow::QueryEndGame()
{
    if(false == IsGame())
         return true;

    const bool running = (GS_RUNNING == GameState);
    if(running)
        Pause(true);

    QMessageBox mb;
    mb.setIcon(QMessageBox::Warning);
    mb.setWindowTitle(APP_NAME);
    mb.setText("End current game?");
    mb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Ok);
    const bool result = QMessageBox::Ok == mb.exec();

    if(running)
        Pause(false);

    return result;
}

