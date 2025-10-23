#include "dubhe.h"

CDubhe::CDubhe(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_dragging = false;
	m_bCtrlPressed = false;
	m_dZoomFactor = 1.0;
	m_iTargetId = 0;

	m_pLabelsMdl = NULL;
}

CDubhe::~CDubhe()
{

}

void CDubhe::ShowPic(int iIndex)
{
	int iCount = ui.picListLwt->count();
	for (int i = 0; i < iCount; i++)
	{
		QListWidgetItem* pItem = ui.picListLwt->item(i);
		if (NULL != pItem)
		{
			CItem* pUI = static_cast<CItem*>(ui.picListLwt->itemWidget(pItem));
			pUI->ui.itemBackWdt->setStyleSheet("background: #282B34;border-radius: 10px 10px 10px 10px; ");
		}
	}

	//获取当前选中的item
	QListWidgetItem* pCurItem = ui.picListLwt->currentItem();
	if (pCurItem == nullptr)
	{
		return;
	}

	//获取下一个item
	int iCurRow = ui.picListLwt->currentRow();
	if (iCurRow + iIndex >= ui.picListLwt->count())
	{
		return;
	}

	if ((0 == iCurRow) && (-1 == iIndex))
	{
		return;
	}

	ui.picListLwt->setCurrentRow(iCurRow + iIndex);
	pCurItem = ui.picListLwt->currentItem();

	CItem* pItem = (CItem*)ui.picListLwt->itemWidget(pCurItem);
	if (pItem == nullptr)
	{
		return;
	}

	pItem->ui.itemBackWdt->setStyleSheet("background: #3D77FF;border-radius: 10px 10px 10px 10px; ");

	QString picPath = pItem->ui.picPathLr->text();
	m_pixmap = QPixmap(picPath); // 加载图片到pixmap
	m_pShowLr->setPixmap(m_pixmap);
	m_pShowLr->setAlignment(Qt::AlignCenter);

	//设置焦点为ui.showSa
	ui.showSa->setFocus();

	m_strCurPic.clear();
	m_strCurPic = picPath.toStdString();

	UpdateMark();
}

void CDubhe::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Control) {
		m_bCtrlPressed = true;
		return;
	}
	if (event->key() == Qt::Key_W) {
		m_crossMode = true;
		m_pShowLr->setDrawCross(true);
		m_pShowLr->setCursor(Qt::CrossCursor);
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_RECT);
		return;
	}
	if (event->key() == Qt::Key_S) {
		m_crossMode = false;
		m_pShowLr->SetDrawPoly(true);
		m_pShowLr->setCursor(Qt::ArrowCursor);
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_POLY);
		return;
	}

	//上一张
	if (event->key() == Qt::Key_A) {
		ShowPic(-1);
		return;
	}

	//下一张
	if (event->key() == Qt::Key_D) {
		ShowPic(1);
		return;
	}

	// 按下 Ctrl+Z 时撤销最近一个点
	if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) 
	{
		m_pShowLr->RemoveLastPoint();
		return;
	}

	if (event->key() == Qt::Key_Escape) 
	{
		m_pShowLr->ClearPoints(); // 清空所有点
		return;
	}
}

void CDubhe::keyReleaseEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Control) {
		m_bCtrlPressed = false;
		m_dZoomFactor1 = m_dZoomFactor;
		m_dZoomFactor = 1.0; // 重置缩放因子
	}
	//if (event->key() == Qt::Key_W) {
	//	m_crossMode = false;
	//	showLr->setCursor(Qt::ArrowCursor); // 恢复默认
	//}
}

