#pragma once
#include <QObject>
#include <QGraphicsItem>
#include <QPen>

#define EDGE_WIDTH       2       //边框的宽度
#define MIDDLELINE_WIDTH 1       //辅助线的宽度

#define POINT_WIDTH      6        //边缘9点的宽度
#define POINT_HEIGHT     6        //边缘9点的高度

#define EDGPADDING       5        //四周边缘可拉伸宽度
#define CORPADDING       6        //四角可拉伸宽度

#define MIN_WIDTH        5        //可拉伸的最小宽度
#define MIN_HEIGHT       5        //可拉伸的最小高度

/* 缩放方向 */
enum Direction
{
	DIR_TOP_ = 0,
	DIR_BOTTOM_,
	DIR_LEFT_,
	DIR_RIGHT_,
	DIR_LEFTTOP_,
	DIR_LEFTBOTTOM_,
	DIR_RIGHTTOP_,
	DIR_RIGHTBOTTOM_,
	DIR_MIDDLE_,
	DIR_NONE_
};

//定义多边形类型
typedef enum Polygons
{
	rect,  //矩形
	line,  //直线
	polygon, //多边形
	none
}polygons;

//定义绘制颜色
typedef enum Color
{
	red,
	blue,
	black
}color;

class QGraphicsView;
class QGraphicsScene;
class QGraphicsLineItem;
class QGraphicsRectItem;
class QGraphicsPolygonItem;
class QGraphicsPathItem;
class QMenu;
class QAction;
class DrawControl : public QObject
{
	Q_OBJECT
public:
	DrawControl(QGraphicsView* view, QGraphicsScene* sence, QObject* parent = nullptr);
	~DrawControl();

public:
	//创建绘画类型：矩形，圆...
	//isPaintingEnable设置false表示通过参数绘制多边形，同时禁用paint绘画功能
	QGraphicsItem* createDrawObject(polygons drawType, color color,int penWidth = 1, bool isPaintingEnable = false);

	//根据矩形参数绘制矩形
	void draw_Rect(QRect rect, QGraphicsItem* itemPtr);

	//根据直线参数绘制直线
	void draw_Line(QLine line, QGraphicsItem* itemPtr);

	//根据多边形参数绘制多边形
	void draw_Polygon(QPolygon polygon, QGraphicsItem* itemPtr);
	
public:
	//获取所有创建的图形项item对象指针
	std::map<polygons, std::map<int, QGraphicsItem*>> getAllGraphicsItems();

	//鼠标按下开始绘画，需接收QGraphicsView中的mousePressEvent事件坐标（视图view坐标）
	void mouseDrawBegin(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button = Qt::MouseButton::LeftButton);

	//鼠标已经按下，移动鼠标进行绘画或者移动多边形，需接收QGraphicsView中的mouseMoveEvent事件坐标（视图view坐标）
	void mouseDrawProgress(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr);

	//鼠标松开，完成绘画或完成拖动，需接收QGraphicsView中的mouseReleaseEvent事件坐标（视图view坐标）
	void mouseDrawEnd(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button = Qt::MouseButton::LeftButton);

	//实时更新鼠标形状，需接收QGraphicsView中的mouseMoveEvent事件坐标（视图view坐标）
	void updateMouseShape(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr);

private:
	//删除指定多边形
	void delete_item(polygons drawType, QGraphicsItem* itemPtr);

private:
	polygons m_drawType = polygons::none;
	QGraphicsView* m_view = nullptr;
	QGraphicsScene* m_scene = nullptr;

	int m_currentRectId; //矩形id
	int m_currentLineId; //直线id
	int m_currentPolygonsId; //多边形id
	std::map<polygons, std::map<int, QGraphicsItem*>> m_itemsPtr;  //存放所有图形项item的指针
};

