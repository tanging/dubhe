#include "cross.h"

CCross::CCross(QWidget* parent) : QLabel(parent), m_drawCross(false) 
{
	m_pInput = NULL;
	m_eFrameType = FRAME_TYPE_EN::FRAME_TYPE_NONE;

	SetSig();
	SetVal();
	SetStyle();

	m_oName.InitName();
}

void CCross::SetSig()
{
	connect(&m_oName, &CName::ReqCrossUI, this, &CCross::OkBtnSlots);
}

void CCross::SetVal()
{
	QScreen* screen = QApplication::primaryScreen();
	if (NULL == screen)
	{
		return;
	}
	QRect screenGeometry = screen->availableGeometry();

	// 计算居中位置
	m_iX = (screenGeometry.width() - m_oName.width()) / 2;
	m_iY = (screenGeometry.height() - m_oName.height()) / 2;

}

void CCross::SetStyle()
{

}

void CCross::OkBtnSlots(const char *pName)
{
	m_strName = pName;

	if (FRAME_TYPE_EN::FRAME_TYPE_RECT == m_eFrameType)
	{
		size_t iCount = m_vtrRect.size() - 1;
		m_vtrRect[iCount].strName = pName;
	}

	update();
	emit ReqDubheUI();
}

void CCross::GetName(std::string& strName)
{
	strName = m_strName;
}

void CCross::setDrawCross(bool enable) 
{ 
    setMouseTracking(true);
    m_drawCross = enable; 

	// 清除矩形框相关数据
	m_rectStart = QPoint();
	m_rectEnd = QPoint();
	rect = QRect();
	m_rectDrawing = false;
	m_vtrRect.clear();

    update(); 
}

void CCross::SetDrawPoly(bool bEnable)
{
	m_drawPoly = bEnable;
	update();
}

void CCross::setCrossPos(const QPoint& pos) 
{ 
    m_crossPos = pos; 
    update(); 
}

void CCross::ClearRect()
{
	m_vtrRect.clear();
}

void CCross::DrawText(std::string &strName, int iX, int iY)
{
	if (!strName.empty())
	{
		QPainter painter(this);
		painter.setPen(QPen(Qt::red, 10));  // 设置文本颜色
		painter.drawText(iX, iY, m_strName.c_str());  // 在矩形框左上角绘制文本
	}
}

void CCross::DrawRect(QPainter &painter)
{
	painter.drawRect(rect);
	// 绘制四个角的小圆点
	painter.setBrush(Qt::black);
	painter.drawEllipse(rect.topLeft(), 5, 5);  // 左上角
	painter.drawEllipse(rect.topRight(), 5, 5); // 右上角
	painter.drawEllipse(rect.bottomLeft(), 5, 5); // 左下角
	painter.drawEllipse(rect.bottomRight(), 5, 5); // 右下角
	// 绘制四个边中点的小圆点
	painter.drawEllipse(QPoint(rect.left() + rect.width() / 2, rect.top()), 5, 5);    // 上边中点
	painter.drawEllipse(QPoint(rect.left() + rect.width() / 2, rect.bottom()), 5, 5);  // 下边中点
	painter.drawEllipse(QPoint(rect.left(), rect.top() + rect.height() / 2), 5, 5);    // 左边中点
	painter.drawEllipse(QPoint(rect.right(), rect.top() + rect.height() / 2), 5, 5);   // 右边中点
}