void CDubhe::SavePolyPoints()
{
	QVector<QPoint> vtrPoints = m_pShowLr->GetPolyPoints();
	if (vtrPoints.size() < 3)
	{
		QMessageBox::information(this, "提示", "多边形点数必须大于等于3");
		return;
	}
	//多边形
	int iW = m_pixmap.width();
	int iH = m_pixmap.height();
	std::string strName;
	m_pShowLr->GetName(strName);

	// 构造保存路径
	std::string strLabelPath = m_strPicDir;
	strLabelPath += "/labels/";
	size_t pos = m_strCurPic.find_last_of("/\\");
	std::string strFileName = m_strCurPic.substr(pos + 1);
	strFileName = strFileName.substr(0, strFileName.find_last_of('.'));
	strLabelPath += strFileName;
	strLabelPath += ".txt";

	//不存在则创建
	std::ofstream file(strLabelPath, std::ios_base::app);
	if (!file.is_open())
	{
		QString errorMessage = QString("无法打开标签文件，错误原因：%1").arg(strerror(errno));
		QMessageBox::warning(this, "错误", errorMessage);
		return;
	}

	file << m_iTargetId;
	TAG_INFO_MAP::iterator iter = m_mapTarInfo.find(strName);
	if (iter == m_mapTarInfo.end())
	{
		TAG_INFO_ST tagInfo;
		tagInfo.iTargetId = m_iTargetId;
		tagInfo.strName = strName;
		tagInfo.strPic = m_strCurPic;
		m_mapTarInfo.insert(std::make_pair(strName, tagInfo));

		for (const QPoint& ptLabel : vtrPoints)
		{
			//计算每个点在图片中的位置,注意这不是矩形框，是一个一个的点
			// 转换为图片坐标
			QRect rectLabel(ptLabel, ptLabel); // 单点也可用 mapRectToImage
			QRect rectImg = mapRectToImage(rectLabel);
			QPoint ptImg = rectImg.topLeft();

			// 归一化
			float x_norm = static_cast<float>(ptImg.x()) / iW;
			float y_norm = static_cast<float>(ptImg.y()) / iH;
			file << " " << x_norm << " " << y_norm;
		}

		UpdateClasses(strName);
		UpdateLabelsInfo(strName.c_str(), std::to_string(m_iTargetId).c_str());
	}
	else {
		TAG_INFO_ST tagInfo;
		tagInfo.iTargetId = iter->second.iTargetId;
		tagInfo.strName = strName;
		tagInfo.strPic = m_strCurPic;
		m_mapTarInfo.insert(std::make_pair(strName, tagInfo));

		for (const QPoint& ptLabel : vtrPoints)
		{
			//计算每个点在图片中的位置,注意这不是矩形框，是一个一个的点
			// 转换为图片坐标
			QRect rectLabel(ptLabel, ptLabel); // 单点也可用 mapRectToImage
			QRect rectImg = mapRectToImage(rectLabel);
			QPoint ptImg = rectImg.topLeft();

			// 归一化
			float x_norm = static_cast<float>(ptImg.x()) / iW;
			float y_norm = static_cast<float>(ptImg.y()) / iH;
			file << " " << x_norm << " " << y_norm;
		}
	}

	file << std::endl;
	file.close();

	SetMark();
}

void CDubhe::SaveRectPoints()
{
	int iW = m_pixmap.width();
	int iH = m_pixmap.height();

	//获取矩形框在showLr中的位置
	QPoint rectStart, rectEnd;
	m_pShowLr->GetRectPoint(rectStart, rectEnd);

	QRect rectInLabel(rectStart, rectEnd); // 获取矩形区域
	QRect rectInImage = mapRectToImage(rectInLabel);// 转换为图片坐标

	Box box;
	box.x_max = rectInImage.right();
	box.y_max = rectInImage.bottom();
	box.x_min = rectInImage.left();
	box.y_min = rectInImage.top();

	NormalizedBox nbox = normalizeBox(box, iW, iH);

	std::string strName;
	m_pShowLr->GetName(strName);
	TAG_INFO_MAP::iterator iter = m_mapTarInfo.find(strName);
	if (iter == m_mapTarInfo.end())
	{
		TAG_INFO_ST tagInfo;
		tagInfo.iTargetId = m_iTargetId;
		tagInfo.strName = strName;
		tagInfo.box = nbox;
		tagInfo.strPic = m_strCurPic;
		m_mapTarInfo.insert(std::make_pair(strName, tagInfo));

		UpdateClasses(strName);
		UpdateLabels(nbox, m_iTargetId);
		UpdateLabelsInfo(strName.c_str(), std::to_string(m_iTargetId).c_str());
		m_pShowLr->SetLabelsName(strName);

		m_iPicTarId = m_iTargetId;
		m_iTargetId++;
	}
	else {
		TAG_INFO_ST tagInfo;
		tagInfo.iTargetId = iter->second.iTargetId;
		tagInfo.strName = strName;
		tagInfo.box = nbox;
		tagInfo.strPic = m_strCurPic;
		m_mapTarInfo.insert(std::make_pair(strName, tagInfo));
		m_iPicTarId = iter->second.iTargetId;
		UpdateLabels(nbox, iter->second.iTargetId);
	}

	SetMark();
}

void CDubhe::SetDrawCrossSlots()
{
	m_crossMode = false;
	m_pShowLr->setCursor(Qt::ArrowCursor);

	FRAME_TYPE_EN enType = m_pShowLr->GetFrameType();
	if (enType == FRAME_TYPE_EN::FRAME_TYPE_POLY)
	{
		SavePolyPoints();
		return;
	}

	if (enType == FRAME_TYPE_EN::FRAME_TYPE_RECT)
	{
		SaveRectPoints();
		return;
	}
}

