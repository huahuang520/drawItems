#pragma once
#include <QObject>
#include <QGraphicsItem>
#include <QPen>

#define EDGE_WIDTH       2       //�߿�Ŀ��
#define MIDDLELINE_WIDTH 1       //�����ߵĿ��

#define POINT_WIDTH      6        //��Ե9��Ŀ��
#define POINT_HEIGHT     6        //��Ե9��ĸ߶�

#define EDGPADDING       5        //���ܱ�Ե��������
#define CORPADDING       6        //�Ľǿ�������

#define MIN_WIDTH        5        //���������С���
#define MIN_HEIGHT       5        //���������С�߶�

/* ���ŷ��� */
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

//������������
typedef enum Polygons
{
	rect,  //����
	line,  //ֱ��
	polygon, //�����
	none
}polygons;

//���������ɫ
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
	//�����滭���ͣ����Σ�Բ...
	//isPaintingEnable����false��ʾͨ���������ƶ���Σ�ͬʱ����paint�滭����
	QGraphicsItem* createDrawObject(polygons drawType, color color,int penWidth = 1, bool isPaintingEnable = false);

	//���ݾ��β������ƾ���
	void draw_Rect(QRect rect, QGraphicsItem* itemPtr);

	//����ֱ�߲�������ֱ��
	void draw_Line(QLine line, QGraphicsItem* itemPtr);

	//���ݶ���β������ƶ����
	void draw_Polygon(QPolygon polygon, QGraphicsItem* itemPtr);
	
public:
	//��ȡ���д�����ͼ����item����ָ��
	std::map<polygons, std::map<int, QGraphicsItem*>> getAllGraphicsItems();

	//��갴�¿�ʼ�滭�������QGraphicsView�е�mousePressEvent�¼����꣨��ͼview���꣩
	void mouseDrawBegin(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button = Qt::MouseButton::LeftButton);

	//����Ѿ����£��ƶ������л滭�����ƶ�����Σ������QGraphicsView�е�mouseMoveEvent�¼����꣨��ͼview���꣩
	void mouseDrawProgress(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr);

	//����ɿ�����ɻ滭������϶��������QGraphicsView�е�mouseReleaseEvent�¼����꣨��ͼview���꣩
	void mouseDrawEnd(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr, Qt::MouseButton button = Qt::MouseButton::LeftButton);

	//ʵʱ���������״�������QGraphicsView�е�mouseMoveEvent�¼����꣨��ͼview���꣩
	void updateMouseShape(polygons drawType, const QPoint& point, QGraphicsItem* itemPtr);

private:
	//ɾ��ָ�������
	void delete_item(polygons drawType, QGraphicsItem* itemPtr);

private:
	polygons m_drawType = polygons::none;
	QGraphicsView* m_view = nullptr;
	QGraphicsScene* m_scene = nullptr;

	int m_currentRectId; //����id
	int m_currentLineId; //ֱ��id
	int m_currentPolygonsId; //�����id
	std::map<polygons, std::map<int, QGraphicsItem*>> m_itemsPtr;  //�������ͼ����item��ָ��
};