void CCross::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);
	if (FRAME_TYPE_EN::FRAME_TYPE_RECT == m_eFrameType)
	{
		if (true == m_drawCross)
		{
			QPainter painter(this);
			QPen pen(Qt::green, 1);
			painter.setPen(pen);
			painter.drawLine(0, m_crossPos.y(), width(), m_crossPos.y());
			painter.drawLine(m_crossPos.x(), 0, m_crossPos.x(), height());
		}

		if ((true == resizing) || (true == dragging))
		{
			QPainter painter(this);
			painter.setPen(QPen(Qt::blue, 2));  // 设置绘制的笔刷和线条宽度
			DrawRect(painter);
			DrawText(m_strName, m_rectStart.x(), m_rectStart.y() - 5);
			//把rect转换成rectStart和rectEnd;
			QPoint rectStart = rect.topLeft();
			QPoint rectEnd = rect.bottomRight();
			emit UpdateDubheLabel(rectStart, rectEnd);
			return;
		}

		if (true == m_bImageScale)
		{
			QPainter painter(this);
			painter.setPen(QPen(Qt::blue, 2));  // 设置绘制的笔刷和线条宽度
			DrawRect(painter);
			DrawText(m_strName, m_rectStart.x(), m_rectStart.y() - 5);
			m_bImageScale = false;
			return;
		}

		// 画当前正在拖动的矩形
		if (m_rectDrawing) 
		{
			QPainter painter(this);
			painter.setPen(QPen(Qt::blue, 2));  // 设置绘制的笔刷和线条宽度
			rect = QRect(m_rectStart, m_rectEnd).normalized();
			DrawRect(painter);
		}

		if (!m_vtrRect.empty())
		{
			MORE_RECT_VTR::iterator iter = m_vtrRect.begin();
			for (; iter != m_vtrRect.end(); iter++)
			{
				QPainter painter(this);
				painter.setPen(QPen(Qt::blue, 2));  // 设置绘制的笔刷和线条宽度
				rect = QRect(iter->rectStart, iter->rectEnd).normalized();
				DrawRect(painter);
				DrawText(iter->strName, iter->rectStart.x(), iter->rectStart.y() - 5);
			}
		}

		return;
	}

	if (FRAME_TYPE_EN::FRAME_TYPE_POLY == m_eFrameType)
	{
		QPainter painter(this);
		painter.setPen(QPen(Qt::blue, 2));

		// 绘制所有的点
		for (int i = 0; i < m_vtrPoints.size(); ++i) 
		{
			const QPoint& point = m_vtrPoints[i];
			if (i == 0) {
				// 第一个点固定为 6
				painter.drawEllipse(point, 6, 6);
			}
			else {
				// 其他点为 3
				painter.drawEllipse(point, 3, 3);
			}
		}


		// 绘制线段
		if (m_vtrPoints.size() > 1) 
		{
			for (int i = 1; i < m_vtrPoints.size(); ++i) 
			{
				painter.drawLine(m_vtrPoints[i - 1], m_vtrPoints[i]);
			}
		}

		// 如果点数超过2且已经连接到第一个点，画回到第一个点的线段
		if (m_vtrPoints.size() > 2 && m_vtrPoints.first() != m_vtrPoints.last()) 
		{
			painter.drawLine(m_vtrPoints.last(), m_vtrPoints.first());
		}

		return;
	}

	if(FRAME_TYPE_EN::FRAME_TYPE_NONE == m_eFrameType)
	{
		return;
	}
}

void CCross::mousePressEvent(QMouseEvent* event)
{
	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_NONE)
	{
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_RECT)
	{
		if (isInResizeArea(event->pos())) {
			resizing = true;  // 开始调整大小
			dragStartPos = event->pos();  // 记录开始拖动的位置
			return;
		}
		else if (rect.contains(event->pos())) {
			dragging = true;  // 开始拖动矩形
			dragStartPos = event->pos();
			return;
		}

		m_strName.clear();
		// 记录鼠标按下的起始位置
		m_rectStart = event->pos();
		m_rectDrawing = true;  // 开始绘制
		//update();  // 重新绘制窗口
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_POLY)
	{
		QPoint clickPos = event->pos();
		if (m_vtrPoints.size() > 2 && ((m_vtrPoints.first() - clickPos).manhattanLength() < 10))
		{
			// 如果回到第一个点，关闭图形绘制
			m_oName.setWindowModality(Qt::ApplicationModal);
			m_oName.move(m_iX, m_iY);
			m_oName.show();

			//m_vtrPoints.clear();
			//update();
		}
		else {
			m_vtrPoints.append(event->pos());
			if (m_vtrPoints.size() >= 1)
			{
				// 如果有多个点，就绘制线段
				update();
			}
		}
	}
}

// 处理鼠标释放事件
void CCross::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_NONE)
	{
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_RECT)
	{
		if (true == dragging || true == resizing)
		{
			dragging = false;
			resizing = false;
			return;
		}

		m_strName.clear();
		// 记录鼠标松开时的结束位置
		m_rectEnd = event->pos();
		m_rectDrawing = false;  // 停止绘制
		m_drawCross = false;

		MORE_RECT_ST stRect;
		stRect.rectStart = m_rectStart;
		stRect.rectEnd = m_rectEnd;
		m_vtrRect.push_back(stRect);

		update();  // 重新绘制窗口

		m_oName.setWindowModality(Qt::ApplicationModal);
		m_oName.move(m_iX, m_iY);
		m_oName.show();

		//获取图片坐标
		//QRect recvRect = QRect(m_rectStart, m_rectEnd).normalized();
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_POLY)
	{

	}

}