void CDubhe::SetMark()
{
	//获取选中行
	QListWidgetItem* pCurItem = ui.picListLwt->currentItem();
	if (pCurItem == nullptr)
	{
		return;
	}
	CItem* pItem = (CItem*)ui.picListLwt->itemWidget(pCurItem);
	if (pItem == nullptr)
	{
		return;
	}
	QString picPath = pItem->ui.picPathLr->text();
	pItem->ui.markLr->setText("已标记");

	pItem->ui.markLr->setStyleSheet("QLabel{color:#03BE6A;font-size: 12px;border:none;}");
	pItem->ui.okLr->setStyleSheet("border-image: url(:/dubhe/pic/ok.png);");
	pItem->ui.markWdt->setStyleSheet("border: 1px solid #03BE6A;border-radius: 6px;");
}

void CDubhe::UpdateLabelsInfo(const char* pName, const char* pId)
{
	int iCurLine = m_pLabelsMdl->rowCount();

	m_pLabelsMdl->setItem(iCurLine, 0, new QStandardItem(QString::fromLocal8Bit(pName)));
	m_pLabelsMdl->item(iCurLine, 0)->setTextAlignment(Qt::AlignCenter);

	m_pLabelsMdl->setItem(iCurLine, 1, new QStandardItem(QString::fromLocal8Bit(pId)));
	m_pLabelsMdl->item(iCurLine, 1)->setTextAlignment(Qt::AlignCenter);

	m_pLabelsMdl->setColumnCount(2);
	m_pLabelsMdl->setHeaderData(0, Qt::Horizontal, ("标签名"));
	m_pLabelsMdl->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("ID"));
	ui.labelsLv->setModel(m_pLabelsMdl);
}

void CDubhe::SetLabelsTabHeader()
{
	if (NULL != m_pLabelsMdl)
	{
		m_pLabelsMdl->clear();
		m_pLabelsMdl = NULL;
	}

	m_pLabelsMdl = new QStandardItemModel();
	m_pLabelsMdl->setColumnCount(2);
	m_pLabelsMdl->setHeaderData(0, Qt::Horizontal, ("标签名"));
	m_pLabelsMdl->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("ID"));
	ui.labelsLv->setModel(m_pLabelsMdl);

	ui.labelsLv->horizontalHeader()->setStretchLastSection(true);
	ui.labelsLv->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.labelsLv->verticalHeader()->setDefaultSectionSize(35);
	 
	//奇数偶数行不同背景色
	ui.labelsLv->setAlternatingRowColors(true);
	//选中整行,每次只允许选中一行
	ui.labelsLv->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.labelsLv->setSelectionMode(QAbstractItemView::SingleSelection);
	//设置行不能编辑
	ui.labelsLv->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.labelsLv->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CDubhe::UpdateLabels(NormalizedBox &box, int iTargetName)
{
	//m_strCurPic;
	//iTargetName;
	//box;

	std::string strLabelPath = m_strPicDir;
	//把d:/test/pic.png转换为d:/test/pic.txt
	strLabelPath += "/labels/";

	size_t pos = m_strCurPic.find_last_of("/\\");
	std::string strFileName = m_strCurPic.substr(pos + 1);
	strFileName = strFileName.substr(0, strFileName.find_last_of('.'));

	strLabelPath += strFileName;
	strLabelPath += ".txt";
	//打开strLabelPath，不存在则创建，存在则追加内容到末尾
	std::ofstream file(strLabelPath, std::ios_base::app); 
	if (!file.is_open())
	{
		return;
	}

	char szBuf[256] = { 0 };
	snprintf(szBuf, 256, "%d %f %f %f %f", iTargetName, box.x_center, box.y_center, box.width, box.height);
	file << szBuf << std::endl;
	file.close();
}

void CDubhe::UpdateClasses(std::string strName)
{
	std::string strClassPath = m_strPicDir;
	strClassPath += "/labels/classes.txt";

	//打开strClassPath，不存在则创建，存在则追加strName到文件末尾，并换行, 用QT写
	std::ofstream file(strClassPath, std::ios::app);  // 使用std::ios::app打开文件以追加模式
	if (!file.is_open()) 
	{
		return;
	}

	// 将 strName 写入文件并换行
	file << strName << std::endl;

	// 关闭文件
	file.close();
}

