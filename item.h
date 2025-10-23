#pragma once

#include <QWidget>
#include "ui_item.h"

class CItem : public QWidget
{
	Q_OBJECT
public:
	CItem(QWidget* parent = nullptr);
	~CItem();
public:
	void AddPicItemList(const char *pPath, const char *pMark, bool bOk);

public:
	Ui::uiItem ui;
};