// 处理鼠标移动事件
void CCross::mouseMoveEvent(QMouseEvent* event)
{
	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_NONE)
	{
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_RECT)
	{
		if (true == dragging)
		{
			QPoint offset = event->pos() - dragStartPos;
			rect.moveTopLeft(rect.topLeft() + offset);  // 移动矩形
			dragStartPos = event->pos();
			update();
			return;
		}

		if (true == resizing) 
		{
			QPoint offset = event->pos() - dragStartPos;

			// 判断是调整哪个角落或边的大小
			if (isInTopLeft(event->pos())) {
				rect.setTopLeft(rect.topLeft() + offset);
			}
			else if (isInTopRight(event->pos())) {
				rect.setTopRight(rect.topRight() + offset);
			}
			else if (isInBottomLeft(event->pos())) {
				rect.setBottomLeft(rect.bottomLeft() + offset);
			}
			else if (isInBottomRight(event->pos())) {
				rect.setBottomRight(rect.bottomRight() + offset);
			}
			else if (isInTopEdge(event->pos())) {
				rect.setTop(rect.top() + offset.y());
			}
			else if (isInBottomEdge(event->pos())) {
				rect.setBottom(rect.bottom() + offset.y());
			}
			else if (isInLeftEdge(event->pos())) {
				rect.setLeft(rect.left() + offset.x());
			}
			else if (isInRightEdge(event->pos())) {
				rect.setRight(rect.right() + offset.x());
			}

			dragStartPos = event->pos();
			update();
			return;
		}

		if (m_rectDrawing) {
			// 在鼠标按下时画出矩形框
			m_rectEnd = event->pos();
			update();  // 实时更新绘制
		}
		return;
	}

	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_POLY)
	{

	}
}

void CCross::SavePolyPoints(QPoint point, int iW, int iH)
{
	m_vtrPoints.append(point);
	// 转换为图片坐标
	QPoint imgPt = mapPointToImage(point, iW, iH, pixmap(), size());
	m_vtrImagePoints.append(imgPt);
}

void CCross::SetFrameType(FRAME_TYPE_EN enType)
{
	m_eFrameType = enType;
}

FRAME_TYPE_EN CCross::GetFrameType()
{
	return m_eFrameType;
}

QVector<QPoint> CCross::GetPolyPoints() const
{
	return m_vtrPoints;
}

void CCross::SetLabelsName(std::string strName)
{
	m_oName.InsertLabel(strName);
}

void CCross::GetScreenCenter(int& iX, int& iY)
{
	iX = m_iX;
	iY = m_iY;
}

void CCross::RemoveLastPoint()
{
	if (m_eFrameType == FRAME_TYPE_EN::FRAME_TYPE_POLY)
	{
		if (!m_vtrPoints.isEmpty())
		{
			m_vtrPoints.removeLast();
			update();
		}
	}
}

void CCross::SetRectPoint(
	const QPoint& start, const QPoint& end, 
	const QPoint& imgStart, const QPoint& imgEnd,
	int iW, int iH, std::string &strName)
{
	m_strName.clear();
	m_strName = strName;

	MORE_RECT_ST stRect;
	stRect.rectStart = start;
	stRect.rectEnd = end;
	stRect.imageRectStart = imgStart;
	stRect.imageRectEnd = imgEnd;
	stRect.strName = strName;
	stRect.iW = iW;
	stRect.iH = iH;
	m_vtrRect.push_back(stRect);

	update();
}

void CCross::UpdateRectsForZoom(const QPixmap& scaledPixmap, const QSize& labelSize, int iWidth, int iHeight)
{
	if (FRAME_TYPE_EN::FRAME_TYPE_RECT == m_eFrameType)
	{
		MORE_RECT_VTR::iterator iter = m_vtrRect.begin();
		for (; iter != m_vtrRect.end(); iter++)
		{
			double scaleX = static_cast<double>(scaledPixmap.width()) / iter->iW;
			double scaleY = static_cast<double>(scaledPixmap.height()) / iter->iH;
			int offsetX = (labelSize.width() - scaledPixmap.width()) / 2;
			int offsetY = (labelSize.height() - scaledPixmap.height()) / 2;

			for (auto& rect : m_vtrRect)
			{
				int x1 = static_cast<int>(rect.imageRectStart.x() * scaleX + offsetX);
				int y1 = static_cast<int>(rect.imageRectStart.y() * scaleY + offsetY);
				int x2 = static_cast<int>(rect.imageRectEnd.x() * scaleX + offsetX);
				int y2 = static_cast<int>(rect.imageRectEnd.y() * scaleY + offsetY);

				rect.rectStart = QPoint(x1, y1);
				rect.rectEnd = QPoint(x2, y2);
			}
		}
	}
	
	if (FRAME_TYPE_EN::FRAME_TYPE_POLY == m_eFrameType)
	{
		m_vtrPoints.clear();
		double scaleX = static_cast<double>(scaledPixmap.width()) / iWidth;
		double scaleY = static_cast<double>(scaledPixmap.height()) / iHeight;
		int offsetX = (labelSize.width() - scaledPixmap.width()) / 2;
		int offsetY = (labelSize.height() - scaledPixmap.height()) / 2;
		for (const auto& ptImg : m_vtrImagePoints)
		{
			int x = static_cast<int>(ptImg.x() * scaleX + offsetX);
			int y = static_cast<int>(ptImg.y() * scaleY + offsetY);
			m_vtrPoints.append(QPoint(x, y));
		}
	}

	update();
}

