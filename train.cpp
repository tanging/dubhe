#include "train.h"

CTrain::CTrain(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	m_pProcess = NULL;
}

CTrain::~CTrain()
{

}

void CTrain::InitTrain(const char *pExePath)
{
	m_strExePath = pExePath;

	SetSig();
	SetVal();
	SetStyle();
}

void CTrain::SetDataPath(const char* pDataSetsPath, const char* pResultPath)
{
	m_strDatasetsDir = pDataSetsPath;
	m_strResult = pResultPath;

	QString qsVal = ui.scriptCbx->currentText();
	ScriptCbxSlots(qsVal);
}

void CTrain::SetSig()
{
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(CloseBtnSlots()));
	connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(MinBtnSlots()));

	connect(ui.openBtn, SIGNAL(clicked()), this, SLOT(OpenBtnSlots()));
	connect(ui.saveBtn, SIGNAL(clicked()), this, SLOT(SaveBtnSlots()));
	connect(ui.startBtn, SIGNAL(clicked()), this, SLOT(StartBtnSlots()));
	connect(ui.stopBtn, SIGNAL(clicked()), this, SLOT(StopBtnSlots()));
	connect(ui.scriptCbx, SIGNAL(currentTextChanged(QString)), this, SLOT(ScriptCbxSlots(QString)));
	connect(this, SIGNAL(SetDocColor()), this, SLOT(UpdateDocColor()));
	connect(this, SIGNAL(SetShowPic()), this, SLOT(ShowPic()));

	//连接m_pProcess
	m_pProcess = new QProcess(this);
	connect(m_pProcess, &QProcess::readyReadStandardOutput, this, &CTrain::ShowProcessOutput);
	connect(m_pProcess, &QProcess::readyReadStandardError, this, &CTrain::ShowProcessOutput);
}

void CTrain::MinBtnSlots()
{
	CTrain::showMinimized();
}

void CTrain::CloseBtnSlots()
{
	CTrain::close();
}

void CTrain::SetVal()
{
	ui.scriptCbx->addItem("目标检测-BASE");
	ui.scriptCbx->addItem("分割-SEG");
	ui.scriptCbx->addItem("分类-CLS");
	ui.scriptCbx->addItem("姿势估计-POSE");
	ui.scriptCbx->addItem("旋转框-OBB");

	//ui.resultWdt->setVisible(false);

	////E:\dubhe\dubhe\x64\Release\result\train15

	//std::string strResult = "E:\\dubhe\\dubhe\\x64\\Release\\result\\train15\\results.png";
	//QPixmap pixmap = QPixmap(strResult.c_str()); // 加载图片到pixmap
	//ui.picLb->setPixmap(pixmap);
	//ui.picLb->setAlignment(Qt::AlignCenter);
	////让图片自适应label大小
	//ui.picLb->setScaledContents(true);

	//ui.trainTe->setFont(QFont("Courier New")); // 设置等宽字体
	//ui.trainTe->setFontPointSize(12); // 设置字体大小
	//ui.trainTe->moveCursor(QTextCursor::Start); // 光标移动到开头
	//ui.trainTe->setStyleSheet("background-color: #f0f0f0;"); // 设置背景颜色
	//ui.trainTe->setAlignment(Qt::AlignLeft); // 设置左对齐
	//ui.trainTe->setCursorWidth(2); // 设置光标宽度
	//ui.trainTe->setCursor(Qt::IBeamCursor); // 设置光标形状
	//ui.trainTe->setLineWrapColumnOrWidth(0); // 设置换行列数或宽度为0，表示不换行
	//ui.trainTe->setTabStopDistance(4 * ui.trainTe->fontMetrics().horizontalAdvance(' ')); // 设置制表符宽度为4个空格
}

