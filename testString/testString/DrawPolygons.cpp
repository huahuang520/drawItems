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
    ////=视图转场景坐标
    //QPointF fScenePoint = m_view->mapToScene(point);
    ////场景转图形项坐标
    //QGraphicsItem* pItem = m_scene->itemAt(fScenePoint, QTransform());

    //if (NULL == pItem)
    //{
    //    return;
    //}
    //QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
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
        //左上角
        m_view->setCursor(Qt::SizeFDiagCursor); //设置从左上到右下的斜线箭头光标
        dir = DIR_LEFTTOP_;
    }
    else if (mouseX >= rectBottomRight.x() - CORPADDING && mouseX <= rectBottomRight.x() && mouseY <= rectTopLeft.y() + CORPADDING && mouseY >= rectTopLeft.y())
    {
        //右上角
        m_view->setCursor(Qt::SizeBDiagCursor); //设置从左下到右上的斜线箭头光标。
        dir = DIR_RIGHTTOP_;
    }
    else if (mouseX <= rectTopLeft.x() + CORPADDING && mouseX >= rectTopLeft.x() && mouseY >= rectBottomRight.y() - CORPADDING && mouseY <= rectBottomRight.y())
    {
        //左下角
        m_view->setCursor(Qt::SizeBDiagCursor);
        dir = DIR_LEFTBOTTOM_;
    }
    else if (mouseX >= rectBottomRight.x() - CORPADDING && mouseX <= rectBottomRight.x() && mouseY >= rectBottomRight.y() - CORPADDING && mouseY <= rectBottomRight.y())
    {
        //右下角
        m_view->setCursor(Qt::SizeFDiagCursor);
        dir = DIR_RIGHTBOTTOM_;
    }
    else if (mouseX >= rectBottomRight.x() - EDGPADDING && mouseX <= rectBottomRight.x() && mouseY >= rectTopLeft.y() && mouseY <= rectBottomRight.y())
    {
        //右边
        m_view->setCursor(Qt::SizeHorCursor); //水平调整光标
        dir = DIR_RIGHT_;
    }
    else if (mouseY <= rectTopLeft.y() + EDGPADDING && mouseY >= rectTopLeft.y() && mouseX >= rectTopLeft.x() && mouseX <= rectBottomRight.x())
    {
        //上边
        m_view->setCursor(Qt::SizeVerCursor); //垂直调整光标
        dir = DIR_TOP_;
    }
    else if (mouseY >= rectBottomRight.y() - EDGPADDING && mouseY <= rectBottomRight.y() && mouseX >= rectTopLeft.x() && mouseX <= rectBottomRight.x())
    {
        //下边
        m_view->setCursor(Qt::SizeVerCursor);
        dir = DIR_BOTTOM_;
    }
    else if (mouseX <= rectTopLeft.x() + EDGPADDING && mouseX >= rectTopLeft.x() && mouseY >= rectTopLeft.y() && mouseY <= rectBottomRight.y())
    {
        //左边
        m_view->setCursor(Qt::SizeHorCursor);
        dir = DIR_LEFT_;
    }
    else if (m_Rect.contains(point))
    {
        //中间
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
        m_Rect = QRect(); //先清空
        QGraphicsRectItem* rectItem = (QGraphicsRectItem*)this;
        //让矩形初始点在鼠标点击的位置，这样在放大界面后依旧可以画矩形
        rectItem->setRect(QRectF(m_paintStartPoint, m_paintStartPoint)); 
        m_isReDrawPolygon = false;
        m_view->viewport()->update();
    }

    m_view->setCursor(Qt::ArrowCursor);                    //设置鼠标为指针形状

    int width = mousePoint.x() - m_paintStartPoint.x();  //移动的宽度
    int height = mousePoint.y() - m_paintStartPoint.y(); //移动的高度

    //确定矩形左上角坐标
    if (width < 0 && height < 0)
    {
        //鼠标向左上角移动
        m_Rect.setX(mousePoint.x());
        m_Rect.setY(mousePoint.y());
    }
    else if (width < 0)
    {
        //鼠标向左下位置移动
        m_Rect.setX(mousePoint.x());
        m_Rect.setY(m_paintStartPoint.y());
    }
    else if (height < 0)
    {
        //鼠标向右上位置移动
        m_Rect.setX(m_paintStartPoint.x());
        m_Rect.setY(mousePoint.y());
    }
    else
    {
        //正常  向右下移动
        m_Rect.setX(m_paintStartPoint.x());
        m_Rect.setY(m_paintStartPoint.y());
    }

    //设置矩形大小 绝对值 避免反方向的产生的负值
    m_Rect.setSize(QSize(abs(width) + 1, abs(height) + 1)); //+1是因为假如宽高都为265，则像素坐标范围为[0, 264] 
}

