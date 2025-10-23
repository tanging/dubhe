#include "item.h"


CItem::CItem(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

CItem::~CItem()
{

}

void CItem::AddPicItemList(const char* pPath, const char* pMark, bool bOk)
{
	ui.picPathLr->setText(pPath);
	ui.markLr->setText(pMark);
//	ui.okLr->setText("OK");

	if (true == bOk)
	{
		ui.markLr->setStyleSheet("QLabel{color:#03BE6A;font-size: 12px;border:none;}");
		ui.okLr->setStyleSheet("border-image: url(:/dubhe/pic/ok.png);");
		ui.markWdt->setStyleSheet("border: 1px solid #03BE6A;border-radius: 6px;");
	}
	else {
		ui.markLr->setStyleSheet("QLabel{color:#FD342D;font-size: 12px;border:none;}");
		ui.okLr->setStyleSheet("border-image: url(:/dubhe/pic/no.png);");
		ui.markWdt->setStyleSheet("border: 1px solid #FD342D;border-radius: 6px;");
	}
}