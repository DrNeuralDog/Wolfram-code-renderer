#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer;
    thread = new QThread;
    connector = new Connector;
    disconnector = new Disconnector;
    imageGenerator = new ImageCellGenerator;

    target = new QRectF(10.0, 20.0, 700.0, 570.0);
    source = new QRectF(0.0, 0.0, 150.0, 150.0);

    connector->timers_connects(timer, imageGenerator);
    connector->imageGenerator_connects(this, imageGenerator);

    settingsThread();
    installWindowStartConfig();

    setWindowTitle("Wolfram code renderer");
}
MainWindow::~MainWindow()
{
    disconnector->timers_disconnects(timer, imageGenerator);
    disconnector->imageGenerator_disconnects(this, imageGenerator);

    delete imageGenerator;
    delete ui;
}
void MainWindow::settingsThread()
{
    imageGenerator->moveToThread(thread);
    thread->start();
}
void MainWindow::installWindowStartConfig()
{
    for(int i = 0; i < imageGenerator->get_currentImage()->height(); i++)
        for(int c = 0; c < imageGenerator->get_currentImage()->width(); c++)
            imageGenerator->get_currentImage()->setPixel(c, i, 0);

    repaint();
}

void MainWindow::on_pushButton_StartPainting_clicked()
{
    emit startCalculateCells();
    ui->pushButton_StartPainting->setCheckable(false);
    ui->pushButton_StartPainting->setCheckable(false);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    painter.drawImage(*target, *imageGenerator->get_currentImage(), *source);
}

void MainWindow::callRepraint()
{
    repaint();
}


ImageCellGenerator::ImageCellGenerator()
{
    calculateStartImageState();
    installImageConfig();
}
ImageCellGenerator::~ImageCellGenerator()
{

}

void ImageCellGenerator::calculateStartImageState()
{
    renderingPickture_buffer.resize(sizeSidesPicture);
    cellPickture.resize(sizeSidesPicture);
    renderingPickture_buffer.fill(false);
    cellPickture.fill(false);

    cellPickture[sizeSidesPicture - 1] = true;
}
void ImageCellGenerator::installImageConfig()
{
    image = new QImage(sizeSidesPicture, sizeSidesPicture, QImage::Format_Indexed8);

    value = qRgb(120, 140, 250);
    image->setColor(0, value);

    value = qRgb(80, 250, 51);
    image->setColor(1, value);
}

void ImageCellGenerator::consolePaint()
{
    for(int c = 0; c < sizeSidesPicture; c++)
    {
        size_t pixel = (((cellPickture[c]) == false) ? 0 : 1);
        image->setPixel(c, numberString, pixel);
    }

    emit callRep();
}

void ImageCellGenerator::renderingStringImage()
{
    bool *checkPixelsBuffeer = new bool[3];

    for(int i = 0; i < sizeSidesPicture; i++)
    {
        size_t firstVal = ((i == 0) ? (sizeSidesPicture - 1) : (i - 1));
        size_t currentVal = i;
        size_t thridVal = (i == (sizeSidesPicture - 1)) ? 0 : (i + 1);

        checkPixelsBuffeer[0] = cellPickture[firstVal];
        checkPixelsBuffeer[1] = cellPickture[currentVal];
        checkPixelsBuffeer[2] = cellPickture[thridVal];

        renderingPickture_buffer[i] = currentRull->calculateCelluarPixel(checkPixelsBuffeer);
    }

    for(int i = 0; i < sizeSidesPicture; i++) cellPickture[i] = renderingPickture_buffer[i];
}
void ImageCellGenerator::paintCellRulles()
{
    rules.append(new Rule110);
    rules.append(new Rule30);
    rules.append(new Rule225);
    rules.append(new Rule184);
    rules.append(new Rule120);
    currentRull = rules.at(printRulesCounter);

    for(int i = 0; i != -1; i++)
    {
        renderingStringImage();
        consolePaint();
        numberString++;
        Sleep(70);

        if(numberString == sizeSidesPicture)
        {
            numberString = 0;
            printRulesCounter++;

            if(printRulesCounter == numRules)
                printRulesCounter = 0;
            currentRull = rules.at(printRulesCounter);
        }
    }
}

QImage *ImageCellGenerator::get_currentImage()
{
    return image;
}


void Connector::timers_connects(QTimer *currentTimer, ImageCellGenerator *currentImgGen)
{
    connect(currentTimer, SIGNAL(timeout()), currentImgGen, SLOT(paintCellRulles()));
}
void Connector::imageGenerator_connects(MainWindow *currentUI, ImageCellGenerator *currentImgGen)
{
    connect(currentUI, SIGNAL(startCalculateCells()), currentImgGen, SLOT(paintCellRulles()));
    connect(currentImgGen, SIGNAL(callRep()), currentUI, SLOT(callRepraint()));
}

void Disconnector::timers_disconnects(QTimer *currentTimer, ImageCellGenerator *currentImgGen)
{
    disconnect(currentTimer, SIGNAL(timeout()), currentImgGen, SLOT(paintCellRulles()));
}
void Disconnector::imageGenerator_disconnects(MainWindow *currentUI, ImageCellGenerator *currentImgGen)
{
    disconnect(currentUI, SIGNAL(startCalculateCells()), currentImgGen, SLOT(paintCellRulles()));
    disconnect(currentImgGen, SIGNAL(callRep()), currentUI, SLOT(callRepraint()));
}