NormalizedBox CDubhe::normalizeBox(Box box, float imageWidth, float imageHeight)
{
	NormalizedBox normBox;

	// 计算中心坐标
	normBox.x_center = (box.x_min + box.x_max) / 2.0f / imageWidth;
	normBox.y_center = (box.y_min + box.y_max) / 2.0f / imageHeight;

	// 计算宽度和高度
	normBox.width = (box.x_max - box.x_min) / imageWidth;
	normBox.height = (box.y_max - box.y_min) / imageHeight;

	return normBox;
}

QRect CDubhe::mapRectToImage(const QRect& rectInLabel)
{
	if (m_pixmap.isNull()) return QRect();

	QSize pixSize = m_pixmap.size();
	QSize labelSize = m_pShowLr->size();

	// 计算保持比例的缩放
	double scaleX = static_cast<double>(pixSize.width()) / (m_pShowLr->pixmap().width());
	double scaleY = static_cast<double>(pixSize.height()) / m_pShowLr->pixmap().height();

	// 计算图片在 QLabel 中的偏移（居中显示）
	int offsetX = (labelSize.width() - m_pShowLr->pixmap().width()) / 2;
	int offsetY = (labelSize.height() - m_pShowLr->pixmap().height()) / 2;

	// 转换坐标
	int x1 = static_cast<int>((rectInLabel.left() - offsetX) * scaleX);
	int y1 = static_cast<int>((rectInLabel.top() - offsetY) * scaleY);
	int x2 = static_cast<int>((rectInLabel.right() - offsetX) * scaleX);
	int y2 = static_cast<int>((rectInLabel.bottom() - offsetY) * scaleY);

	return QRect(QPoint(x1, y1), QPoint(x2, y2));
}

QRect CDubhe::mapRectFromImage(const QRect& rectInImage)
{
	if (m_pixmap.isNull()) return QRect();

	QSize pixSize = m_pixmap.size();
	QSize labelSize = m_pShowLr->size();

	// 缩放比例
	double scaleX = static_cast<double>(m_pShowLr->pixmap().width()) / pixSize.width();
	double scaleY = static_cast<double>(m_pShowLr->pixmap().height()) / pixSize.height();

	// 居中偏移
	int offsetX = (labelSize.width() - m_pShowLr->pixmap().width()) / 2;
	int offsetY = (labelSize.height() - m_pShowLr->pixmap().height()) / 2;

	int x1 = static_cast<int>(rectInImage.left() * scaleX + offsetX);
	int y1 = static_cast<int>(rectInImage.top() * scaleY + offsetY);
	int x2 = static_cast<int>(rectInImage.right() * scaleX + offsetX);
	int y2 = static_cast<int>(rectInImage.bottom() * scaleY + offsetY);

	return QRect(QPoint(x1, y1), QPoint(x2, y2));
}

void CDubhe::wheelEvent(QWheelEvent* event)
{
	if (m_bCtrlPressed)
	{
		// 获取鼠标滚轮的增量
		if (event->angleDelta().y() > 0) 
		{
			ZoomIn();  // 放大
		}
		else {
			ZoomOut();  // 缩小
		}
	}
}

void CDubhe::ZoomIn()
{
	m_dZoomFactor += 0.1;  // 增加缩放因子
	if (m_dZoomFactor > 10.0)
	{  // 限制最大缩放因子
		m_dZoomFactor = 10.0;
	}
	UpdateImage();  // 更新图片显示
}

void CDubhe::ZoomOut()
{
	m_dZoomFactor -= 0.1;  // 减少缩放因子
	if (m_dZoomFactor < 0.1)
	{  // 限制最小缩放因子
		m_dZoomFactor = 0.1;
	}
	UpdateImage();  // 更新图片显示
}

void CDubhe::UpdateImage()
{
	// 限制缩放后的图片最大不超过showLr控件的大小
	QSize targetSize = m_pixmap.size() * m_dZoomFactor;
	QPixmap scaledPixmap = m_pixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	m_pShowLr->setPixmap(scaledPixmap);
	m_pShowLr->setAlignment(Qt::AlignCenter);


	//矩型框得重新放大,通过m_dZoomFactor,再计算
	m_pShowLr->UpdateRectsForZoom(scaledPixmap, m_pShowLr->size(), m_iWidth, m_iHeight);
}

void CDubhe::InitDubhe(const char *pCurExePath)
{
	m_strExePath = pCurExePath;

	SetVal();
	SetSig();
	SetStyle();
}

