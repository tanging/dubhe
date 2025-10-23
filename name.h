#pragma once


#include <QStandardItemModel>
#include <QWidget>
#include <QHeaderView>
#include "ui_name.h"
#include "SetStyle.h"

class CName : public QWidget, public CSetStyle
{
	Q_OBJECT
public:
	CName(QWidget* parent = nullptr);
	~CName();
	void InitName();
public:
	void InsertLabel(std::string strName);
signals:
	void ReqCrossUI(const char *pName);
private:
	void SetSig();
	void SetVal();
	void SetStyle();
private slots:
	void OkBtnSlots();
	void QuitBtnSlots();
private slots:
	void NameTvDoubleClickedSlots(const QModelIndex&);
	void NameTvClickedSlots(const QModelIndex&);
private:
	QStandardItemModel* m_pItem;
public:
	Ui::nameUI ui;
};
