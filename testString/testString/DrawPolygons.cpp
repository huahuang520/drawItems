#include "DrawPolygons.h"
#include <QPixmap>
#include <QPen>
#include <QFont>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsPathItem>

DrawControl::DrawControl(QGraphicsView* view, QGraphicsScene* sence, QObject* parent)
    : QObject(parent)
    , m_view(view)
    , m_scene(sence)
    , m_currentRectId(0)
    , m_currentLineId(0)
    , m_currentPolygonsId(0)
{   
}

DrawControl::~DrawControl()
{
    for (auto it : m_itemsPtr)
    {
        for (auto gt : it.second)
        {
            delete gt.second;
        }
    }
}

QGraphicsItem* DrawControl::createDrawObject(polygons drawType, color color, int penWidth, bool isPaintingEnable)
{
    m_drawType = drawType;
    QPen pen;
    pen.setWidth(penWidth);
    switch (color)
    {
    case color::red:
        pen.setColor(QColor(255, 0, 0));
        break;
    case color::blue:
        pen.setColor(QColor(0, 0, 255));
        break;
    case color::black:
        pen.setColor(QColor(0, 0, 0));
        break;
    default:
        pen.setColor(QColor(0, 0, 0));
        break;
    }

    switch (drawType)
    {
    case polygons::rect:
    {   
        drawRect* rect = new drawRect(m_currentRectId, m_view, isPaintingEnable);
        connect(rect, &drawRect::deleteItemSignal, this, &DrawControl::delete_item);
        rect->setPen(pen);
        m_scene->addItem(rect);
        m_itemsPtr[polygons::rect][m_currentRectId++] = rect;
        return rect;
    }
    case polygons::line:
    {
        drawLine* line = new drawLine(m_currentLineId, m_view, isPaintingEnable);
        connect(line, &drawLine::deleteItemSignal, this, &DrawControl::delete_item);
        line->setPen(pen);
        m_scene->addItem(line);
        m_itemsPtr[polygons::line][m_currentLineId++] = line;
        return line;
    }
    case polygons::polygon:
    {
        drawPolygon* polygon = new drawPolygon(m_currentPolygonsId, m_view, isPaintingEnable);
        connect(polygon, &drawPolygon::deleteItemSignal, this, &DrawControl::delete_item);
        polygon->setPen(pen);
        m_scene->addItem(polygon);
        m_itemsPtr[polygons::polygon][m_currentPolygonsId++] = polygon;
        return polygon;
    }
    }
}

std::map<polygons, std::map<int, QGraphicsItem*>> DrawControl::getAllGraphicsItems()
{
    return m_itemsPtr;
}

void DrawControl::draw_Rect(QRect rect, QGraphicsItem* itemPtr)
{
    ((drawRect*)itemPtr)->setRect(rect);
    QPen pen = ((drawRect*)itemPtr)->getPen();
    QGraphicsRectItem* rectItem = (QGraphicsRectItem*)itemPtr;
    rectItem->setRect(rect);
    rectItem->setPen(pen);
}

void DrawControl::draw_Line(QLine line, QGraphicsItem* itemPtr)
{
    ((drawLine*)itemPtr)->setLine(line);
    QPen pen = ((drawLine*)itemPtr)->getPen();
    QGraphicsLineItem* lineItem = (QGraphicsLineItem*)itemPtr;
    lineItem->setLine(line);
    lineItem->setPen(pen);
}

void DrawControl::draw_Polygon(QPolygon polygon, QGraphicsItem* itemPtr)
{
    ((drawPolygon*)itemPtr)->setPolygon(polygon);
    QPen pen = ((drawPolygon*)itemPtr)->getPen();
    QGraphicsPolygonItem* polygonItem = (QGraphicsPolygonItem*)itemPtr;
    polygonItem->setPolygon(polygon);
    polygonItem->setPen(pen);
}

void DrawControl::delete_item(polygons drawType, QGraphicsItem* itemPtr)
{
    QPen pen;
    pen.setWidth(-1);
    for (auto it : m_itemsPtr[drawType])
    {
        if ((int)itemPtr == (int)it.second)
        {
            if (drawType == polygons::rect)
            {
                ((drawRect*)itemPtr)->setRect(QRect());
                QGraphicsRectItem* rectItem = (QGraphicsRectItem*)itemPtr;
                rectItem->setPen(pen);
                rectItem->setRect(QRect());
            }
            else if (drawType == polygons::line)
            {
                ((drawLine*)itemPtr)->setLine(QLine());
                QGraphicsLineItem* lineItem = (QGraphicsLineItem*)itemPtr;
                lineItem->setPen(pen);
                lineItem->setLine(QLine());
            }
            else if (drawType == polygons::polygon)
            {
                ((drawPolygon*)itemPtr)->setPolygon(QPolygon());
                QGraphicsPolygonItem* polygonItem = (QGraphicsPolygonItem*)itemPtr;
                polygonItem->setPen(pen);
                polygonItem->setPolygon(QPolygon());
            }
        }
    }
}

