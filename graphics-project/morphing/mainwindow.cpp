#include "mainwindow.h"
#include "ui_mainwindow.h"

QVector2D inputTriangle[3] = {QVector2D(150, 25), QVector2D(50, 200), QVector2D(200, 200)};
QVector2D outputTriangle[3] = {QVector2D(50, 60), QVector2D(50, 150), QVector2D(200, 200)};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imgL.load("C:\\Studia\\PGK\\Projects\\Morphing\\img.png");
    imgF.load("C:\\Studia\\PGK\\Projects\\Morphing\\img2.png");
    imgF = imgF.scaled(300,300);

    OutputScreen *s1 = ui->widget;
    OutputScreen *s2 = ui->widget_2;
    o = ui->widget_3;
    s1->DrawTriangle(inputTriangle, inputTriangle, imgL);
    s2->DrawTriangle(outputTriangle, inputTriangle, imgF);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartAnimation()
{
    o->Morph(inputTriangle, imgL, outputTriangle, imgF);
}

