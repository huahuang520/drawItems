#include "GraphicsView.h"
#include <QGraphicsBlurEffect>

GraphicsView::GraphicsView(QWidget*parent)
	: QGraphicsView(parent)
    , isDrawROI(true)
{
    m_scene = new QGraphicsScene();
    m_PixmapItem = new QGraphicsPixmapItem();

    this->setScene(m_scene);

    m_scene->addItem(m_PixmapItem);

    setMouseTracking(true);  //Ϊfalseʱ��Ҫ��������Ŵ�������ƶ��¼�

    m_scaledValue = 1; //���ű���

    //�����Ϊ���Ľ�������
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    m_pOptMenu = new QMenu(this);
    m_pDelAction = new QAction(QStringLiteral("ɾ��"), this);
   // connect(m_pDelAction, &QAction::triggered, m_drawROI, &DrawROI::DeleteCurrentROI);
    m_pSaveAction = new QAction(QStringLiteral("����"), this);
   // connect(m_pSaveAction, &QAction::triggered, m_drawROI, &DrawROI::saveAllROIImage);

    m_pOptMenu->addAction(m_pDelAction);
    m_pOptMenu->addAction(m_pSaveAction);

    m_graphicsViewTool = new GraphicsViewTool(this, m_scene, m_PixmapItem);


    drawControl = new DrawControl(this, m_scene);
    rect_item = drawControl->createDrawObject(polygons::rect, color::blue, 1, true);
    rect_item1 = drawControl->createDrawObject(polygons::rect, color::blue, 1, true);
    polygons_item = drawControl->createDrawObject(polygons::polygon, color::red, 1, true);
    line_item = drawControl->createDrawObject(polygons::line, color::red, 1, true);

    
}

GraphicsView::~GraphicsView()
{
    delete m_scene;
}

QImage GraphicsView::GetImage()
{
    return m_image;
}

void GraphicsView::SetPix(QPixmap pix)
{
    this->pix = pix;
}

qreal GraphicsView::GetScaledValue()
{
    return m_scaledValue;
}

//��갴���¼�
void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    //������Ϊthis->width(), this->height()������this->viewport()->width()��this->viewport()->height()������Ҫ�ܶ������֤
    qreal viewWidth = this->viewport()->width(); //859
    qreal viewHeight = this->viewport()->height(); //517
    qreal fviewWidth = this->width(); //882
    qreal fviewHeight = this->height(); //519
    qreal pviewWidth  = m_graphicsViewTool->GetCurrentPix().width(); //778
    qreal pviewHeight = m_graphicsViewTool->GetCurrentPix().height(); //519
    qreal sceneWidth = this->scene()->width();
    qreal sceneHeight = this->scene()->height();

    //����������
    if (event->button() == Qt::LeftButton) {
        //��ͼ����
        QPoint nViewPoint = event->pos();
        //��ͼ����С������
        QPointF fViewPoint = event->pos();  //QPointFתQPoint  QPoint = QPointF.toPoint();
        //��������
        QPointF fScenePoint = this->mapToScene(nViewPoint);
        //ͼ��������
        QGraphicsItem* pItem = this->scene()->itemAt(fScenePoint, QTransform());
        
        if (NULL != pItem)
        {
            QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
            //ͼԪת��ͼ
            QPointF fViewPoint1 = this->mapFromScene(fItemPoint.x(), fItemPoint.y());
            int a = 0;
        }

       // drawControl->mouseDrawBegin(polygons::rect, nViewPoint, rect_item);
       // drawControl->mouseDrawBegin(polygons::line, nViewPoint, line_item);
        
    }
    drawControl->mouseDrawBegin(polygons::polygon, event->pos(), polygons_item, event->button());

    //������д�����mousePressEvent�¼�����ִ��mousePressEvent��ʱ��ֻ��ִ��������д�Ĺ��ܣ������Ҫ�õ����������������Ҫ����
    //����mousePressEvent�����¼�
    QGraphicsView::mousePressEvent(event);

    
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {

       // drawControl->mouseDrawEnd(polygons::rect, event->pos(), rect_item);
        
    }
    drawControl->mouseDrawEnd(polygons::polygon, event->pos(), polygons_item, event->button());

    QGraphicsView::mouseReleaseEvent(event);

}

void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        //isDoubleClick = true;

    }

    QGraphicsView::mouseDoubleClickEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    //��갴ס����ƶ�
    if (event->buttons() & Qt::LeftButton) {

        //drawControl->mouseDrawProgress(polygons::rect, event->pos(),rect_item);
       // drawControl->mouseDrawProgress(polygons::line, event->pos(), line_item);
    }
    //drawControl->updateMouseShape(polygons::rect, event->pos(), rect_item);
    
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent* event)
{
    //ֱ���ø���ļ��̰����¼����й���
    QGraphicsView::keyPressEvent(event);
    if (event->key() == Qt::Key_Delete)
    {
        this->viewport()->update();
    }
}

//���ڱ仯�����������²��֡�����update/repaint(�ػ�)�����ڵ�һ����ʾ������������ڵ�����ʾ�ȴ���paintEvent
void GraphicsView::paintEvent(QPaintEvent* event)
{
    //���ø���滭����
    QGraphicsView::paintEvent(event);
}

