#pragma once

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QScrollBar>

class GraphicsViewTool : public QObject
{
	Q_OBJECT

public:
	GraphicsViewTool(QObject* parent = nullptr);
	GraphicsViewTool(QGraphicsView* graphicsView, QGraphicsScene* scene, QGraphicsPixmapItem* pixmapItem, QObject* parent = nullptr);
	~GraphicsViewTool();

public:
	void SetPixMiddle(const QImage& image); //放在resizeEvent， 界面变化保证图像在界面中央   ------必须
	QPixmap& GetCurrentPix(); //获取最新的QPixmap

	void ScaleImage(QWheelEvent* event); //放在wheelEvent， 缩放图像
	double GetScale(); //获取放大倍数
private:
	QGraphicsView* m_graphicsView;
	QGraphicsScene* m_scene;
	QGraphicsPixmapItem* m_pixmapItem;
	QPixmap m_pixmap;
	double m_scale;
};