void DrawControl::mouseDrawBegin(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button)
{
    if (!m_view || !m_scene || m_drawType == polygons::none) return;
    ////=��ͼת��������
    //QPointF fScenePoint = m_view->mapToScene(point);
    ////����תͼ��������
    //QGraphicsItem* pItem = m_scene->itemAt(fScenePoint, QTransform());

    //if (NULL == pItem)
    //{
    //    return;
    //}
    //QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    //QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    switch (drawType)
    {
    case polygons::rect:
    {
        drawRect* rectItem = (drawRect*)itemPtr;
        rectItem->mouseDrawRectBegin(point);
        break;
    }
    case polygons::line:
    {
        drawLine* lineItem = (drawLine*)itemPtr;
        lineItem->mouseDrawLineBegin(point);
        break;
    }
    case polygons::polygon:
    {
        drawPolygon* polygonItem = (drawPolygon*)itemPtr;
        polygonItem->mouseDrawPolygonBegin(point, button);
        break;
    }
    }

    
}

void DrawControl::mouseDrawProgress(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr)
{
    if (!m_view || !m_scene || m_drawType == polygons::none) return;

    switch (drawType)
    {
    case polygons::rect:
    {
        drawRect* rectItem = (drawRect*)itemPtr;
        rectItem->mouseDrawRectInProgress(point);
        break;
    }
    case polygons::line:
    {
        drawLine* lineItem = (drawLine*)itemPtr;
        lineItem->mouseDrawLineInProgress(point);
        break;
    }
    case polygons::polygon:
    {
        drawPolygon* polygonItem = (drawPolygon*)itemPtr;
        polygonItem->mouseDrawPolygonInProgress(point);
        break;
    }
    }
}

void DrawControl::mouseDrawEnd(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button)
{
    if (!m_view || !m_scene || m_drawType == polygons::none) return;

    switch (drawType)
    {
    case polygons::rect:
    {
        drawRect* rectItem = (drawRect*)itemPtr;
        rectItem->mouseDrawRectEnd(point);
        break;
    }
    case polygons::line:
    {
        drawLine* lineItem = (drawLine*)itemPtr;
        lineItem->mouseDrawLineEnd(point);
        break;
    }
    case polygons::polygon:
    {
        drawPolygon* polygonItem = (drawPolygon*)itemPtr;
        polygonItem->mouseDrawPolygonEnd(point, button);
        break;
    }
    }
}

void DrawControl::updateMouseShape(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr)
{
    if (!m_view || !m_scene || m_drawType == polygons::none) return;

    switch (drawType)
    {
    case polygons::rect:
        drawRect* rectItem = (drawRect*)itemPtr;
        rectItem->changeMouseShape(point);
        break;
    }
}

drawRect::drawRect(int currentIndex, QGraphicsView* view, bool isPaintingEnable,  QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
    m_isPaintingEnable = isPaintingEnable;
    isBeginGraw = false;
    m_bMovedPressed = false;
    m_bPainterPressed = false;
    m_bScalePressed = false;
    m_currentIndex = currentIndex;
    m_view = view;
    m_isReDrawPolygon = true;
}

drawRect::~drawRect()
{
}

void drawRect::setRect(QRect rect)
{
    m_Rect = rect;
}

void drawRect::setPen(QPen pen)
{
    m_pen = pen;
}

QPen& drawRect::getPen()
{
    return m_pen;
}

