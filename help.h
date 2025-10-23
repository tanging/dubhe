#pragma once

#include <QWidget>
#include <QPixmap>
#include <thread>
#include <QFile>
#include "ui_help.h"
#include "SetStyle.h"



class CHelp : public QWidget, public CSetStyle
{
	Q_OBJECT
public:
	CHelp(QWidget* parent = nullptr);
	~CHelp();
	void InitHelp(const char *pExePath);
	void OpenText();
signals:
	void ShowText();
private:
	void SetStyle();
	void SetVal();
	void SetSig();
private slots:
	void MinBtnSlots();
	void CloseBtnSlots();
	void ShowTextSlots();
private:
	static void task(void *pUser);
private:
	QPixmap m_pixmap;
private:
	std::string m_strExePath;
	std::string m_strText;
public:
	Ui::helpUI ui;
};
