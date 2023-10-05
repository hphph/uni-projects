#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    img1 = QImage("C:\\Studia\\PGK\\Projects\\blendmodes\\img1.png");
    img2 = QImage("C:\\Studia\\PGK\\Projects\\blendmodes\\img2.png");
    img3 = QImage("C:\\Studia\\PGK\\Projects\\blendmodes\\img3.png");
    img1 = img1.scaled(250, 250);
    img2 = img2.scaled(250, 250);
    img3 = img3.scaled(250, 250);

    ui->widget->img[0] = img1;
    ui->widget->img[1] = img2;
    ui->widget->img[2] = img3;
    ui->widget->layer[0] = img1;
    ui->widget->layer[1] = img2;
    ui->widget->layer[2] = img3;

    ui->widget->alpha[0] = ui->horizontalSlider;
    ui->widget->alpha[1] = ui->horizontalSlider_2;
    ui->widget->alpha[2] = ui->horizontalSlider_3;
}

MainWindow::~MainWindow()
{
    delete ui;
}