Direction drawRect::region(const QPoint& point)
{
    int mouseX = point.x();
    int mouseY = point.y();

    if (m_Rect.isNull() || !m_isPaintingEnable || !m_view)
    {
        return DIR_NONE_;
    }

    QPoint rectTopLeft = m_Rect.topLeft();
    QPoint rectBottomRight = m_Rect.bottomRight();

    Direction dir = DIR_NONE_;

    if (mouseX <= rectTopLeft.x() + CORPADDING && mouseX >= rectTopLeft.x() && mouseY <= rectTopLeft.y() + CORPADDING && mouseY >= rectTopLeft.y())
    {
        //���Ͻ�
        m_view->setCursor(Qt::SizeFDiagCursor); //���ô����ϵ����µ�б�߼�ͷ���
        dir = DIR_LEFTTOP_;
    }
    else if (mouseX >= rectBottomRight.x() - CORPADDING && mouseX <= rectBottomRight.x() && mouseY <= rectTopLeft.y() + CORPADDING && mouseY >= rectTopLeft.y())
    {
        //���Ͻ�
        m_view->setCursor(Qt::SizeBDiagCursor); //���ô����µ����ϵ�б�߼�ͷ��ꡣ
        dir = DIR_RIGHTTOP_;
    }
    else if (mouseX <= rectTopLeft.x() + CORPADDING && mouseX >= rectTopLeft.x() && mouseY >= rectBottomRight.y() - CORPADDING && mouseY <= rectBottomRight.y())
    {
        //���½�
        m_view->setCursor(Qt::SizeBDiagCursor);
        dir = DIR_LEFTBOTTOM_;
    }
    else if (mouseX >= rectBottomRight.x() - CORPADDING && mouseX <= rectBottomRight.x() && mouseY >= rectBottomRight.y() - CORPADDING && mouseY <= rectBottomRight.y())
    {
        //���½�
        m_view->setCursor(Qt::SizeFDiagCursor);
        dir = DIR_RIGHTBOTTOM_;
    }
    else if (mouseX >= rectBottomRight.x() - EDGPADDING && mouseX <= rectBottomRight.x() && mouseY >= rectTopLeft.y() && mouseY <= rectBottomRight.y())
    {
        //�ұ�
        m_view->setCursor(Qt::SizeHorCursor); //ˮƽ�������
        dir = DIR_RIGHT_;
    }
    else if (mouseY <= rectTopLeft.y() + EDGPADDING && mouseY >= rectTopLeft.y() && mouseX >= rectTopLeft.x() && mouseX <= rectBottomRight.x())
    {
        //�ϱ�
        m_view->setCursor(Qt::SizeVerCursor); //��ֱ�������
        dir = DIR_TOP_;
    }
    else if (mouseY >= rectBottomRight.y() - EDGPADDING && mouseY <= rectBottomRight.y() && mouseX >= rectTopLeft.x() && mouseX <= rectBottomRight.x())
    {
        //�±�
        m_view->setCursor(Qt::SizeVerCursor);
        dir = DIR_BOTTOM_;
    }
    else if (mouseX <= rectTopLeft.x() + EDGPADDING && mouseX >= rectTopLeft.x() && mouseY >= rectTopLeft.y() && mouseY <= rectBottomRight.y())
    {
        //���
        m_view->setCursor(Qt::SizeHorCursor);
        dir = DIR_LEFT_;
    }
    else if (m_Rect.contains(point))
    {
        //�м�
        dir = DIR_MIDDLE_;
    }
    else
    {
        dir = DIR_NONE_;
    }
    m_emCurDir = dir;
    return dir;
}

void drawRect::paintRect(const QPoint& mousePoint)
{
    if (m_isReDrawPolygon && m_bPainterPressed)
    {
        m_Rect = QRect(); //�����
        QGraphicsRectItem* rectItem = (QGraphicsRectItem*)this;
        //�þ��γ�ʼ�����������λ�ã������ڷŴ��������ɿ��Ի�����
        rectItem->setRect(QRectF(m_paintStartPoint, m_paintStartPoint)); 
        m_isReDrawPolygon = false;
        m_view->viewport()->update();
    }

    m_view->setCursor(Qt::ArrowCursor);                    //�������Ϊָ����״

    int width = mousePoint.x() - m_paintStartPoint.x();  //�ƶ��Ŀ��
    int height = mousePoint.y() - m_paintStartPoint.y(); //�ƶ��ĸ߶�

    //ȷ���������Ͻ�����
    if (width < 0 && height < 0)
    {
        //��������Ͻ��ƶ�
        m_Rect.setX(mousePoint.x());
        m_Rect.setY(mousePoint.y());
    }
    else if (width < 0)
    {
        //���������λ���ƶ�
        m_Rect.setX(mousePoint.x());
        m_Rect.setY(m_paintStartPoint.y());
    }
    else if (height < 0)
    {
        //���������λ���ƶ�
        m_Rect.setX(m_paintStartPoint.x());
        m_Rect.setY(mousePoint.y());
    }
    else
    {
        //����  �������ƶ�
        m_Rect.setX(m_paintStartPoint.x());
        m_Rect.setY(m_paintStartPoint.y());
    }

    //���þ��δ�С ����ֵ ���ⷴ����Ĳ����ĸ�ֵ
    m_Rect.setSize(QSize(abs(width) + 1, abs(height) + 1)); //+1����Ϊ�����߶�Ϊ265�����������귶ΧΪ[0, 264] 
}

