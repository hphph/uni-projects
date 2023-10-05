#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Ekran *ekran = ui->Display;
    ekran->slider = ui->horizontalSlider;
    ekran->buttonBezierPlus = ui->plusBezier;
    ekran->buttonBezierMinus = ui->minusBezier;
    ekran->buttonBSplineMinus = ui->minsBSpline;
    ekran->buttonBSplinePlus = ui->plusBSpline;
    ekran->buttonFloodFill = ui->floodFill;
    ekran->buttonScanLinePlus = ui->plusScanLine;
    ekran->LateUpdate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

