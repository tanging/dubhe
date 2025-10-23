#ifndef SKT_SET_STYLE_H_
#define SKT_SET_STYLE_H_




#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QStyledItemDelegate>
#include <QPixmap>
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QCheckBox>
#include <QFile>
#include <QApplication>


class CSetStyle
{
public:
	CSetStyle();
	~CSetStyle();
public:
	bool SktLoadQssFile(const QString &qssFile);
	bool SktLoadRcc(const QString &rccFile);
	void SktSetRadioBtnStyle(QRadioButton *pBtn, Qt::CursorShape s, QString n, QString v, QString p);
	void SktSetBtnStyle(QPushButton *pBtn, QString n, QString v, QString p);
	void SktSetLabelStyle(QLabel *pLable, QString text, int w, int h, int x, int y, QString strFontType, int iFontSize, QString strColor);
	void SktSetLineEditStyle(QLineEdit *pLineEdit, bool isPasswd, int iLen);
	void SktSetCheckBoxStyle(QCheckBox *pCheckBox);
	void SktSetWdtBack(QWidget *pWdt, const char *pstPath);
};



#endif