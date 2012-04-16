#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class Form;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startLoginDefault();

private slots:
    void onActionExit();
    void onActionShow();
    void onActionHistoryLocation();

private:
    Ui::MainWindow *ui;

    Form* m_pForm;
};

#endif // MAINWINDOW_H
