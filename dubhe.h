#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QKeyEvent>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPainter>
#include <QStandardItemModel>
#include <QHeaderView>

#include <iostream>
#include <fstream>
#include <string>


#include "ui_dubhe.h"
#include "item.h"
#include "cross.h"
#include "datasets.h"
#include "train.h"
#include "help.h"
#include "SetStyle.h"


struct Box {
    float x_min, y_min, x_max, y_max;
};

struct NormalizedBox {
    float x_center, y_center, width, height;
};

typedef struct TagInfo
{
    int iTargetId;
	std::string strName;
    std::string strPic;
	NormalizedBox box;
}TAG_INFO_ST;
typedef std::multimap<std::string, TAG_INFO_ST>TAG_INFO_MAP;

class CDubhe : public QMainWindow, public CSetStyle
{
    Q_OBJECT
protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
	virtual bool eventFilter(QObject* obj, QEvent* event) override;
public:
    CDubhe(QWidget *parent = nullptr);
    ~CDubhe();
	void InitDubhe(const char* pCurExePath);
private:
    void UpdateImage();
    void ZoomIn();
    void ZoomOut();
    void ReadClassess();
    void UpdateMark();
private:
    QRect mapRectToImage(const QRect& rectInLabel);
    QRect mapRectFromImage(const QRect& rectInLabel);
    NormalizedBox normalizeBox(Box box, float imageWidth, float imageHeight);
private:
    void SetVal();
    void SetSig();
	void SetStyle();
private slots:
    void CloseBtnSlots();
    void MinBtnSlots();
    void DatasetBtnSlots();
    void OpenDirBtnSlots();
    void ItemDoubleSlots();
    void SetDrawCrossSlots();
    void TrainBtnSlots();

    void HelpBtnSlots();
    void UpdateLabel(QPoint rectStart, QPoint rectEnd);
private:
	void SaveRectPoints();
    void SavePolyPoints();
    void UpdateClasses(std::string strName);
    void UpdateLabels(NormalizedBox& box, int iTargetName);
    void SetLabelsTabHeader();
    void UpdateLabelsInfo(const char* pstAxle, const char* pstSid);
    void ShowPic(int iIndex);
    void SetMark();
    bool CheckMark(const char *pPicPath);
    void UpdatePolyMark(std::string strLabelPath);
    void UpdateRectMark(std::string strLabelPath);
private:
	QPixmap m_pixmap;      // 当前显示的图片
    double m_dZoomFactor; // 缩放因子
    double m_dZoomFactor1; // 缩放因子
    bool m_bCtrlPressed; // 控制Ctrl键的状态
private:
    QStandardItemModel* m_pLabelsMdl;
private:
    QPointF m_lastMousePos;
    bool m_dragging;
private:
    bool m_crossMode = false;           // 是否处于十字架模式
	QPoint lastPos;

private:
    int m_iWidth;                    //原始图像宽高
    int m_iHeight;
    //目标类型
    int m_iTargetId;                //目标名称
    int m_iPicTarId;                //当前选中图片的ID
    std::string m_strCurPic;        //当前选中的是哪张图片

    std::string m_strPicDir;        //当前打开的图片目录
	std::string m_strLabelsDir;    //当前打开的标签目录
	std::string m_strDatasetsDir;  //当前打开的数据集目录
	std::string m_strResultDir;    //当前结果目录
	std::string m_strExePath;       //当前程序运行目录
private:
    TAG_INFO_MAP m_mapTarInfo;
private:
    CCross* m_pShowLr;
	CDataset m_oDataset;
    CTrain m_oTrain;
    CHelp m_oHelp;
private:
    Ui::dubheClass ui;
};
