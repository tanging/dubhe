#pragma once


#include <deque>
#include <QLabel>
#include <QPainter>
#include <QPoint>
#include <QMessageBox>
#include <QMouseEvent>

#include "name.h"


typedef enum FrameType
{
    FRAME_TYPE_NONE = 0,
    FRAME_TYPE_RECT = 1,       //十字架，也就是画矩形
    FRAME_TYPE_POLY = 2	        //多边形    
}FRAME_TYPE_EN;

typedef struct MoreRect
{
	QPoint rectStart;               // 矩形起点
	QPoint rectEnd;                 // 矩形终点
    QPoint imageRectStart;          // 原始图片坐标
    QPoint imageRectEnd;            // 原始图片坐标
	std::string strName;            // 矩形名称
	int iW;                         //原始图片宽高
    int iH;
}MORE_RECT_ST;
typedef std::vector<MORE_RECT_ST>MORE_RECT_VTR;


class CCross : public QLabel
{
    Q_OBJECT
protected:
    // 处理鼠标按下事件
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    // 绘制矩形框
    virtual void paintEvent(QPaintEvent* event) override;
public:
    explicit CCross(QWidget* parent = nullptr);
    void setDrawCross(bool enable);
    void SetDrawPoly(bool bEnable);
    void setCrossPos(const QPoint& pos);
    void UpdateRect(qreal scaleFactorX, qreal scaleFactorY);
    void GetRectPoint(QPoint &rectStart, QPoint &rectEnd);
    void UpdateRect(double dZoom);
    void GetName(std::string &strName);
    void SetLabelsName(std::string strName);
    void GetScreenCenter(int& iX, int& iY);
    void SetFrameType(FRAME_TYPE_EN enType);
    FRAME_TYPE_EN GetFrameType();
    void RemoveLastPoint();
    void ClearPoints();
	void SetRectPoint(const QPoint& start, const QPoint& end, 
        const QPoint& imgStart, const QPoint& imgEnd, 
        int iW, int iH, std::string& strName);
    void DrawText(std::string& strName, int iX, int iY);
	void ClearRect();
    void DrawRect(QPainter& painter);
    void UpdateRectsForZoom(const QPixmap& scaledPixmap, const QSize& labelSize, int iWidth, int iHeight);
    QVector<QPoint> GetPolyPoints() const;
    void SavePolyPoints(QPoint point, int iW, int iH);
    QPoint mapPointToImage(const QPoint& labelPt, int imgW, int imgH, const QPixmap& pixmap, const QSize& labelSize);
private:
    bool isInResizeArea(const QPoint& point);
    bool isInTopLeft(const QPoint& point);
    bool isInTopRight(const QPoint& point);
    bool isInBottomLeft(const QPoint& point);
    bool isInBottomRight(const QPoint& point);

    bool isInTopEdge(const QPoint& point);
    bool isInBottomEdge(const QPoint& point);
    bool isInLeftEdge(const QPoint& point);
    bool isInRightEdge(const QPoint& point);
private:
    void SetSig();
    void SetVal();
    void SetStyle();
signals:
	void ReqDubheUI();
    void UpdateDubheLabel(QPoint rectStart, QPoint rectEnd);
private slots:
	void OkBtnSlots(const char *pName);
private:
    int m_iX;                       //屏幕中心坐标
    int m_iY;
private:
    bool m_drawPoly = false;
    bool m_drawCross = false;                 
    QPoint m_crossPos;
private:
    bool m_crossMode = false;         // 是否处于十字架模式
    bool m_rectDrawing = false;       // 是否正在画矩形

    QPoint m_rectStart;               // 矩形起点
	QPoint m_rectEnd;                 // 矩形终点
private:
    QRect rect;                     // 矩形区域
    QPoint dragStartPos;            // 鼠标按下时的位置
    bool dragging;                  // 是否正在拖动
    bool resizing;                  // 是否正在调整大小
    int cornerSize = 5;             // 小圆点的半径
    int borderWidth = 5;            // 边界区域的宽度
private:
    bool m_bImageScale = false;
    FRAME_TYPE_EN m_eFrameType;     //当前的框选类型
private:
    QLineEdit* m_pInput;
    std::string m_strName;
private:
    QVector<QPoint> m_vtrImagePoints; // 原始图片坐标
    QVector<QPoint> m_vtrPoints; // 存储点击的点
    MORE_RECT_VTR m_vtrRect;
private:
    CName m_oName;
};