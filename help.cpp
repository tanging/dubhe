#include "help.h"

CHelp::CHelp(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

CHelp::~CHelp()
{

}

void CHelp::InitHelp(const char *pExePath)
{
	m_strExePath = pExePath;
	SetStyle();
	SetVal();
	SetSig();
}

void CHelp::SetStyle()
{
	CHelp::setWindowFlags(Qt::FramelessWindowHint);//无边框   
	CHelp::setAttribute(Qt::WA_TranslucentBackground);//背景透明

	CHelp::SktSetBtnStyle(ui.minBtn, ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png", ":/dubhe/pic/m2.png");
	CHelp::SktSetBtnStyle(ui.maxBtn, ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png");
	CHelp::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png");

	CHelp::setWindowIcon(QIcon(":/dubhe/pic/xly.png"));
}

void CHelp::SetVal()
{
	//std::string strResult = m_strExePath;
	//strResult += "\\qq.jpg";
	//m_pixmap = QPixmap(strResult.c_str()); // 加载图片到pixmap
	//ui.helpCodeLr->setPixmap(m_pixmap);
	//ui.helpCodeLr->setAlignment(Qt::AlignCenter);
	//ui.helpCodeLr->setScaledContents(true);
}

void CHelp::SetSig()
{
	connect(this, SIGNAL(ShowText()), this, SLOT(ShowTextSlots()));

	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(CloseBtnSlots()));
	connect(ui.minBtn, SIGNAL(clicked()), this, SLOT(MinBtnSlots()));
}

void CHelp::MinBtnSlots()
{
	CHelp::showMinimized();
}

void CHelp::CloseBtnSlots()
{
	CHelp::close();
}

void CHelp::ShowTextSlots()
{
	//ui.textLr->setText(m_strText.c_str());
}

void CHelp::OpenText()
{
	std::thread thread(task, this);
	thread.detach();
}

void CHelp::task(void* pUser)
{
	CHelp* pThis = static_cast<CHelp *>(pUser);

	if (pThis->m_strExePath.empty())
	{
		return;
	}

	std::string strHelpFile = pThis->m_strExePath;
	strHelpFile += "\\help.txt";
	QFile file(strHelpFile.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Cannot open file for reading: " << qPrintable(file.errorString());
		return;
	}
	QTextStream in(&file);
	pThis->m_strText = in.readAll().toStdString();
	emit pThis->ShowText();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	file.close();

}