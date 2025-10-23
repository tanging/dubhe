#include "name.h"

CName::CName(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	m_pItem = NULL;
}

CName::~CName()
{

}

void CName::InitName()
{
	SetSig();
	SetVal();
	SetStyle();
}

void CName::SetSig()
{
	connect(ui.nOkBtn,	SIGNAL(clicked()), this, SLOT(OkBtnSlots()));
	connect(ui.nQuitBtn, SIGNAL(clicked()), this, SLOT(QuitBtnSlots()));
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(QuitBtnSlots()));

	connect(ui.nameLv, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(NameTvDoubleClickedSlots(const QModelIndex&)));
	connect(ui.nameLv, SIGNAL(clicked(const QModelIndex&)), this, SLOT(NameTvClickedSlots(const QModelIndex&)));
}

void CName::SetVal()
{
	m_pItem = new QStandardItemModel();

	//奇数偶数行不同背景色
	ui.nameLv->setAlternatingRowColors(true);
	//选中整行,每次只允许选中一行
	ui.nameLv->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.nameLv->setSelectionMode(QAbstractItemView::SingleSelection);
	//设置行不能编辑
	ui.nameLv->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.nameLv->setEditTriggers(QAbstractItemView::NoEditTriggers);

	ui.nameLv->horizontalHeader()->setStretchLastSection(true);
	ui.nameLv->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.nameLv->verticalHeader()->setDefaultSectionSize(35);

	//ui.nameLv->setSpacing(30);
	ui.nameLv->setModel(m_pItem);
	ui.nameLv->show();
}

void CName::SetStyle()
{
	CName::setWindowFlags(Qt::FramelessWindowHint);//无边框   
	CName::setAttribute(Qt::WA_TranslucentBackground);//背景透明

	CName::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c2.png", ":/dubhe/pic/c2.png", ":/dubhe/pic/c2.png");
	//CHelp::SktSetBtnStyle(ui.maxBtn, ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png", ":/dubhe/pic/m1.png");
	//CHelp::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png", ":/dubhe/pic/c1.png");

	CName::setWindowIcon(QIcon(":/dubhe/pic/xly.png"));
}

void CName::InsertLabel(std::string strName)
{
	int iCurLine = m_pItem->rowCount();

	m_pItem->setItem(iCurLine, 0, new QStandardItem(QString::fromLocal8Bit(strName)));
	m_pItem->item(iCurLine, 0)->setTextAlignment(Qt::AlignCenter);

	ui.nameEt->setText(strName.c_str());
}

void CName::OkBtnSlots()
{
	QString qsName = ui.nameEt->text();
	if (qsName.isEmpty())
	{
		return;
	}

	emit ReqCrossUI(qsName.toLocal8Bit().data());
	CName::close();
}

void CName::QuitBtnSlots()
{
	CName::close();
}

void CName::NameTvDoubleClickedSlots(const QModelIndex& index)
{
	QString qsName = m_pItem->data(m_pItem->index(index.row(), 0)).toString();
	emit ReqCrossUI(qsName.toLocal8Bit().data());
	CName::close();
}

void CName::NameTvClickedSlots(const QModelIndex& index)
{
	QString qsName = m_pItem->data(m_pItem->index(index.row(), 0)).toString();
	ui.nameEt->setText(qsName);
}