void CDubhe::SetSig()
{
	connect(ui.closeBtn, &QPushButton::clicked, this, &CDubhe::CloseBtnSlots);
	connect(ui.minBtn, &QPushButton::clicked, this, &CDubhe::MinBtnSlots);

	connect(ui.helpBtn, &QPushButton::clicked, this, &CDubhe::HelpBtnSlots);
	connect(ui.exitBtn, &QPushButton::clicked, this, &CDubhe::CloseBtnSlots);
	connect(ui.datasetBtn, &QPushButton::clicked, this, &CDubhe::DatasetBtnSlots);
	connect(ui.trainBtn, &QPushButton::clicked, this, &CDubhe::TrainBtnSlots);
	connect(ui.openDirBtn, &QPushButton::clicked, this, &CDubhe::OpenDirBtnSlots);
	connect(ui.picListLwt, &QListWidget::itemDoubleClicked, this, &CDubhe::ItemDoubleSlots);
	connect(m_pShowLr, &CCross::ReqDubheUI, this, &CDubhe::SetDrawCrossSlots);
	connect(m_pShowLr, SIGNAL(UpdateDubheLabel(QPoint, QPoint)), this, SLOT(UpdateLabel(QPoint, QPoint)));
}

void CDubhe::MinBtnSlots()
{
	CDubhe::showMinimized();
}

void CDubhe::CloseBtnSlots()
{
	QApplication::exit(0); // 正常退出程序
}

void CDubhe::UpdateLabel(QPoint rectStart, QPoint rectEnd)
{
	//获取矩形框在showLr中的位置
	QRect rectInLabel(rectStart, rectEnd); // 获取矩形区域
	QRect rectInImage = mapRectToImage(rectInLabel);// 转换为图片坐标
	int iW = m_pixmap.width();
	int iH = m_pixmap.height();
	Box box;
	box.x_max = rectInImage.right();
	box.y_max = rectInImage.bottom();
	box.x_min = rectInImage.left();
	box.y_min = rectInImage.top();
	NormalizedBox nbox = normalizeBox(box, iW, iH);

//	UpdateLabels(nbox, m_iPicTarId);

	std::string strLabelPath = m_strPicDir;
	//把d:/test/pic.png转换为d:/test/pic.txt
	strLabelPath += "/labels/";

	size_t pos = m_strCurPic.find_last_of("/\\");
	std::string strFileName = m_strCurPic.substr(pos + 1);
	strFileName = strFileName.substr(0, strFileName.find_last_of('.'));

	strLabelPath += strFileName;
	strLabelPath += ".txt";
	//打开strLabelPath，不存在则创建，存在则追加内容到末尾
	//std::ofstream file(strLabelPath, std::ios_base::app);  // 使用默认模式打开文件（覆盖写入）
	//if (!file.is_open())
	//{
	//	return;
	//}
	std::ofstream file(strLabelPath, std::ios::trunc);  // 使用std::ios::trunc打开文件以覆盖写入
	if (!file.is_open())
	{
		return;
	}

	char szBuf[256] = { 0 };
	snprintf(szBuf, 256, "%d %f %f %f %f", m_iPicTarId, nbox.x_center, nbox.y_center, nbox.width, nbox.height);
	file << szBuf << std::endl;
	file.close();
}

void CDubhe::SetVal()
{
	CDubhe::showMaximized();
	
	m_pShowLr = new CCross(this);
	m_pShowLr->setAlignment(Qt::AlignCenter);
	m_pShowLr->installEventFilter(this);

	ui.showSa->setWidget(m_pShowLr); // showSa为QScrollArea

	SetLabelsTabHeader();

	m_oDataset.InitDataset();
	m_oTrain.InitTrain(m_strExePath.c_str());
	m_oHelp.InitHelp(m_strExePath.c_str());

	ui.trainBtn->setDisabled(true);
	ui.datasetBtn->setDisabled(true);
}

void CDubhe::SetStyle()
{
	CDubhe::setWindowFlags(Qt::FramelessWindowHint);//无边框   
	CDubhe::setAttribute(Qt::WA_TranslucentBackground);//背景透明

	bool bRet = CSetStyle::SktLoadQssFile(":/dubhe/dobhe.css");

	CSetStyle::SktSetBtnStyle(ui.minBtn, ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png");
	CSetStyle::SktSetBtnStyle(ui.maxBtn, ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png");
	CSetStyle::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png");

	m_pShowLr->setStyleSheet("QLabel { background: transparent; }");
	CDubhe::setWindowIcon(QIcon(":/dubhe/pic/xly.png"));
}

bool CDubhe::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_pShowLr && m_crossMode)
	{
		if (event->type() == QEvent::MouseMove) 
		{
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
			m_pShowLr->setCrossPos(mouseEvent->pos());
		}
	}

	return QMainWindow::eventFilter(obj, event);
}

