#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ekran = ui->display;
    ekran->SetButton(ui->drawButton, Ekran::DRAW);
    ekran->SetButton(ui->dilationButton, Ekran::DILATION);
    ekran->SetButton(ui->erosionButton, Ekran::EROSION);
}

MainWindow::~MainWindow()
{
    delete ui;
}