void drawRect::moveRect(const QPoint& mousePoint)
{
    m_view->setCursor(Qt::ClosedHandCursor);

    int width = mousePoint.x() - m_moveStartPoint.x();
    int height = mousePoint.y() - m_moveStartPoint.y();

    //如果矩形边界超过的图像边界进行处理
    if ((m_Rect.x() + width + m_Rect.width() > m_view->scene()->width() && m_Rect.y() + height + m_Rect.height() > m_view->scene()->height()) //右下
        || (m_Rect.x() + width + m_Rect.width() > m_view->scene()->width()) || (m_Rect.y() + height + m_Rect.height() > m_view->scene()->height()) //右边或下边
        || (m_Rect.x() + width < 0) || (m_Rect.y() + height < 0) //左边或上边
        || (m_Rect.x() + width + m_Rect.width() > m_view->scene()->width() && m_Rect.y() + height < 0) //右上
        || (m_Rect.x() + width < 0 && m_Rect.y() + height < 0) //左上
        || (m_Rect.x() + width < 0 && m_Rect.y() + height + m_Rect.height() > m_view->scene()->height())) //左下
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
    int width = mousePoint.x() - m_moveStartPoint.x();   //移动的宽度
    int height = mousePoint.y() - m_moveStartPoint.y();  //移动的高度

    //根据当前的缩放状态来改变矩形的位置大小信息
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
        //缩放的大小限制
        return;
    }

    m_Rect = newRect;
    m_moveStartPoint = mousePoint;  //更新鼠标的起始位置
}

void drawRect::mouseDrawRectBegin(const QPoint& point)
{
    if (!m_isPaintingEnable || !m_view) return;

    //视图转场景坐标
    QPointF fScenePoint = m_view->mapToScene(point);
    //场景转图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    isBeginGraw = true;
    Direction dir = region(itemPoint);     //获取鼠标当前的坐标在rect什么位置

    if (dir == DIR_MIDDLE_)
    {
        //鼠标在矩形中心位置
        m_view->setCursor(Qt::ClosedHandCursor);
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bMovedPressed = true;
    }
    else if (dir == DIR_NONE_)
    {
        //鼠标在矩形外部
        m_view->setCursor(Qt::ArrowCursor);
        m_bPainterPressed = true;
        m_paintStartPoint.setX(itemPoint.x());
        m_paintStartPoint.setY(itemPoint.y());
        m_isReDrawPolygon = true; //重绘矩形
    }
    else
    {
        //鼠标在矩形边缘
        m_moveStartPoint.setX(itemPoint.x());
        m_moveStartPoint.setY(itemPoint.y());
        m_bScalePressed = true;
    }
}