bool CDubhe::CheckMark(const char* pPicPath)
{
	//d:/data/10.png
	std::string strLabelPath = m_strLabelsDir;
	strLabelPath += "/";
	size_t pos = std::string(pPicPath).find_last_of("/\\");
	std::string strFileName = std::string(pPicPath).substr(pos + 1);
	strFileName = strFileName.substr(0, strFileName.find_last_of('.'));
	strLabelPath += strFileName;
	strLabelPath += ".txt";
	QFile file(strLabelPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	file.close();
	return true;
}

void CDubhe::OpenDirBtnSlots()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./");
	if (dir.isEmpty())
	{
		return;
	}

	ui.picPathLr->setText(dir);

	//遍历目录下的图片文件
	//ui.picListLwt->clear();
	QDir dirPath(dir);
	QString qsLabelsDir = dir;
	qsLabelsDir += "/labels";
	m_strLabelsDir.clear();
	m_strLabelsDir += qsLabelsDir.toStdString();

	ui.picListLwt->clear();
	QStringList filters;
	filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif";
	dirPath.setNameFilters(filters);
	QFileInfoList fileList = dirPath.entryInfoList(QDir::Files | QDir::NoSymLinks);
	for (int i = 0; i < fileList.size(); i++)
	{
		QFileInfo fileInfo = fileList.at(i);
		QString filePath = fileInfo.absoluteFilePath();
		CItem* pItem = new CItem(this);

		std::string strMark;
		bool bMark = CheckMark(filePath.toStdString().c_str());
		if (true == bMark)
		{
			strMark = "已标记";
		}
		else {
			strMark = "未标记";
		}

		pItem->AddPicItemList(filePath.toStdString().c_str(), strMark.c_str(), bMark);
		QListWidgetItem* pWdtItem = new QListWidgetItem(ui.picListLwt);
		//设置ITEM宽和高
		pWdtItem->setSizeHint(QSize(200, 48));
		ui.picListLwt->setItemWidget(pWdtItem, pItem);
		ui.picListLwt->setSpacing(6);
	}

	//设置选中第一行
	if (ui.picListLwt->count() > 0)
	{
		ui.picListLwt->setCurrentRow(0);
		ItemDoubleSlots();
	}

	m_strPicDir.clear();
	m_strPicDir += dir.toStdString();
	//创建目录
	QDir dirCreate;
	if (!dirCreate.exists(qsLabelsDir))
	{
		dirCreate.mkpath(qsLabelsDir);
	}


	QString qsDataSetsDir = dir;
	qsDataSetsDir += "/datasets";
	m_strDatasetsDir.clear();
	m_strDatasetsDir += qsDataSetsDir.toStdString();
	//创建目录
	if (!dirCreate.exists(qsDataSetsDir))
	{
		dirCreate.mkpath(qsDataSetsDir);
	}

	QString qsResultDir = dir;
	qsResultDir += "/result";
	m_strResultDir.clear();
	m_strResultDir += qsResultDir.toStdString();
	//创建目录
	if (!dirCreate.exists(qsResultDir))
	{
		dirCreate.mkpath(qsResultDir);
	}

	ReadClassess();
	UpdateMark();

	ui.trainBtn->setDisabled(false);
	ui.datasetBtn->setDisabled(false);
}

void CDubhe::ItemDoubleSlots()
{
	int iCount = ui.picListLwt->count();
	for (int i = 0; i < iCount; i++)
	{
		QListWidgetItem* pItem = ui.picListLwt->item(i);
		if (NULL != pItem)
		{
			CItem* pUI = static_cast<CItem*>(ui.picListLwt->itemWidget(pItem));
			pUI->ui.itemBackWdt->setStyleSheet("background: #282B34;border-radius: 10px 10px 10px 10px; ");
		}
	}


	//获取当前选中的item
	QListWidgetItem* pCurItem = ui.picListLwt->currentItem();
	if (pCurItem == nullptr)
	{
		return;
	}

	CItem* pItem = (CItem*)ui.picListLwt->itemWidget(pCurItem);
	if (pItem == nullptr)
	{
		return;
	}

	pItem->ui.itemBackWdt->setStyleSheet("background: #3D77FF;border-radius: 10px 10px 10px 10px; ");

	QString picPath = pItem->ui.picPathLr->text();
	m_pixmap = QPixmap(picPath); // 加载图片到pixmap
	m_pShowLr->setPixmap(m_pixmap);
	m_pShowLr->setAlignment(Qt::AlignCenter);
	m_iWidth = m_pixmap.width();
	m_iHeight = m_pixmap.height();

	//设置焦点为ui.showSa
	ui.showSa->setFocus();
	
	m_strCurPic.clear();
	m_strCurPic = picPath.toStdString();

	UpdateMark();
}