void CTrain::SetStyle()
{
	CTrain::setWindowFlags(Qt::FramelessWindowHint);//无边框   
	CTrain::setAttribute(Qt::WA_TranslucentBackground);//背景透明
	CTrain::setWindowIcon(QIcon(":/dubhe/pic/xly.png"));

	CSetStyle::SktSetBtnStyle(ui.minBtn, ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png");
	CSetStyle::SktSetBtnStyle(ui.maxBtn, ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png");
	CSetStyle::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png");
}

void CTrain::OkBtnSlots()
{

}

void CTrain::UpdateDocColor()
{
	//读取train.py里面的内容，显示在textEdit里面
	std::string strTrainFile = m_strExePath;
	strTrainFile += "\\script\\";
	strTrainFile += m_strScript;
	QFile file(strTrainFile.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Cannot open file for reading: " << qPrintable(file.errorString());
		return;
	}

	ui.trainTe->setPlainText(m_strText.c_str());

	QTextCursor cursor = ui.trainTe->document()->find("");
	// 设置格式：文字颜色为指定颜色
	QTextCharFormat format;
	format.setForeground(QColor(Qt::red));

	// 查找关键字并设置格式
	QStringList keywords = {
		"data", "optimizer", "epochs", "imgsz", "device", "batch", "workers",
		"patience", "project" };
	foreach(const QString & keyword, keywords)
	{
		cursor = ui.trainTe->document()->find(keyword);
		while (!cursor.isNull())
		{
			cursor.mergeCharFormat(format);  // 合并字符格式
			cursor = ui.trainTe->document()->find(keyword, cursor);  // 查找下一个匹配的关键字
		}
	}
}

void CTrain::ScriptCbxSlots(QString qsVal)
{
	ui.trainTe->clear();
	m_strScript.clear();
	if (qsVal == "目标检测-BASE")
	{
		m_strScript = "RectTrain.py";
	}

	if (qsVal == "分割-SEG")
	{
		m_strScript = "SegTrain.py";
	}

	std::thread thread(task, this);
	thread.detach();
}

void CTrain::task(void* pUser)
{
	CTrain* pThis = static_cast<CTrain *>(pUser);
	if (pThis->m_strExePath.empty())
	{
		return;
	}

	std::string strTrainFile = pThis->m_strExePath;
	strTrainFile += "\\script\\";
	strTrainFile += pThis->m_strScript;

	if (!pThis->m_strDatasetsDir.empty())
	{
		pThis->UpdateDataPath(strTrainFile);
	}

	QFile file(strTrainFile.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Cannot open file for reading: " << qPrintable(file.errorString());
		return;
	}

	QTextStream in(&file);
	pThis->m_strText = in.readAll().toStdString();
	emit pThis->SetDocColor();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	file.close();
}

void CTrain::UpdateDataPath(std::string strFilePath)
{
	std::string strYoloYaml;
	std::string strYoloModel;
	std::string strDataYaml;
	std::string strResult;

	std::string strExePath = m_strExePath;
	for (char& c : strExePath)
	{
		if (c == '\\') {
			c = '/';
		}
	}

	if (0 == strcmp(m_strScript.c_str(), "RectTrain.py"))
	{
		strYoloYaml = m_strDatasetsDir;
		strYoloYaml += "/yolo11.yaml";

		strYoloModel = strExePath;
		strYoloModel += "/yolo11-models/yolo11n.pt";

		strDataYaml = m_strDatasetsDir;
		strDataYaml += "/data.yaml";
	}
	if (0 == strcmp(m_strScript.c_str(), "SegTrain.py"))
	{
		strYoloYaml = m_strDatasetsDir;
		strYoloYaml += "/yolo11-seg.yaml";

		strYoloModel = strExePath;
		strYoloModel += "/yolo11-models/yolo11n-seg.pt";

		strDataYaml = m_strDatasetsDir;
		strDataYaml += "/data-seg.yaml";
	}

	strResult = m_strResult;

	std::ifstream input_file(strFilePath.c_str());
	if (!input_file) 
	{
		return;
	}

	// 临时保存修改后的内容
	std::stringstream file_content;
	std::string line;
	bool modified = false;

	// 使用正则表达式匹配 YOLO 括号内的路径
	std::regex yoloYaml(R"(YOLO\(".*"\))");  // 匹配 YOLO("...") 形式的内容
	std::regex yoloModel(R"(load\(".*"\))");  // 匹配 load="..." 形式的内容
	std::regex dataYaml(R"(data=".*")");  // 匹配 data="..." 形式的内容
	std::regex project(R"(project=".*")");  // 匹配 project="..." 形式的内容


	// 遍历文件的每一行
	while (getline(input_file, line)) 
	{
		// 查找 YOLO 括号内的路径部分
		std::smatch match;
		if (regex_search(line, match, yoloYaml))
		{
			//model = YOLO("yolo11-seg.yaml").load("yolo11n-seg.pt"),把括号内的路径YOLO里面的替换成strYoloYaml,load里面的替换成strYoloModel
			line = std::regex_replace(line, std::regex(R"(YOLO\(\"([^\"]*)\"\))"), "YOLO(\"" + strYoloYaml + "\")");
			line = std::regex_replace(line, std::regex(R"(load\(\"([^\"]*)\"\))"), "load(\"" + strYoloModel + "\")");
			file_content << line << std::endl;
			modified = true;
		}
		else if (regex_search(line, match, dataYaml))
		{
			const char* pLine = line.c_str();
			line = std::regex_replace(line, std::regex(R"(data=\"([^\"]*)\")"), "data=\"" + strDataYaml + "\"");
			pLine = line.c_str();
			line = std::regex_replace(line, std::regex(R"(project=\"([^\"]*)\")"), "project=\"" + strResult + "\"");
			pLine = line.c_str();
			file_content << line << std::endl;
			modified = true;
		}
		else {
			file_content << line << std::endl;
		}
	}

	input_file.close();

	// 如果文件内容有修改，重新写入文件
	if (modified) 
	{
		std::ofstream output_file(strFilePath.c_str());
		if (!output_file) 
		{
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		output_file << file_content.str();
		output_file.close();

		std::cout << "Path inside YOLO() has been successfully updated!" << std::endl;
	}
	else {
		std::cout << "No YOLO path found to update." << std::endl;
	}
}

void CTrain::SaveBtnSlots()
{
	QString qsVal = ui.scriptCbx->currentText();
	if (qsVal == "目标检测-BASE")
	{
		m_strScript = "RectTrain.py";
	}

	if (qsVal == "分割-SEG")
	{
		m_strScript = "SegTrain.py";
	}

	QString qsText = ui.trainTe->toPlainText();
	//删除train.py里面的内容，重新写入
	std::string strTrainFile = m_strExePath;
	strTrainFile += "\\script\\";
	strTrainFile += m_strScript;

	QFile file(strTrainFile.c_str());
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() << "Cannot open file for writing: " << qPrintable(file.errorString());
		return;
	}
	QTextStream out(&file);
	out << qsText;
	file.close();
}

void CTrain::StopBtnSlots()
{
	if (m_pProcess->state() != QProcess::NotRunning)
	{
		m_pProcess->terminate(); // 结束进程
		m_pProcess->waitForFinished(); // 等待进程结束
	}

	ui.showCmdTe->append("已停止训练......");
}

void CTrain::StartBtnSlots()
{
	ui.showCmdTe->append("开始训练......");
	//E:\yolov11-1\.venv\Scripts\python.exe E:\yolov11-1\ultralytics-8.3.143\ultralytics-8.3.143\train.py
	if (m_pProcess->state() == QProcess::NotRunning)
	{
		std::string strTrainFile = m_strExePath;
		strTrainFile += "\\script\\";
		strTrainFile += m_strScript;

		std::string strPython = m_strExePath;
		strPython += "\\pyTrain\\python.exe";
		QString program = strPython.c_str();

		//QString program = "E:/yolov11-1/.venv/Scripts/python.exe";
		QStringList arguments;
		arguments << strTrainFile.c_str();
		m_pProcess->start(program, arguments);
	}
	else
	{
		m_pProcess->terminate(); // 结束进程
		m_pProcess->waitForFinished(); // 等待进程结束
	}
}

void CTrain::ShowProcessOutput()
{
	QByteArray output = m_pProcess->readAllStandardOutput();
	QByteArray error = m_pProcess->readAllStandardError();
	if (!output.isEmpty())
	{
		ui.showCmdTe->append(QString::fromLocal8Bit(output));
	}
	if (!error.isEmpty())
	{
		ui.showCmdTe->append(QString::fromLocal8Bit(error));
	}

	const char* pData = (QString::fromLocal8Bit(output)).toStdString().c_str();
	if (NULL != strstr(pData, "Results"))
	{
		//显示图片
		//Results saved to [1mE:\dubhe\dubhe\x64\Release\result\train11[0m

		const char* pFind = strstr(pData, "[1m");
		if (NULL == pFind)
		{
			return;
		}
		const char* pEnd = strstr(pFind, "[");
		if (NULL == pEnd)
		{
			return;
		}

		std::string strPath(pFind + 3, pEnd - pFind - 3);
		std::string strResult = strPath;
		m_strResult = strPath;
		ui.resultLe->setText(strPath.c_str());
		ui.resultWdt->setVisible(true);

		//strResult += "/results.png";
		//m_pixmap = QPixmap(strResult.c_str()); // 加载图片到pixmap
		//ui.picLb->setPixmap(m_pixmap);
		//ui.picLb->setAlignment(Qt::AlignCenter);
		//ui.picLb->setScaledContents(true);

		std::thread thread(show, this);
		thread.detach();
	}
}

void CTrain::show(void* pUser)
{
	CTrain* pThis = static_cast<CTrain *>(pUser);
	std::string strResult = pThis->m_strResult;
	strResult += "/results.png";
	for (int i = 0; i < 10; i++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//检查文件是否存在
		QFile file(strResult.c_str());
		if (file.exists())
		{
			CTrain* pThis = static_cast<CTrain*>(pUser);
			emit pThis->SetShowPic();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			file.close();
			break;
		}
	}

}

void CTrain::ShowPic()
{	
	std::string strResult = m_strResult;
	strResult += "/results.png";
	m_pixmap = QPixmap(strResult.c_str()); // 加载图片到pixmap
	ui.picLb->setPixmap(m_pixmap);
	ui.picLb->setAlignment(Qt::AlignCenter);
	ui.picLb->setScaledContents(true);
}

void CTrain::OpenBtnSlots()
{
	//打开结果目录,弹出文件夹
	QString qsResultDir = ui.resultLe->text();
	if (qsResultDir.isEmpty())
	{
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(qsResultDir));

}