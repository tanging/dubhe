#pragma once

#include <QWidget>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <thread>
#include <QFile>
#include "ui_datasets.h"
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "SetStyle.h"


typedef std::vector<std::string>PIC_NAME_VTR;



class CDataset : public QWidget, public CSetStyle
{
	Q_OBJECT
public:
	CDataset(QWidget* parent = nullptr);
	~CDataset();
	void InitDataset();
signals:
	void SendSig(int iVal, const char* pMsg);
public:
	void SetDataPath(const char *pImage, const char *pLabels, const char *pDatasets, const char* pExePath, int iType, int iNcCout);
private:
	void SplitDataset(const char* input_image_folder, const char* input_label_folder, const char* output_folder, float test_ratio);
	int GetPicFiles(const char *pPicDir);
	bool FileExists(const std::string& filePath);
	bool CreateYamlFile(const char* pTrain, const char* pVal);
	bool CopyYamlFile();
private slots:
	void StartBtnSlots();
	void CloseBtnSlots();
	void RecvSig(int iVal, const char *pMsg);
private:
	void SetVal();
	void SetSig();
	void SetStyle();
	void start();
private:
	static void task(void *pUser);
private:
	std::string m_strExePath;
	int m_iType; 
	int m_iNc;
private:
	PIC_NAME_VTR m_vtrPicName;
public:
	Ui::datasetsUI ui;
};
