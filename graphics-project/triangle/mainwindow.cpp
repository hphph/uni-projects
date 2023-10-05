#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ekran.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Ekran *ekran_input = ui->ekran_input;
    Ekran *ekran_output = ui->ekran_output;
    Ekran::Triangle uvTriangle;
    QImage secImg = QImage(350, 350, QImage::Format_RGB32);
    bool isUvLoaded = false;

    ekran_input->isTexturePointsLoaded = &isUvLoaded;
    ekran_output->isTexturePointsLoaded = &isUvLoaded;

    ekran_input->output = ekran_output;

    ekran_input->secIm = &secImg;
    ekran_output->secIm = &secImg;

    ekran_input->textureTrianglePoints = &uvTriangle;
    ekran_output->textureTrianglePoints = &uvTriangle;

    ekran_input->LoadImage("C:\\Studia\\PGK\\Projects\\Triangle\\img.png");
    ekran_output->LoadSecImage(&ekran_input->im);
}

MainWindow::~MainWindow()
{
    delete ui;
}


