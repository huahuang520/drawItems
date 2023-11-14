#include "GraphicsViewTool.h"

GraphicsViewTool::GraphicsViewTool(QObject *parent)
	: QObject(parent)
{

}

GraphicsViewTool::GraphicsViewTool(QGraphicsView* graphicsView, QGraphicsScene* scene, QGraphicsPixmapItem* pixmapItem, QObject* parent)
    : QObject(parent)
{
    m_graphicsView = graphicsView;
    m_scene = scene;
    m_pixmapItem = pixmapItem;
	m_scale = 1;
}

GraphicsViewTool::~GraphicsViewTool()
{
}

void GraphicsViewTool::SetPixMiddle(const QImage& image)
{
    QSize s = m_graphicsView->size();
    //让图像跟界面保持纵横比，图像会以界面宽或高小的一边作为pix的宽或高，跟随界面变化。
	//界面宽高为this->width(), this->height()，不是this->viewport()->width()和this->viewport()->height()，但需要很多测试验证
    QPixmap pix = QPixmap::fromImage(image).scaled(s, Qt::KeepAspectRatio);  //界面变化不会改变image的size，因为先把image变成了pix，然后对pix进行缩放，所以只改变了Pix的size
    m_pixmap = pix;
	int w = pix.width();
	int h = pix.height();

    m_pixmapItem->setPixmap(pix);

    m_scene->setSceneRect(0, 0, pix.width(), pix.height());  //保持图像在scene正中央

    m_graphicsView->viewport()->update();
}

QPixmap& GraphicsViewTool::GetCurrentPix()
{
    return m_pixmap;
}

void GraphicsViewTool::ScaleImage(QWheelEvent* event)
{
    if (Qt::CTRL == event->modifiers())   //键盘CTRL键
    {
        // 获取当前鼠标相对于view的位置;
        QPointF cursorPoint = event->pos();
        // 获取当前鼠标相对于scene的位置;
        QPointF scenePos = m_graphicsView->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
		//图形项坐标
		QGraphicsItem* pItem = m_graphicsView->scene()->itemAt(scenePos, QTransform());

		if (NULL != pItem)
		{
			QPointF fItemPoint = pItem->mapFromScene(scenePos);   //获取图形项坐标

			int a = 0;
		}


		// 获取view的宽高;
		qreal viewWidth = m_pixmap.width();
		qreal viewHeight = m_pixmap.height();

		// 获取当前鼠标位置相当于view大小的横纵比例;
		qreal hScale = cursorPoint.x() / viewWidth;
		qreal vScale = cursorPoint.y() / viewHeight;

		// 当前放缩倍数;
		qreal scaleFactor = m_graphicsView->matrix().m11(); //m11为水平缩放
		int wheelDeltaValue = event->delta();
		// 向上滚动，放大;
		if (wheelDeltaValue > 0)
		{
			m_graphicsView->scale(1.2, 1.2);
			m_scale = 1.2;
		}
		// 向下滚动，缩小;
		else
		{
			m_graphicsView->scale(1.0 / 1.2, 1.0 / 1.2);
			m_scale = 1.0 / 1.2;
		}
		
		// 将scene坐标转换为放大缩小后的坐标;
		QPointF scenePoint = m_graphicsView->matrix().map(scenePos);
		//图形项坐标
		QGraphicsItem* pItem1 = m_graphicsView->scene()->itemAt(scenePoint, QTransform());

		if (NULL != pItem1)
		{
			QPointF fItemPoint = pItem1->mapFromScene(scenePoint);   //获取图形项坐标

			int a = 0;
		}

		// 通过滚动条控制view放大缩小后的展示scene的位置;
		m_graphicsView->horizontalScrollBar()->setValue(int(scenePoint.x() - viewWidth * hScale));
		m_graphicsView->verticalScrollBar()->setValue(int(scenePoint.y() - viewHeight * vScale));
    }
}

double GraphicsViewTool::GetScale()
{
	return m_scale;
}