void drawRect::moveRect(const QPoint& mousePoint)
{
    m_view->setCursor(Qt::ClosedHandCursor);

    int width = mousePoint.x() - m_moveStartPoint.x();
    int height = mousePoint.y() - m_moveStartPoint.y();

    //������α߽糬����ͼ��߽���д���
    if ((m_Rect.x() + width + m_Rect.width() > m_view->scene()->width() && m_Rect.y() + height + m_Rect.height() > m_view->scene()->height()) //����
        || (m_Rect.x() + width + m_Rect.width() > m_view->scene()->width()) || (m_Rect.y() + height + m_Rect.height() > m_view->scene()->height()) //�ұ߻��±�
        || (m_Rect.x() + width < 0) || (m_Rect.y() + height < 0) //��߻��ϱ�
        || (m_Rect.x() + width + m_Rect.width() > m_view->scene()->width() && m_Rect.y() + height < 0) //����
        || (m_Rect.x() + width < 0 && m_Rect.y() + height < 0) //����
        || (m_Rect.x() + width < 0 && m_Rect.y() + height + m_Rect.height() > m_view->scene()->height())) //����
    {
        m_moveStartPoint = mousePoint;
        return;
    }

    QRect ret;
    ret.setX(m_Rect.x() + width);
    ret.setY(m_Rect.y() + height);
    ret.setSize(m_Rect.size());
    m_Rect = ret;
    m_moveStartPoint = mousePoint;
}

void drawRect::scaleRect(const QPoint& mousePoint)
{
    QRect newRect(m_Rect.topLeft(), m_Rect.bottomRight());
    int width = mousePoint.x() - m_moveStartPoint.x();   //�ƶ��Ŀ��
    int height = mousePoint.y() - m_moveStartPoint.y();  //�ƶ��ĸ߶�

    //���ݵ�ǰ������״̬���ı���ε�λ�ô�С��Ϣ
    switch (m_emCurDir)
    {
    case DIR_LEFT_:
        newRect.setLeft(mousePoint.x());
        break;
    case DIR_RIGHT_:
        newRect.setRight(mousePoint.x());
        break;
    case DIR_TOP_:
        newRect.setTop(mousePoint.y());
        break;
    case DIR_BOTTOM_:
        newRect.setBottom(mousePoint.y());
        break;
    case DIR_LEFTTOP_:
        newRect.setTopLeft(mousePoint);
        break;
    case DIR_LEFTBOTTOM_:
        newRect.setBottomLeft(mousePoint);
        break;
    case DIR_RIGHTTOP_:
        newRect.setTopRight(mousePoint);
        break;
    case DIR_RIGHTBOTTOM_:
        newRect.setBottomRight(mousePoint);
        break;
    }

    if (newRect.width() < MIN_WIDTH || newRect.height() < MIN_HEIGHT)
    {
        //���ŵĴ�С����
        return;
    }

    m_Rect = newRect;
    m_moveStartPoint = mousePoint;  //����������ʼλ��
}

void drawRect::mouseDrawRectBegin(const QPoint& point)
{
    if (!m_isPaintingEnable || !m_view) return;

    //��ͼת��������
    QPointF fScenePoint = m_view->mapToScene(point);
    //����תͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    isBeginGraw = true;
    Direction dir = region(itemPoint);     //��ȡ��굱ǰ��������rectʲôλ��

    if (dir == DIR_MIDDLE_)
    {
        //����ھ�������λ��
        m_view->setCursor(Qt::ClosedHandCursor);
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bMovedPressed = true;
    }
    else if (dir == DIR_NONE_)
    {
        //����ھ����ⲿ
        m_view->setCursor(Qt::ArrowCursor);
        m_bPainterPressed = true;
        m_paintStartPoint.setX(itemPoint.x());
        m_paintStartPoint.setY(itemPoint.y());
        m_isReDrawPolygon = true; //�ػ����
    }
    else
    {
        //����ھ��α�Ե
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bScalePressed = true;
    }
}