void CDubhe::UpdatePolyMark(std::string strLabelPath)
{
	QFile file(strLabelPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_NONE);
		m_pShowLr->update();
		return;
	}

	m_pShowLr->ClearPoints();
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();  // 读取一行
		//0 0.14242 0.320455 0.168453 0.481818 0.218989 0.625 0.269525 0.715909 0.320061
		QStringList list = line.split(" ");
		if (list.size() < 3)
		{
			continue;
		}

		int iW = m_pixmap.width();
		int iH = m_pixmap.height();

		for (int i = 0; i < list.size(); ++i) 
		{
			if (0 == i)
			{
				m_iPicTarId = list[0].toInt();
			}
			else {
				//float x_center = list[i].toFloat();
				//QRect rectInImage(QPoint(x_center, x_center), QPoint(x_center, x_center));
				//QRect rectInLabel = mapRectFromImage(rectInImage);
				//QPoint point = rectInLabel.topLeft();
				//m_pShowLr->SavePolyPoints(point);

				float x_norm, y_norm;
				x_norm = list[i].toFloat();
				y_norm = list[i + 1].toFloat();
				i++;
				int x_img = static_cast<int>(x_norm * iW);
				int y_img = static_cast<int>(y_norm * iH);

				QRect rectInImage(QPoint(x_img, y_img), QPoint(x_img, y_img));
				QRect rectInLabel = mapRectFromImage(rectInImage);
				QPoint point = rectInLabel.topLeft();
				m_pShowLr->SavePolyPoints(point, iW, iH);
			}
		}


		std::string strName;
		TAG_INFO_MAP::iterator iter = m_mapTarInfo.begin();
		for (; iter != m_mapTarInfo.end(); iter++)
		{
			if (iter->second.iTargetId == m_iPicTarId)
			{
				strName = iter->second.strName;
				break;
			}
		}
	}

	file.close();  // 关闭文件
	m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_POLY);
	m_pShowLr->update();

}

void CDubhe::UpdateRectMark(std::string strLabelPath)
{
	QFile file(strLabelPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_NONE);
		m_pShowLr->update();
		return;
	}

	//0 0.504551 0.555944 0.762029 0.876923,
	//QPoint rectStart, rectEnd;

	m_pShowLr->ClearRect();

	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();  // 读取一行
		//0 0.504551 0.555944 0.762029 0.876923,解析这个字符串
		QStringList list = line.split(" ");
		if (list.size() != 5)
		{
			continue;
		}

		m_iPicTarId = list[0].toInt();
		float x_center = list[1].toFloat();
		float y_center = list[2].toFloat();
		float width = list[3].toFloat();
		float height = list[4].toFloat();

		//QPoint rectStart, rectEnd， 计算矩形框的左上角和右下角坐标
		int iW = m_pixmap.width();
		int iH = m_pixmap.height();
		int x_min = static_cast<int>((x_center - width / 2) * iW);
		int y_min = static_cast<int>((y_center - height / 2) * iH);
		int x_max = static_cast<int>((x_center + width / 2) * iW);
		int y_max = static_cast<int>((y_center + height / 2) * iH);

		std::string strName;
		TAG_INFO_MAP::iterator iter = m_mapTarInfo.begin();
		for (; iter != m_mapTarInfo.end(); iter++)
		{
			if (iter->second.iTargetId == m_iPicTarId)
			{
				strName = iter->second.strName;
				break;
			}
		}

		QRect rectInImage(QPoint(x_min, y_min), QPoint(x_max, y_max));
		QRect rectInLabel = mapRectFromImage(rectInImage);
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_RECT);

		m_pShowLr->SetRectPoint(
			rectInLabel.topLeft(), rectInLabel.bottomRight(),
			rectInImage.topLeft(), rectInImage.bottomRight(),
			iW, iH, strName);
	}

	file.close();  // 关闭文件
}

