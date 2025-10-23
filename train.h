#pragma once

#include <QFile>
#include <QWidget>
#include <QProcess>
#include <string.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <QDesktopServices>

#include"ui_train.h"
#include "SetStyle.h"


using namespace std;


class CTrain : public QWidget, CSetStyle
{
	Q_OBJECT
public:
	CTrain(QWidget* parent = nullptr);
	~CTrain();
	void InitTrain(const char* pExePath);
	void SetDataPath(const char* pDataSetsPath, const char *pResultPath);
signals:
	void SetDocColor();
	void SetShowPic();
private:
	void SetSig();
	void SetVal();
	void SetStyle();
private slots:
	void MinBtnSlots();
	void CloseBtnSlots();

	void OkBtnSlots();
	void SaveBtnSlots();
	void StartBtnSlots();
	void StopBtnSlots();
	void ShowProcessOutput();
	void ScriptCbxSlots(QString qsVal);
	void UpdateDocColor();
	void OpenBtnSlots();
	void ShowPic();
private:
	void UpdateDataPath(std::string strFilePath);
private:
	static void task(void *pUser);
	static void show(void *pUser);
private:
	QPixmap m_pixmap;
	QProcess* m_pProcess;
	std::string m_strScript;
	std::string m_strText;
	std::string m_strExePath;
	std::string m_strDatasetsDir;  //当前打开的数据集目录
	std::string m_strResult;       //结果目录,训练结果
public:
	Ui::trainUI ui;
};