void drawRect::mouseDrawRectInProgress(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || !m_view) return;

    //��ͼת��������
    QPointF fScenePoint = m_view->mapToScene(point);
    //תͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //����rect����ͼ��߽���д�����갴�»��ƾ����ϵ�ͼ���ⲿ����ʱpItem == NULL��
         //����
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //���
        else if (fScenePoint.x() < 0)
        {
            //y������itemPoint.y��ԭ������Ϊһ������item����item����Ϊ�գ���ʱֻ�ܻ�ȡsence����
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //�ұ�
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //����ͬ��
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //�ϱ�
        else if (fScenePoint.y() < 0)
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //�±�
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), m_view->scene()->height() - 1);
            itemPoint = item;
        }
    }

    if (m_bPainterPressed)
    {
        //���ڻ���״̬
        paintRect(itemPoint);

    }
    else if (m_bMovedPressed)
    {
        //�ж��϶������Ƿ񳬹�ͼ��߽�
        //����
        if ((m_Rect.x() <= 0 && m_Rect.y() <= 0))
        {
            m_Rect.setX(0);
            m_Rect.setY(0);
        }
        //����
        else if ((m_Rect.bottomRight().x() >= m_view->scene()->width() - 1 && m_Rect.bottomRight().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.bottomRight().setX(m_view->scene()->width() - 1);
            m_Rect.bottomRight().setY(m_view->scene()->height() - 1);
        }
        //����
        else if ((m_Rect.x() <= 0 && m_Rect.bottomLeft().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.setX(0);
            m_Rect.bottomLeft().setY(m_view->scene()->height() - 1);
        }
        //����
        else if ((m_Rect.topRight().x() >= m_view->scene()->width() - 1 && m_Rect.y() <= 0))
        {
            m_Rect.topRight().setX(m_view->scene()->width() - 1);
            m_Rect.setY(0);
        }
        //���
        else if ((m_Rect.x() <= 0))
        {
            m_Rect.setX(0);
        }
        //�ϱ�
        else if ((m_Rect.y() <= 0))
        {
            m_Rect.setY(0);
        }
        //�ұ�
        else if ((m_Rect.bottomRight().x() >= m_view->scene()->width() - 1))
        {
            m_Rect.bottomRight().setX(m_view->scene()->width() - 1);
        }
        //�±�
        else if ((m_Rect.bottomRight().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.bottomRight().setY(m_view->scene()->height() - 1);
        }

        //�����ƶ�״̬
        moveRect(itemPoint);
    }
    else if (m_bScalePressed)
    {
        //���������С״̬
        scaleRect(itemPoint);
    }
    m_view->viewport()->update();
}

void drawRect::mouseDrawRectEnd(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || m_Rect.isNull() || !m_view) return;

    QPointF fScenePoint = m_view->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

        //�ж�����Ƿ��ھ�����
        if (m_Rect.contains(itemPoint))
        {
            //�ɿ����ǰ�Ƿ������Ϸ�
            if (m_bMovedPressed)
            {
                m_view->setCursor(Qt::OpenHandCursor);
            }
            else
            {
                m_view->setCursor(Qt::ArrowCursor);
            }
        }
    }

    //���ƾ���ʱֻ�ڽ��������±��ɿ���꣬��������������µľ���
    if (m_Rect.width() == 0 || m_Rect.height() == 0)
    {
        m_Rect = QRect();
    }

    m_paintStartPoint = QPoint();
    m_bMovedPressed = false;
    m_bPainterPressed = false;
    m_bScalePressed = false;
    isBeginGraw = false;
}

void drawRect::changeMouseShape(const QPoint& point)
{
    if (!m_isPaintingEnable || m_Rect.isNull() || !m_view)
    {
        m_view->setCursor(Qt::ArrowCursor);
        return;
    }

    //����������ͼ�����ƶ�����û�ɿ����֮ǰӦ��һֱ�ǹر��ֵ�ͼ��״̬
    if (m_bMovedPressed)
    {
        m_view->setCursor(Qt::ClosedHandCursor);
        return;
    }

    QPointF fScenePoint = m_view->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (pItem == NULL)
    {
        m_view->setCursor(Qt::ArrowCursor);
        return;
    }

    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    //��������λ�����õ�ǰ�������״
    Direction dir = region(itemPoint);

    if (dir == DIR_NONE_)
    {
        m_view->setCursor(Qt::ArrowCursor);
    }
    else if (dir == DIR_TOP_ || dir == DIR_BOTTOM_)
    {
        m_view->setCursor(Qt::SizeVerCursor);
    }
    else if (dir == DIR_LEFT_ || dir == DIR_RIGHT_)
    {
        m_view->setCursor(Qt::SizeHorCursor);
    }
    else if (dir == DIR_LEFTTOP_ || dir == DIR_RIGHTBOTTOM_)
    {
        m_view->setCursor(Qt::SizeFDiagCursor);
    }
    else if (dir == DIR_LEFTBOTTOM_ || dir == DIR_RIGHTTOP_)
    {
        m_view->setCursor(Qt::SizeBDiagCursor);
    }
    else if (dir == DIR_MIDDLE_ && !m_bMovedPressed)
    {
        m_view->setCursor(Qt::OpenHandCursor);
    }
    else if (dir == DIR_MIDDLE_ && m_bMovedPressed)
    {
        m_view->setCursor(Qt::ClosedHandCursor);
    }
}

void drawRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_Rect.isNull() || !m_isPaintingEnable || !m_view)
    {
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    QFont font;
    int width = m_pen.width();
    font.setPixelSize(12); //���������С

    //painter->begin(); //�滭��ʼ
   // painter->setBrush(QBrush(QColor(0, 0, 200, 20)));  //��ˢ��������ɫ
    painter->setPen(m_pen);
    //painter->setFont(font);

    //QString strPoint = QString("X:%0, Y:%1").arg(m_Rect.x()).arg(m_Rect.y());           //rectλ����Ϣ
    //QString strSize = QString("W:%0, H:%1").arg(m_Rect.width()).arg(m_Rect.height());   //rect��С��Ϣ
    //QString roiIndex = QString("#rect:%0").arg(m_currentIndex); //rect���

    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 5, strSize);
    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 18, strPoint);
    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 31, roiIndex);

    // painter.drawRect(m_roiRect);
    painter->drawRect(m_Rect);

    //if (m_Rect.width() != 0 && m_Rect.height() != 0)
    //{
    //    //�����м�ʮ�ּ�
    //    QPen dashPen(Qt::white);
    //    dashPen.setWidth(MIDDLELINE_WIDTH);
    //    dashPen.setStyle(Qt::DashDotLine);
    //    painter->setPen(dashPen);
    //    //�������»��ߣ���ȡroi��y����ʼ��Ϊ�߿���ϱ�
    //    painter->drawLine(m_Rect.topLeft().x() + m_Rect.width() / 2, m_Rect.topLeft().y() + EDGE_WIDTH, m_Rect.bottomRight().x() - m_Rect.width() / 2, m_Rect.bottomRight().y());
    //    //�������һ��ߣ� ��ȡroi��x����ʼ�����߿�����
    //    painter->drawLine(m_Rect.topLeft().x() + EDGE_WIDTH, m_Rect.topLeft().y() + m_Rect.height() / 2, m_Rect.bottomRight().x(), m_Rect.bottomRight().y() - m_Rect.height() / 2);

    //    //���Ʊ�Ե�Ÿ���
    //    //painter.setBrush(Qt::green);
    //    //pen.setWidth(0);
    //    //painter.setPen(pen);

    //    //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT); //���Ͻ�
    //    //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //������ĵ�
    //    //painter.drawRect(m_roiRect.bottomLeft().x(), m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //���½�
    //    //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT);  //��������
    //    //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT);  //���ĵ�
    //    //painter.drawRect(m_roiRect.bottomLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //�ײ����ĵ�
    //    //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y(), POINT_WIDTH, POINT_HEIGHT); //���Ͻ�
    //    //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //�ұ����ĵ�
    //    //painter.drawRect(m_roiRect.bottomRight().x() - POINT_WIDTH, m_roiRect.bottomRight().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //���½ǵ�
    //}
    //�滭����
    // painter.end();
    QGraphicsRectItem* rectItem = (QGraphicsRectItem*)this;
    rectItem->setRect(m_Rect);
    m_view->viewport()->update();
}

void drawRect::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu(m_view);
    QAction m_pDelAction(QStringLiteral("delete"), m_view);
    menu.addAction(&m_pDelAction);
    connect(&m_pDelAction, SIGNAL(triggered()), this, SLOT(deleteItemSlot()));

    menu.exec(QCursor::pos());
}

void drawRect::deleteItemSlot()
{
    emit deleteItemSignal(polygons::rect, this);
}

drawLine::drawLine(int currentIndex, QGraphicsView* view, bool isPaintingEnable, QGraphicsItem* parent) : QGraphicsLineItem(parent)
{
    m_isPaintingEnable = isPaintingEnable;
    m_currentIndex = currentIndex;
    m_view = view;
    isBeginGraw = false;
    m_isReDrawLine = true;
}

drawLine::~drawLine()
{
}

void drawLine::setLine(QLine line)
{
    m_line = line;
}