/**
 * @brief RectView::paintEvent
 *     ˫�����ͼ��ԭ�������϶��������Ȱ�ԭ����ͼ�θ��Ƶ�tempPix���沢��tempPix���滭��
 * ���Ǵ�ʱ�����ľ�����tempPix���ͼ�Ρ�ֻ������ͷŵ�ʱ�����pix��һ�Ρ�
 */
//void GraphicsView::paintEvent(QPaintEvent* event)
//{
//    
//    QPainter painter(this->viewport());
//    if (isDrawing)
//    {
//        tempPix = pix;
//        QPainter pp(&tempPix);
//        pp.drawLine(lasetPoint, endPoint);
//        painter.drawPixmap(0, 0, tempPix);
//
//    }
//    else
//    {
//        QPainter pp(&pix);
//        pp.drawLine(lasetPoint, endPoint);
//        painter.drawPixmap(0, 0, pix);
//    }
//
//    QGraphicsView::paintEvent(event);
//  
//}

//�����С�����仯����ø��¼�
void GraphicsView::resizeEvent(QResizeEvent* event)
{
    m_graphicsViewTool->SetPixMiddle(m_image);
   
}

void GraphicsView::wheelEvent(QWheelEvent* ev)
{
    //�Ŵ�ͼԪ
    m_graphicsViewTool->ScaleImage(ev);
    //ͼԪ�Ŵ���ͼԪ�ϻ���roi����Ŵ����淽�����û���roi��ͼԪͬ�����Ŵ�����Ļ�������QPainter��ʽ�������Ķ���������QGraphicsItemͼԪ�ı���ı�
    //�����ͼԪ����Ȼ��QGraphicsItemͼԪ�����������ǻ�һ��ı䣬�Զ�ͬ�����Ŵ���С����Ϊ��������QGraphicsϵ��

    QGraphicsView::wheelEvent(ev);

}

void GraphicsView::contextMenuEvent(QContextMenuEvent* ev)
{
    QPoint mousePos = ev->pos();

    //QRect.contains�����ĵ��ھ����ڲ�����true
    //��������
    QPointF fScenePoint = this->mapToScene(mousePos);
    //ͼ��������
    QGraphicsItem* pItem = this->scene()->itemAt(fScenePoint, QTransform());

    if (NULL != pItem)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

        /*for (int i = 0; i < m_drawROI->getRoiRect().size(); i++)
        {
            if (m_drawROI->getRoiRect()[i].contains(itemPoint))
            {
                m_pOptMenu->exec(QCursor::pos());
                break;
            }
        }*/
    }
    
    ev->accept();
    QGraphicsView::contextMenuEvent(ev);
}

void GraphicsView::rotate()
{
    // boundingRect�õ�ͼ����������С�����Σ� minAreaRect�õ�ͼ����������Сб����

    //QPointF centerPos = m_PixmapItem->boundingRect().center(); //��ȡͼ����������
    //m_PixmapItem->setTransformOriginPoint(centerPos.x(), centerPos.y()); //����QGraphicsPixmapItem��ת�㣬��תQGraphicsPixmapItem����ͼ��ͼ�����QGraphicsPixmapItem����
    //m_PixmapItem->setRotation(90); //����QGraphicsPixmapItem��ת����

    //�ڶ��ַ�������ֱ����opencv��תͼ��

    //ͼ����ת����ܵ�view����߻����ұߣ����������setPixMap���setSceneRect��֤ͼ����view������ת
    //m_PixmapItem->setPixmap(QPixmap::fromImage(image).scaled(s, Qt::KeepAspectRatio));
    //m_Scene->setSceneRect(m_PixmapItem->boundingRect()); //Image center rotation
}

void GraphicsView::setBackImage(QImage img)
{
    m_image = img;
    QSize s = this->size();
    //�����ݺ�Ⱥ�ͼ������ͼ���ģ�����ȡm_PixmapItem������Ȼ��(0, 0)��ע������ͼ���scene�����Ͻ����꣬�����Ҫ��ͼ������Ҫת��
    QPixmap pix = QPixmap::fromImage(m_image).scaled(s, Qt::KeepAspectRatio);
    this->pix = pix;
    m_PixmapItem->setPixmap(pix);

    //QGraphicsRectItem* item = new QGraphicsRectItem();
    //item->setRect(0, 0, 80, 80);
    //item->setFlag(QGraphicsItem::ItemIsMovable);
    //m_scene->addItem(item);
}

void GraphicsView::drawPolygon_()
{

    drawControl->draw_Rect(QRect(0, 0, 100, 100), rect_item);
    drawControl->draw_Rect(QRect(150, 150, 50, 50), rect_item1);
    
    QPolygon polygon;
    polygon << QPoint(200, 100) << QPoint(250, 150) << QPoint(150, 150) << QPoint(150, 100);
    drawControl->draw_Polygon(polygon, polygons_item);

    drawControl->draw_Line(QLine(80,80,180,180), line_item);
    
    ////ʹitem��ģ��
    //QGraphicsBlurEffect* m_blurEffect = new QGraphicsBlurEffect(this);
    //m_blurEffect->setBlurRadius(10);

    //rect_item->setGraphicsEffect(m_blurEffect);
}
