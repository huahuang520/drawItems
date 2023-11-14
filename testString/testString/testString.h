#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_testString.h"
#include <QLabel>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QGraphicsLineItem>
#include "GraphicsView.h"
#include <QDebug>

class testString : public QObject, public QMainWindow
{
    Q_OBJECT

public:
    testString(QWidget *parent = Q_NULLPTR);

    void CreateWidget();

    void ShowCentralCross(double centerWidth, double centerHeight,QGraphicsScene* scene, QGraphicsLineItem* lineRow, QGraphicsLineItem* linecCol, bool isShow = true);
private:
    Ui::testStringClass ui;

private slots:
    void on_button_clicked();
    void on_button1_clicked();
signals:

private:
   // QGraphicsView* view;
    GraphicsView* gview;
    QWidget* test1;
    QVBoxLayout* vboxLayout;
    QPushButton* button;
    QPushButton* button1;

    QGraphicsLineItem* lineRow;  //hang
    QGraphicsLineItem* lineCol;  //lie
    QPen* pen;
    QGraphicsRectItem* rect;
    QGraphicsRectItem* rect1;
    QImage m_img;
    double centerWidth;
    double centerHeight;

};