void drawLine::setPen(QPen pen)
{
    m_pen = pen;
}

QPen& drawLine::getPen()
{
    return m_pen;
}

void drawLine::mouseDrawLineBegin(const QPoint& point)
{
    if (!m_isPaintingEnable || !m_view) return;

    //��ͼת��������
    QPointF fScenePoint = m_view->mapToScene(point);
    //����תͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    isBeginGraw = true;
    m_isReDrawLine = true;
  
    //��꿪ʼ���ߵ����
    m_paintStartPoint.setX(itemPoint.x());
    m_paintStartPoint.setY(itemPoint.y());
}

void drawLine::mouseDrawLineInProgress(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || !m_view) return;

    //��ͼת��������
    QPointF fScenePoint = m_view->mapToScene(point);
    //תͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //����ֱ�߳���ͼ��߽���д�����갴�»���ֱ���ϵ�ͼ���ⲿ����ʱpItem == NULL��
        //����
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //���
        else if (fScenePoint.x() < 0)
        {
            //y������itemPoint.y��ԭ������Ϊһ������item����item����Ϊ�գ���ʱֻ�ܻ�ȡsence����
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //�ұ�
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //����ͬ��
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //�ϱ�
        else if (fScenePoint.y() < 0)
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //�±�
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), m_view->scene()->height() - 1);
            itemPoint = item;
        }
    }

    if (m_isReDrawLine)
    {
        m_line = QLine();
        QGraphicsLineItem* lineItem = (QGraphicsLineItem*)this;
        lineItem->setLine(QLine(itemPoint, itemPoint));
        m_view->viewport()->update();
        m_isReDrawLine = false;
    }

    //���ڻ���״̬
    paintLine(itemPoint);

    m_view->viewport()->update();
}

void drawLine::mouseDrawLineEnd(const QPoint& point)
{
    if (!m_isPaintingEnable || !m_view) return;

    QPointF fScenePoint = m_view->mapToScene(point);
    //ͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());
    }

    m_paintStartPoint = QPoint();
    isBeginGraw = false;
}

void drawLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (m_line.isNull() || !m_isPaintingEnable || !m_view)
    {
       /* QGraphicsLineItem* lineItem = (QGraphicsLineItem*)this;
        lineItem->setLine(m_line);*/
        QGraphicsLineItem::paint(painter, option, widget);
        return;
    }

    QFont font;
    int width = m_pen.width();
    font.setPixelSize(12); //���������С

    //painter->begin(); //�滭��ʼ
    painter->setPen(m_pen);
    //painter->setFont(font);

    //QString strPoint = QString("X1:%0, Y1:%1").arg(m_line.x1()).arg(m_line.y1());           //line��1λ����Ϣ
    //QString strSize = QString("X2:%0, Y2:%1").arg(m_line.x2()).arg(m_line.y2());       //line��2λ����Ϣ
    //QString roiIndex = QString("#line:%0").arg(m_currentIndex); //rect���

    //painter->drawText(m_line.x1(), m_line.y1() - 5, strSize);
    //painter->drawText(m_line.x1(), m_line.y1() - 18, strPoint);
    //painter->drawText(m_line.x1(), m_line.y1() - 31, roiIndex);

    painter->drawLine(m_line);

    //�滭����
    // painter.end();
    QGraphicsLineItem* lineItem = (QGraphicsLineItem*)this;
    lineItem->setLine(m_line);
    m_view->viewport()->update();
}

void drawLine::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu(m_view);
    QAction m_pDelAction(QStringLiteral("delete"), m_view);
    menu.addAction(&m_pDelAction);
    connect(&m_pDelAction, SIGNAL(triggered()), this, SLOT(deleteItemSlot()));
    menu.exec(QCursor::pos());
}

void drawLine::paintLine(const QPoint& mousePoint)
{
    m_view->setCursor(Qt::ArrowCursor);                    //�������Ϊָ����״

    m_line.setPoints(m_paintStartPoint, mousePoint);
}

void drawLine::deleteItemSlot()
{
    emit deleteItemSignal(polygons::line, this);
}

drawPolygon::drawPolygon(int currentIndex, QGraphicsView* view, bool isPaintingEnable, QGraphicsItem* parent) : QGraphicsPolygonItem(parent)
{
    m_isPaintingEnable = isPaintingEnable;
    m_currentIndex = currentIndex;
    m_view = view;
    m_isLeftButtonPress = false;
    m_isRightButtonPress = false;
    m_isPaintEnd = false;
    m_isReDrawPolygon = true;
    pathItem = new QGraphicsPathItem();
}