void drawRect::mouseDrawRectInProgress(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || !m_view) return;

    //视图转场景坐标
    QPointF fScenePoint = m_view->mapToScene(point);
    //转图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //绘制rect超过图像边界进行处理（鼠标按下绘制矩形拖到图像外部，这时pItem == NULL）
         //右下
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左上
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //右上
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //左下
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左边
        else if (fScenePoint.x() < 0)
        {
            //y不设置itemPoint.y的原因是因为一旦出了item区域，item坐标为空，这时只能获取sence坐标
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //右边
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //理由同上
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //上边
        else if (fScenePoint.y() < 0)
        {
            //理由同上
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //下边
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //理由同上
            QPoint item(fScenePoint.x(), m_view->scene()->height() - 1);
            itemPoint = item;
        }
    }

    if (m_bPainterPressed)
    {
        //正在绘制状态
        paintRect(itemPoint);

    }
    else if (m_bMovedPressed)
    {
        //判断拖动矩形是否超过图像边界
        //左上
        if ((m_Rect.x() <= 0 && m_Rect.y() <= 0))
        {
            m_Rect.setX(0);
            m_Rect.setY(0);
        }
        //右下
        else if ((m_Rect.bottomRight().x() >= m_view->scene()->width() - 1 && m_Rect.bottomRight().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.bottomRight().setX(m_view->scene()->width() - 1);
            m_Rect.bottomRight().setY(m_view->scene()->height() - 1);
        }
        //左下
        else if ((m_Rect.x() <= 0 && m_Rect.bottomLeft().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.setX(0);
            m_Rect.bottomLeft().setY(m_view->scene()->height() - 1);
        }
        //右上
        else if ((m_Rect.topRight().x() >= m_view->scene()->width() - 1 && m_Rect.y() <= 0))
        {
            m_Rect.topRight().setX(m_view->scene()->width() - 1);
            m_Rect.setY(0);
        }
        //左边
        else if ((m_Rect.x() <= 0))
        {
            m_Rect.setX(0);
        }
        //上边
        else if ((m_Rect.y() <= 0))
        {
            m_Rect.setY(0);
        }
        //右边
        else if ((m_Rect.bottomRight().x() >= m_view->scene()->width() - 1))
        {
            m_Rect.bottomRight().setX(m_view->scene()->width() - 1);
        }
        //下边
        else if ((m_Rect.bottomRight().y() >= m_view->scene()->height() - 1))
        {
            m_Rect.bottomRight().setY(m_view->scene()->height() - 1);
        }

        //正在移动状态
        moveRect(itemPoint);
    }
    else if (m_bScalePressed)
    {
        //正在拉伸大小状态
        scaleRect(itemPoint);
    }
    m_view->viewport()->update();
}

void drawRect::mouseDrawRectEnd(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || m_Rect.isNull() || !m_view) return;

    QPointF fScenePoint = m_view->mapToScene(point);
    //图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

        //判断鼠标是否在矩形中
        if (m_Rect.contains(itemPoint))
        {
            //松开鼠标前是否正在拖放
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

    //绘制矩形时只在界面点击了下便松开鼠标，这样不算绘制了新的矩形
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

    //如果鼠标拖着图形项移动，在没松开鼠标之前应该一直是关闭手的图标状态
    if (m_bMovedPressed)
    {
        m_view->setCursor(Qt::ClosedHandCursor);
        return;
    }

    QPointF fScenePoint = m_view->mapToScene(point);
    //图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (pItem == NULL)
    {
        m_view->setCursor(Qt::ArrowCursor);
        return;
    }

    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    //根据鼠标的位置设置当前的鼠标形状
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
    font.setPixelSize(12); //设置字体大小

    //painter->begin(); //绘画开始
   // painter->setBrush(QBrush(QColor(0, 0, 200, 20)));  //画刷填充框内颜色
    painter->setPen(m_pen);
    //painter->setFont(font);

    //QString strPoint = QString("X:%0, Y:%1").arg(m_Rect.x()).arg(m_Rect.y());           //rect位置信息
    //QString strSize = QString("W:%0, H:%1").arg(m_Rect.width()).arg(m_Rect.height());   //rect大小信息
    //QString roiIndex = QString("#rect:%0").arg(m_currentIndex); //rect序号

    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 5, strSize);
    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 18, strPoint);
    //painter->drawText(m_Rect.topLeft().x(), m_Rect.topLeft().y() - 31, roiIndex);

    // painter.drawRect(m_roiRect);
    painter->drawRect(m_Rect);

    //if (m_Rect.width() != 0 && m_Rect.height() != 0)
    //{
    //    //绘制中间十字架
    //    QPen dashPen(Qt::white);
    //    dashPen.setWidth(MIDDLELINE_WIDTH);
    //    dashPen.setStyle(Qt::DashDotLine);
    //    painter->setPen(dashPen);
    //    //从上往下画线，获取roi的y坐标始终为线宽的上边
    //    painter->drawLine(m_Rect.topLeft().x() + m_Rect.width() / 2, m_Rect.topLeft().y() + EDGE_WIDTH, m_Rect.bottomRight().x() - m_Rect.width() / 2, m_Rect.bottomRight().y());
    //    //从左往右画线， 获取roi的x坐标始终是线宽的左边
    //    painter->drawLine(m_Rect.topLeft().x() + EDGE_WIDTH, m_Rect.topLeft().y() + m_Rect.height() / 2, m_Rect.bottomRight().x(), m_Rect.bottomRight().y() - m_Rect.height() / 2);

    //    //绘制边缘九个点
    //    //painter.setBrush(Qt::green);
    //    //pen.setWidth(0);
    //    //painter.setPen(pen);

    //    //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT); //左上角
    //    //painter.drawRect(m_roiRect.topLeft().x(), m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //左边中心点
    //    //painter.drawRect(m_roiRect.bottomLeft().x(), m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //左下角
    //    //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y(), POINT_WIDTH, POINT_HEIGHT);  //顶部中心
    //    //painter.drawRect(m_roiRect.topLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.topLeft().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT);  //中心点
    //    //painter.drawRect(m_roiRect.bottomLeft().x() + m_roiRect.width() / 2 - POINT_WIDTH / 2, m_roiRect.bottomLeft().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //底部中心点
    //    //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y(), POINT_WIDTH, POINT_HEIGHT); //右上角
    //    //painter.drawRect(m_roiRect.topRight().x() - POINT_WIDTH, m_roiRect.topRight().y() + m_roiRect.height() / 2 - POINT_HEIGHT / 2, POINT_WIDTH, POINT_HEIGHT); //右边中心点
    //    //painter.drawRect(m_roiRect.bottomRight().x() - POINT_WIDTH, m_roiRect.bottomRight().y() - POINT_HEIGHT, POINT_WIDTH, POINT_HEIGHT); //右下角点
    //}
    //绘画结束
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

    //视图转场景坐标
    QPointF fScenePoint = m_view->mapToScene(point);
    //场景转图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    if (NULL == pItem)
    {
        isBeginGraw = false;
        return;
    }
    QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
    QPoint itemPoint(fItemPoint.x(), fItemPoint.y());

    isBeginGraw = true;
    m_isReDrawLine = true;
  
    //鼠标开始画线的起点
    m_paintStartPoint.setX(itemPoint.x());
    m_paintStartPoint.setY(itemPoint.y());
}