void CCross::ClearPoints()
{
	m_vtrPoints.clear();
	m_vtrImagePoints.clear();
	update();
}

QPoint CCross::mapPointToImage(const QPoint& labelPt, int imgW, int imgH, const QPixmap& pixmap, const QSize& labelSize)
{
	double scaleX = static_cast<double>(imgW) / pixmap.width();
	double scaleY = static_cast<double>(imgH) / pixmap.height();
	int offsetX = (labelSize.width() - pixmap.width()) / 2;
	int offsetY = (labelSize.height() - pixmap.height()) / 2;
	int x = static_cast<int>((labelPt.x() - offsetX) * scaleX);
	int y = static_cast<int>((labelPt.y() - offsetY) * scaleY);
	return QPoint(x, y);
}

bool CCross::isInResizeArea(const QPoint& point)
{
	int cornerSize = 5;
	int borderWidth = 5;

	// 判断是否点击了角落
	if (isInTopLeft(point) || isInTopRight(point) || isInBottomLeft(point) || isInBottomRight(point))
		return true;

	// 判断是否点击了矩形的边中点
	if (isInTopEdge(point) || isInBottomEdge(point) || isInLeftEdge(point) || isInRightEdge(point))
		return true;

	return false;
}

bool CCross::isInTopLeft(const QPoint& point)
{
	return QRect(rect.topLeft() - QPoint(cornerSize, cornerSize), QSize(2 * cornerSize, 2 * cornerSize)).contains(point);
}

bool CCross::isInTopRight(const QPoint& point)
{
	return QRect(rect.topRight() - QPoint(cornerSize, cornerSize), QSize(2 * cornerSize, 2 * cornerSize)).contains(point);
}

bool CCross::isInBottomLeft(const QPoint& point)
{
	return QRect(rect.bottomLeft() - QPoint(cornerSize, cornerSize), QSize(2 * cornerSize, 2 * cornerSize)).contains(point);
}

bool CCross::isInBottomRight(const QPoint& point)
{
	return QRect(rect.bottomRight() - QPoint(cornerSize, cornerSize), QSize(2 * cornerSize, 2 * cornerSize)).contains(point);
}

bool CCross::isInTopEdge(const QPoint& point)
{
	return (point.x() > rect.left() && point.x() < rect.right()) &&
		(point.y() >= rect.top() - borderWidth && point.y() <= rect.top() + borderWidth);
}

bool CCross::isInBottomEdge(const QPoint& point)
{
	return (point.x() > rect.left() && point.x() < rect.right()) &&
		(point.y() >= rect.bottom() - borderWidth && point.y() <= rect.bottom() + borderWidth);
}

bool CCross::isInLeftEdge(const QPoint& point)
{
	return (point.y() > rect.top() && point.y() < rect.bottom()) &&
		(point.x() >= rect.left() - borderWidth && point.x() <= rect.left() + borderWidth);
}

bool CCross::isInRightEdge(const QPoint& point)
{
	return (point.y() > rect.top() && point.y() < rect.bottom()) &&
		(point.x() >= rect.right() - borderWidth && point.x() <= rect.right() + borderWidth);
}

void CCross::UpdateRect(qreal scaleFactorX, qreal scaleFactorY)
{
	m_bImageScale = true;

	rect.setLeft(rect.left() * scaleFactorX);
	rect.setTop(rect.top() * scaleFactorY);
	rect.setRight(rect.right() * scaleFactorX);
	rect.setBottom(rect.bottom() * scaleFactorY);

	update();
}

void CCross::UpdateRect(double dZoom)
{
	m_bImageScale = true;
	rect.setLeft(rect.left() * dZoom);
	rect.setTop(rect.top() * dZoom);
	rect.setRight(rect.right() * dZoom);
	rect.setBottom(rect.bottom() * dZoom);
	update();
}

void CCross::GetRectPoint(QPoint& rectStart, QPoint& rectEnd)
{
	rectStart = m_rectStart;
	rectEnd = m_rectEnd;
}


