drawPolygon::~drawPolygon()
{
    delete pathItem;
}

void drawPolygon::setPolygon(QPolygon polygon)
{
    m_polygon = polygon;
}

void drawPolygon::setPen(QPen pen)
{
    m_pen = pen;
}

QPen& drawPolygon::getPen()
{
    return m_pen;
}

void drawPolygon::mouseDrawPolygonBegin(const QPoint& point, Qt::MouseButton button)
{
    if (!m_isPaintingEnable || !m_view) return;

    if (button == Qt::MouseButton::RightButton)
    {
        m_isRightButtonPress = true;
    }
    else
    {
        //�������
        m_isLeftButtonPress = true;
    }
    m_isPaintEnd = false;
}

void drawPolygon::mouseDrawPolygonInProgress(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || !m_view) return;
}

void drawPolygon::mouseDrawPolygonEnd(const QPoint& point, Qt::MouseButton button)
{
    if (button == Qt::MouseButton::LeftButton)
    {
        m_isLeftButtonPress = false;
        m_isPaintEnd = true;
        m_isReDrawPolygon = true;
    }
    else if (button == Qt::MouseButton::RightButton)
        m_isRightButtonPress = false;


    //��ͼת��������
    QPointF fScenePoint = m_view->mapToScene(point);
    //תͼ��������
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //��ȡͼ��������
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //���ƶ���γ���ͼ��߽���д�����갴�»��ƶ�����ϵ�ͼ���ⲿ����ʱpItem == NULL��
        //����
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //����
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //���
        else if (fScenePoint.x() < 0)
        {
            //y������itemPoint.y��ԭ������Ϊһ������item����item����Ϊ�գ���ʱֻ�ܻ�ȡsence����
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //�ұ�
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //����ͬ��
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //�ϱ�
        else if (fScenePoint.y() < 0)
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //�±�
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //����ͬ��
            QPoint item(fScenePoint.x(), m_view->scene()->height() - 1);
            itemPoint = item;
        }
    }

    if (m_isLeftButtonPress && !m_isRightButtonPress)
    {
        if (m_isReDrawPolygon)
        {
            m_path.clear();
            m_polygon.clear();
            m_isReDrawPolygon = false;
            
            QGraphicsPolygonItem* polygonItem = (QGraphicsPolygonItem*)this;
            polygonItem->setPen(m_pen);
            m_polygon.append(itemPoint);
            polygonItem->setPolygon(m_polygon);

            m_path.moveTo(itemPoint);
            
            m_view->viewport()->update();
            return;
        }
        
        m_path.lineTo(itemPoint); 

        m_polygon << itemPoint;

    }
    //prepareGeometryChange();
    m_view->viewport()->update();
}

void drawPolygon::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (!m_path.elementCount() || !m_isPaintingEnable || !m_view)
    {
        QGraphicsPolygonItem::paint(painter, option, widget);

        return;
    }

    QFont font;
    int width = m_pen.width();
    font.setPixelSize(12); //���������С

    //painter->begin(); //�滭��ʼ
    painter->setPen(m_pen);
    painter->setFont(font);

    QString roiIndex = QString("#polygon:%0").arg(m_currentIndex); //Polygon���

   // painter->drawText(m_paintStartPoint.x(), m_paintStartPoint.y() - 5, roiIndex);
    if (m_isPaintEnd)
    {
        m_path.clear();
        pathItem->setPath(m_path);
        QGraphicsPolygonItem* polygonItem = (QGraphicsPolygonItem*)this;
        polygonItem->setPen(m_pen);
        polygonItem->setPolygon(m_polygon);
    }
    else
    {
        painter->drawPath(m_path);
        pathItem->setPath(m_path);
        
    }
    QGraphicsPolygonItem::paint(painter, option, widget);
   
    m_view->viewport()->update();
}

void drawPolygon::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    if (m_isLeftButtonPress) return;

    QMenu menu(m_view);
    QAction m_pDelAction(QStringLiteral("delete"), m_view);
    menu.addAction(&m_pDelAction);
    connect(&m_pDelAction, SIGNAL(triggered()), this, SLOT(deleteItemSlot()));
    menu.exec(QCursor::pos());
}


void drawPolygon::paintPolygon(const QPoint& mousePoint)
{
    m_view->setCursor(Qt::ArrowCursor);                    //�������Ϊָ����״
    
    m_path.lineTo(mousePoint);
}

void drawPolygon::deleteItemSlot()
{
    emit deleteItemSignal(polygons::polygon, this);
}
