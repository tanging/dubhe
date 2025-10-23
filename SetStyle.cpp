#include "SetStyle.h"
#include <QResource>
#include <iostream>
#include <windows.h>



CSetStyle::CSetStyle()
{

}

CSetStyle::~CSetStyle()
{

}

bool CSetStyle::SktLoadRcc(const QString &rccFile)
{
	return QResource::registerResource(rccFile);
}

bool CSetStyle::SktLoadQssFile(const QString &qssFile)
{
	QFile qss(qssFile);
	bool bRet = qss.open(QFile::ReadOnly);
	if (false == bRet)
	{
		return bRet;
	}
	qApp->setStyleSheet(qss.readAll());
	qss.close();
	return true;
}

void CSetStyle::SktSetBtnStyle(QPushButton *pBtn, QString n, QString v, QString p)
{
//	pBtn->setGeometry(x, y, w, h);
//	pBtn->setCursor(QCursor(s));

	// 按钮正常颜色
	//    strBtnStyle = "QPushButton{border:2px groove gray; \
		//            border-radius:15px; padding:2px 4px; \
//    border-image: url(" + strBtnNormal + ");}";

	QString strBtnStyle = "QPushButton{border-image: url(" + n + ");}";

	// 按钮高亮颜色
	strBtnStyle += "QPushButton:hover{border-image: url(" + v + ");}";

	// 按钮按下颜色
	strBtnStyle += "QPushButton:pressed{border-image: url(" + p + ");}";

	// 设置按钮样式
	pBtn->setStyleSheet(strBtnStyle);
}

void CSetStyle::SktSetRadioBtnStyle(QRadioButton *pBtn, Qt::CursorShape s, QString n, QString v, QString p)
{
	pBtn->setCursor(QCursor(s));

	// 按钮正常颜色
	//    strBtnStyle = "QPushButton{border:2px groove gray; \
			//            border-radius:15px; padding:2px 4px; \
//    border-image: url(" + strBtnNormal + ");}";

	QString strBtnStyle = "QRadioButton{border-image: url(" + n + ");}";

	// 按钮高亮颜色
	strBtnStyle += "QRadioButton:hover{border-image: url(" + v + ");}";

	// 按钮按下颜色
	strBtnStyle += "QRadioButton:pressed{border-image: url(" + p + ");}";

	//隐藏RADIO上面的圈圈
	strBtnStyle += "QRadioButton::indicator{width: 0px;height: 0px;}";

	// 设置按钮样式
	pBtn->setStyleSheet(strBtnStyle);

//	pBtn->setHidden(true);
}

void CSetStyle::SktSetLabelStyle(QLabel *pLable, QString text, int w, int h, int x, int y, QString strFontType, int iSize, QString strColor)
{
	//设置文本
	pLable->setText(text);
	//设置大小
	pLable->resize(w, h);
	//设置位置
	//pLable->move(x, y);
	//设置字体
	QFont font(strFontType, iSize);
	pLable->setFont(font);
	//设置颜色
	pLable->setStyleSheet("color:" + strColor);
}

void CSetStyle::SktSetLineEditStyle(QLineEdit *pLineEdit, bool isPasswd, int iLen)
{
	if (true == isPasswd)
	{
		pLineEdit->setEchoMode(QLineEdit::Password);
	}

//	pLineEdit->setStyleSheet("QLineEdit{border-width:0; border-style:outset}");
	pLineEdit->setMaxLength(iLen);

	//pLineEdit->resize(225, 40);
}

void CSetStyle::SktSetCheckBoxStyle(QCheckBox *pCheckBox)
{
	pCheckBox->setChecked(true);
	QString strCbxStyle = "QCheckBox::indicator{ border-width:40px; border-height:40px;}";
	strCbxStyle += "QCheckBox::indicator::unchecked{image: url(:/login/image/login/b1.png);}";
	strCbxStyle += "QCheckBox::indicator::checked{image: url(:/login/image/login/c1.png);}";
//	strCbxStyle += "QCheckBox::indicator::hover{image: url(./image/login/c2.png);}";

	// 设置样式
	pCheckBox->setStyleSheet(strCbxStyle);
}

void CSetStyle::SktSetWdtBack(QWidget *pWdt, const char *pstPath)
{
	pWdt->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.  
	QPalette palette = pWdt->palette();
	palette.setBrush(pWdt->backgroundRole(), QBrush(QPixmap(pstPath).scaled(pWdt->size())));
	pWdt->setPalette(palette);
}