#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ekran.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Ekran *ekran = ui->ekran;

    ekran->rgbSliders[0] = ui->rSlider;
    ekran->rgbSliders[1] = ui->gSlider;
    ekran->rgbSliders[2] = ui->bSlider;

    ekran->hsvSliders[0] = ui->hSlider;
    ekran->hsvSliders[1] = ui->sSlider;
    ekran->hsvSliders[2] = ui->vSlider;

    ekran->LateUpdate();
}

MainWindow::~MainWindow()
{
    delete ui;
}