void CDubhe::UpdateMark()
{
	std::string strLabelPath = m_strPicDir;
	//把d:/test/pic.png转换为d:/test/pic.txt
	strLabelPath += "/labels/";

	size_t pos = m_strCurPic.find_last_of("/\\");
	std::string strFileName = m_strCurPic.substr(pos + 1);
	strFileName = strFileName.substr(0, strFileName.find_last_of('.'));

	strLabelPath += strFileName;
	strLabelPath += ".txt";


	QFile file(strLabelPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_NONE);
		m_pShowLr->ClearPoints();
		m_pShowLr->update();
		return;
	}

	QTextStream in(&file);
	QString line = in.readLine();  // 读取一行
	//0 0.14242 0.320455 0.168453 0.481818 0.218989 0.625 0.269525 0.715909 0.320061
	QStringList list = line.split(" ");
	if (5 == list.size())
	{
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_RECT);
	}
	else if (list.size() > 5) {
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_POLY);
	}
	else if (list.size() < 5) {
		m_pShowLr->SetFrameType(FRAME_TYPE_EN::FRAME_TYPE_NONE);
		return;
	}

	file.close();  // 关闭文件

	FRAME_TYPE_EN enType = m_pShowLr->GetFrameType();
	if (FRAME_TYPE_EN::FRAME_TYPE_POLY == enType)
	{
		UpdatePolyMark(strLabelPath);
		return;
	}

	if (FRAME_TYPE_EN::FRAME_TYPE_RECT == enType)
	{
		UpdateRectMark(strLabelPath);
		return;
	}
}

void CDubhe::ReadClassess()
{
	m_iTargetId = 0;

	//删除ui.labelsLv里面的所有内容
	//获取行数
	int iCount = m_pLabelsMdl->rowCount();
	for (int i = 0; i < iCount; i++)
	{
		m_pLabelsMdl->removeRow(i);
	}

	m_pLabelsMdl->clear();
	m_mapTarInfo.clear();

	std::string strClassPath = m_strPicDir;
	strClassPath += "/labels/classes.txt";
	//一行一行的读取strClassPath里面的内容
	QFile file(strClassPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		qDebug() << "无法打开文件！";
		return;
	}

	// 使用 QTextStream 逐行读取文件
	QTextStream in(&file);
	while (!in.atEnd()) 
	{
		QString line = in.readLine();  // 读取一行
		m_pShowLr->SetLabelsName(line.toStdString());
		UpdateLabelsInfo(line.toStdString().c_str(), std::to_string(m_iTargetId).c_str());

		TAG_INFO_MAP::iterator iter = m_mapTarInfo.find(line.toStdString());
		if (iter == m_mapTarInfo.end())
		{
			TAG_INFO_ST tagInfo;
			tagInfo.iTargetId = m_iTargetId;
			tagInfo.strName = line.toStdString();
			tagInfo.strPic = m_strCurPic;
			m_mapTarInfo.insert(std::make_pair(line.toStdString(), tagInfo));
		}

		m_iTargetId++;
	}

	file.close();  // 关闭文件
}

void CDubhe::DatasetBtnSlots()
{
	int ix = 0;
	int iy = 0;

	m_pShowLr->GetScreenCenter(ix, iy);

	FRAME_TYPE_EN enType = m_pShowLr->GetFrameType();
	int iType = 0;
	switch (enType)
	{
	case FRAME_TYPE_EN::FRAME_TYPE_NONE:
		iType = 0;
		break;
	case FRAME_TYPE_EN::FRAME_TYPE_RECT:
		iType = 1;
		break;
	case FRAME_TYPE_EN::FRAME_TYPE_POLY:
		iType = 2;
		break;
	}

	m_oDataset.SetDataPath(
		m_strPicDir.c_str(), 
		m_strLabelsDir.c_str(), 
		m_strDatasetsDir.c_str(), 
		m_strExePath.c_str(), iType, m_iTargetId);

	m_oDataset.setWindowModality(Qt::ApplicationModal);
	m_oDataset.move(ix, iy);
	m_oDataset.show();
}

void CDubhe::TrainBtnSlots()
{
	QScreen* screen = QApplication::primaryScreen();
	if (NULL == screen)
	{
		return;
	}
	QRect screenGeometry = screen->availableGeometry();

	// 计算居中位置
	int ix = (screenGeometry.width() - m_oTrain.width()) / 2;
	int iy = (screenGeometry.height() - m_oTrain.height()) / 2;

	//qsDataSetsDir
	m_oTrain.SetDataPath(m_strDatasetsDir.c_str(), m_strResultDir.c_str());
	m_oTrain.setWindowModality(Qt::ApplicationModal);
	m_oTrain.move(ix, iy);
	m_oTrain.showMaximized();
}

void CDubhe::HelpBtnSlots()
{
	QScreen* screen = QApplication::primaryScreen();
	if (NULL == screen)
	{
		return;
	}
	QRect screenGeometry = screen->availableGeometry();

	// 计算居中位置
	int ix = (screenGeometry.width() - m_oHelp.width()) / 2;
	int iy = (screenGeometry.height() - m_oHelp.height()) / 2;

	m_oHelp.OpenText();
	m_oHelp.setWindowModality(Qt::ApplicationModal);
	m_oHelp.move(ix, iy);
	m_oHelp.showMaximized();
}