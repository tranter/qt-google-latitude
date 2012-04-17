#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pForm = new Form(this);
    setCentralWidget(m_pForm);

    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(onActionExit()));
    connect(ui->actionShow_Current_Location,SIGNAL(triggered()),this,SLOT(onActionShow()));
    connect(ui->actionHistory_Locations,SIGNAL(triggered()),this,SLOT(onActionHistoryLocation()));
    connect(ui->actionNewLogin,SIGNAL(triggered()),this,SLOT(onNewLogin()));
}

MainWindow::~MainWindow()
{
    delete m_pForm;
    delete ui;
}

void MainWindow::startLoginDefault()
{
    m_pForm->startLogin(false);
}

void MainWindow::onActionExit()
{
    close();
}

void MainWindow::onActionShow()
{
    m_pForm->showCurrentLocation();
}

void MainWindow::onActionHistoryLocation()
{
    m_pForm->getHistoryLocation();
}

void MainWindow::onNewLogin()
{
    m_pForm->startLogin(true);
}