class drawRect : public QObject, public QGraphicsRectItem
{
	Q_OBJECT
public:
	drawRect(int currentIndex, QGraphicsView* view,  bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawRect();
	
public:
	//���þ��β���
	void setRect(QRect rect);
	//���û���
	void setPen(QPen pen);
	//��ȡ����
	QPen& getPen();
	//��갴�¿�ʼ�滭���贫����갴��ʱ����ͼview����
	void mouseDrawRectBegin(const QPoint& point);
	//���տ�ʼ���¸����������ھ������滹���������ж��Ƿ��ƶ����λ��ǻ��µľ��Σ�������ͼview����
	void mouseDrawRectInProgress(const QPoint& point);
	//pointΪ��ͼ���꣬����ͷŵ�ʱ���������״̬�ı�
	void mouseDrawRectEnd(const QPoint& point);
	//pointΪ��ͼ���꣬��������λ�����õ�ǰ�������״
	void changeMouseShape(const QPoint& point);

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
private:
	//�ж���굱ǰ�����ھ���ʲôλ��(����ͼ����item����)
	Direction region(const QPoint& point);
	//���ƾ���
	void paintRect(const QPoint& mousePoint);
	//�ƶ�����
	void moveRect(const QPoint& mousePoint);
	//�������
	void scaleRect(const QPoint& mousePoint);  
	
public slots:
	void deleteItemSlot();

signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	QRect m_Rect;                 //��ž�������
	QPoint m_moveStartPoint;       //�϶��ĳ�ʼλ��
	bool m_bMovedPressed;          //�Ƿ������϶�
	bool m_bPainterPressed;        //�Ƿ����ڻ���
	QPoint m_paintStartPoint;      //���Ƶĳ�ʼλ��
	bool m_bScalePressed;          //�Ƿ��������Ŵ�С
	bool isBeginGraw;             //��ʼ�滭��־
	Direction m_emCurDir;        //��ǰ�϶��ķ���
	bool m_isPaintingEnable;         //����falseֻ��ͨ�������������Σ����ֻ滭�����޷�ʹ��
	int m_currentIndex;          //��ǰ����id
	QGraphicsView* m_view = nullptr;
	QPen m_pen;   
	bool m_isReDrawPolygon;  //�Ƿ����»��ƾ���
};

class drawLine : public QObject, public QGraphicsLineItem
{
	Q_OBJECT
public:
	drawLine(int currentIndex, QGraphicsView* view, bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawLine();

public:
	//����ֱ�߲���
	void setLine(QLine line);      
	//���û���
	void setPen(QPen pen);
	//��ȡ����
	QPen& getPen();
	//��갴�¿�ʼ�滭���贫����갴��ʱ����ͼview����
	void mouseDrawLineBegin(const QPoint& point);
	//���տ�ʼ���¸����������ھ������滹���������ж��Ƿ��ƶ����λ��ǻ��µľ��Σ�������ͼview����
	void mouseDrawLineInProgress(const QPoint& point);
	//pointΪ��ͼ���꣬����ͷŵ�ʱ���������״̬�ı�
	void mouseDrawLineEnd(const QPoint& point);

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget); //��д����
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
public slots:
	void deleteItemSlot();

private:
	//����ֱ��
	void paintLine(const QPoint& mousePoint);
signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	bool m_isPaintingEnable;         //����falseֻ��ͨ�������������Σ����ֻ滭�����޷�ʹ��
	QLine m_line;                 //���ֱ������
	int m_currentIndex;          //��ǰֱ��id
	QGraphicsView* m_view = nullptr;
	QPen m_pen; 
	bool isBeginGraw;             //��ʼ�滭��־
	QPoint m_paintStartPoint;       //���ߵĳ�ʼλ��
	bool m_isReDrawLine;  //�Ƿ����»���ֱ��
};

class drawPolygon : public QObject, public QGraphicsPolygonItem
{
	Q_OBJECT
public:
	drawPolygon(int currentIndex, QGraphicsView* view, bool isPaintingEnable = false, QGraphicsItem* parent = nullptr);
	~drawPolygon();

public:
	//���ö���β���
	void setPolygon(QPolygon polygon);
	//���û���
	void setPen(QPen pen);
	//��ȡ����
	QPen& getPen();
	//��갴�¿�ʼ�滭���贫����갴��ʱ����ͼview����
	void mouseDrawPolygonBegin(const QPoint& point, Qt::MouseButton button);
	//���տ�ʼ���¸����������ھ������滹���������ж��Ƿ��ƶ����λ��ǻ��µľ��Σ�������ͼview����
	void mouseDrawPolygonInProgress(const QPoint& point);
	//pointΪ��ͼ���꣬����ͷŵ�ʱ���������״̬�ı�
	void mouseDrawPolygonEnd(const QPoint& point, Qt::MouseButton button);
protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget); //��д����
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

public slots:
	void deleteItemSlot();

signals:
	void deleteItemSignal(polygons, QGraphicsItem*);

private:
	//���ƶ����
	void paintPolygon(const QPoint& mousePoint);

private:
	bool m_isPaintingEnable;         //����falseֻ��ͨ�������������Σ����ֻ滭�����޷�ʹ��
	QPolygon  m_polygon;             //��Ŷ��������
	int m_currentIndex;             //��ǰ�����id
	QGraphicsView* m_view = nullptr;
	QPen m_pen;
	bool isBeginGraw;             //��ʼ�滭��־

	QGraphicsPathItem* pathItem;   //·��item
	QPainterPath m_path;
	bool m_isLeftButtonPress;
	bool m_isRightButtonPress;
	bool m_isPaintEnd;
	bool m_isReDrawPolygon;  //�Ƿ����»��ƶ����
};