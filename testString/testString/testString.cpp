#include "testString.h"
#include <QDebug>

using namespace std;
testString::testString(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
   
    CreateWidget();

    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_button_clicked()));
    QObject::connect(button1, SIGNAL(clicked()), this, SLOT(on_button1_clicked()));
}

void testString::CreateWidget()
{
    gview = new GraphicsView(this);
    test1 = new QWidget(this);
    vboxLayout = new QVBoxLayout();
    
    lineRow = new QGraphicsLineItem();
    lineCol = new QGraphicsLineItem();
    pen = new QPen();
    button = new QPushButton(this);
    button1 = new QPushButton(this);
    vboxLayout->addWidget(gview);
    vboxLayout->addWidget(button);
    vboxLayout->addWidget(button1);
    test1->setLayout(vboxLayout);
    QTabWidget* tab1 = new QTabWidget(ui.centralWidget);
    tab1->addTab(test1, "test1");
    this->setCentralWidget(tab1);
    
}

void testString::ShowCentralCross(double centerWidth, double centerHeight,QGraphicsScene* scene, QGraphicsLineItem*lineRow, QGraphicsLineItem* linecCol, bool isShow)
{
    if (isShow)
    {
       
       
      /*  pen->setColor(QColor(255, 0, 255));
        pen->setWidth(2);
        
       
        lineRow->setLine(-50, 0, 50, 0);
        linecCol->setLine(0, 50, 0, -50);

        lineRow->setPen(*pen);
        linecCol->setPen(*pen);

        scene->addItem(lineRow);
        scene->addItem(linecCol);*/

        //QPainter *painter = new QPainter(&m_img);
        
        pen->setWidth(2);
        pen->setColor(Qt::blue);
        /*painter->setPen(*pen);
        painter->drawLine(-50, 0, 50, 0);*/
        scene->addLine(0,0, 50, 50, *pen);
    }
}


void testString::on_button_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, "open", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), "image (*.bmp *.png *.jpg)");

    if (fileName.isEmpty())
    {
        return;
    }

    QImage image(fileName);
    
    gview->setBackImage(image);
}

void testString::on_button1_clicked()
{
    
    gview->drawPolygon_();
}