void drawLine::mouseDrawLineInProgress(const QPoint& point)
{
    if (!isBeginGraw || !m_isPaintingEnable || !m_view) return;

    //视图转场景坐标
    QPointF fScenePoint = m_view->mapToScene(point);
    //转图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //绘制直线超过图像边界进行处理（鼠标按下绘制直线拖到图像外部，这时pItem == NULL）
        //右下
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左上
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //右上
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //左下
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左边
        else if (fScenePoint.x() < 0)
        {
            //y不设置itemPoint.y的原因是因为一旦出了item区域，item坐标为空，这时只能获取sence坐标
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //右边
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //理由同上
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //上边
        else if (fScenePoint.y() < 0)
        {
            //理由同上
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //下边
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //理由同上
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

    //正在绘制状态
    paintLine(itemPoint);

    m_view->viewport()->update();
}

void drawLine::mouseDrawLineEnd(const QPoint& point)
{
    if (!m_isPaintingEnable || !m_view) return;

    QPointF fScenePoint = m_view->mapToScene(point);
    //图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
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
    font.setPixelSize(12); //设置字体大小

    //painter->begin(); //绘画开始
    painter->setPen(m_pen);
    //painter->setFont(font);

    //QString strPoint = QString("X1:%0, Y1:%1").arg(m_line.x1()).arg(m_line.y1());           //line点1位置信息
    //QString strSize = QString("X2:%0, Y2:%1").arg(m_line.x2()).arg(m_line.y2());       //line点2位置信息
    //QString roiIndex = QString("#line:%0").arg(m_currentIndex); //rect序号

    //painter->drawText(m_line.x1(), m_line.y1() - 5, strSize);
    //painter->drawText(m_line.x1(), m_line.y1() - 18, strPoint);
    //painter->drawText(m_line.x1(), m_line.y1() - 31, roiIndex);

    painter->drawLine(m_line);

    //绘画结束
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
    m_view->setCursor(Qt::ArrowCursor);                    //设置鼠标为指针形状

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
        //左键按下
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


    //视图转场景坐标
    QPointF fScenePoint = m_view->mapToScene(point);
    //转图形项坐标
    QGraphicsItem* pItem = m_view->scene()->itemAt(fScenePoint, QTransform());

    QPoint itemPoint;
    if (pItem != NULL)
    {
        QPointF fItemPoint = pItem->mapFromScene(fScenePoint);   //获取图形项坐标
        QPoint item(fItemPoint.x(), fItemPoint.y());
        itemPoint = item;
    }
    else
    {
        //绘制多边形超过图像边界进行处理（鼠标按下绘制多边形拖到图像外部，这时pItem == NULL）
        //右下
        if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() >= m_view->scene()->height())
        {
            QPoint item(m_view->scene()->width() - 1, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左上
        else if (fScenePoint.x() < 0 && fScenePoint.y() < 0)
        {
            QPoint item(0, 0);
            itemPoint = item;
        }
        //右上
        else if (fScenePoint.x() >= m_view->scene()->width() && fScenePoint.y() < 0)
        {
            QPoint item(m_view->scene()->width() - 1, 0);
            itemPoint = item;
        }
        //左下
        else if (fScenePoint.y() >= m_view->scene()->height() && fScenePoint.x() < 0)
        {
            QPoint item(0, m_view->scene()->height() - 1);
            itemPoint = item;
        }
        //左边
        else if (fScenePoint.x() < 0)
        {
            //y不设置itemPoint.y的原因是因为一旦出了item区域，item坐标为空，这时只能获取sence坐标
            //QPoint item(0, itemPoint.y());
            QPoint item(0, fScenePoint.y());
            itemPoint = item;
        }
        //右边
        else if (fScenePoint.x() >= m_view->scene()->width())
        {
            //理由同上
            QPoint item(m_view->scene()->width() - 1, fScenePoint.y());
            itemPoint = item;
        }
        //上边
        else if (fScenePoint.y() < 0)
        {
            //理由同上
            QPoint item(fScenePoint.x(), 0);
            itemPoint = item;
        }
        //下边
        else if (fScenePoint.y() >= m_view->scene()->height())
        {
            //理由同上
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
    font.setPixelSize(12); //设置字体大小

    //painter->begin(); //绘画开始
    painter->setPen(m_pen);
    painter->setFont(font);

    QString roiIndex = QString("#polygon:%0").arg(m_currentIndex); //Polygon序号

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
    m_view->setCursor(Qt::ArrowCursor);                    //设置鼠标为指针形状
    
    m_path.lineTo(mousePoint);
}

void drawPolygon::deleteItemSlot()
{
    emit deleteItemSignal(polygons::polygon, this);
}