class drawRect : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
public:
	drawRect(int currentIndex, QGraphicsView* view,  bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawRect();
	
public:
	//设置矩形参数
	void setRect(QRect rect);
	//设置画笔
	void setPen(QPen pen);
	//获取画笔
	QPen& getPen();
	//鼠标按下开始绘画，需传入鼠标按下时的视图view坐标
	void mouseDrawRectBegin(const QPoint& point);
	//鼠标刚开始按下根据坐标是在矩形里面还是外面来判断是否移动矩形还是画新的矩形，传入视图view坐标
	void mouseDrawRectInProgress(const QPoint& point);
	//point为视图坐标，鼠标释放的时候结束矩形状态改变
	void mouseDrawRectEnd(const QPoint& point);
	//point为视图坐标，根据鼠标的位置设置当前的鼠标形状
	void changeMouseShape(const QPoint& point);

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
private:
	//判断鼠标当前坐标在矩形什么位置(传入图形项item坐标)
	Direction region(const QPoint& point);
	//绘制矩形
	void paintRect(const QPoint& mousePoint);
	//移动矩形
	void moveRect(const QPoint& mousePoint);
	//拉伸矩形
	void scaleRect(const QPoint& mousePoint);  
	
public slots:
	void deleteItemSlot();

signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	QRect m_Rect;                 //存放矩形数据
	QPoint m_moveStartPoint;       //拖动的初始位置
	bool m_bMovedPressed;          //是否正在拖动
	bool m_bPainterPressed;        //是否正在绘制
	QPoint m_paintStartPoint;      //绘制的初始位置
	bool m_bScalePressed;          //是否正在缩放大小
	bool isBeginGraw;             //开始绘画标志
	Direction m_emCurDir;        //当前拖动的方向
	bool m_isPaintingEnable;         //设置false只能通过参数创建矩形，各种绘画功能无法使用
	int m_currentIndex;          //当前矩形id
	QGraphicsView* m_view = nullptr;
	QPen m_pen;   
	bool m_isReDrawPolygon;  //是否重新绘制矩形
};

class drawLine : public QObject, public QGraphicsLineItem
{
	Q_OBJECT
public:
	drawLine(int currentIndex, QGraphicsView* view, bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawLine();

public:
	//设置直线参数
	void setLine(QLine line);      
	//设置画笔
	void setPen(QPen pen);
	//获取画笔
	QPen& getPen();
	//鼠标按下开始绘画，需传入鼠标按下时的视图view坐标
	void mouseDrawLineBegin(const QPoint& point);
	//鼠标刚开始按下根据坐标是在矩形里面还是外面来判断是否移动矩形还是画新的矩形，传入视图view坐标
	void mouseDrawLineInProgress(const QPoint& point);
	//point为视图坐标，鼠标释放的时候结束矩形状态改变
	void mouseDrawLineEnd(const QPoint& point);

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget); //重写画板
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
public slots:
	void deleteItemSlot();

private:
	//绘制直线
	void paintLine(const QPoint& mousePoint);
signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	bool m_isPaintingEnable;         //设置false只能通过参数创建矩形，各种绘画功能无法使用
	QLine m_line;                 //存放直线数据
	int m_currentIndex;          //当前直线id
	QGraphicsView* m_view = nullptr;
	QPen m_pen; 
	bool isBeginGraw;             //开始绘画标志
	QPoint m_paintStartPoint;       //画线的初始位置
	bool m_isReDrawLine;  //是否重新绘制直线
};

class drawPolygon : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT
public:
	drawPolygon(int currentIndex, QGraphicsView* view, bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawPolygon();

public:
	//设置多边形参数
	void setPolygon(QPolygon polygon);
	//设置画笔
	void setPen(QPen pen);
	//获取画笔
	QPen& getPen();
	//鼠标按下开始绘画，需传入鼠标按下时的视图view坐标
	void mouseDrawPolygonBegin(const QPoint& point, Qt::MouseButton button);
	//鼠标刚开始按下根据坐标是在矩形里面还是外面来判断是否移动矩形还是画新的矩形，传入视图view坐标
	void mouseDrawPolygonInProgress(const QPoint& point);
	//point为视图坐标，鼠标释放的时候结束矩形状态改变
	void mouseDrawPolygonEnd(const QPoint& point, Qt::MouseButton button);
protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget); //重写画板
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

public slots:
	void deleteItemSlot();

signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	//绘制多边形
	void paintPolygon(const QPoint& mousePoint);

private:
	bool m_isPaintingEnable;         //设置false只能通过参数创建矩形，各种绘画功能无法使用
	QPolygon  m_polygon;             //存放多边形数据
	int m_currentIndex;             //当前多边形id
	QGraphicsView* m_view = nullptr;
	QPen m_pen;
	bool isBeginGraw;             //开始绘画标志

	QGraphicsPathItem* pathItem;   //路径item
	QPainterPath m_path;
	bool m_isLeftButtonPress;
	bool m_isRightButtonPress;
	bool m_isPaintEnd;
	bool m_isReDrawPolygon;  //是否重新绘制多边形
};