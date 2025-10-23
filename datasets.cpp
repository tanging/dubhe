#include "datasets.h"
#pragma comment(lib, "./yaml/yaml-cpp.lib")


CDataset::CDataset(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);
}

CDataset::~CDataset()
{

}

void CDataset::InitDataset()
{
	SetVal();
	SetSig();
	SetStyle();
}

void CDataset::SetSig()
{
	connect(ui.dataStartBtn, SIGNAL(clicked()), this, SLOT(StartBtnSlots()));
	connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(CloseBtnSlots()));
	connect(this, SIGNAL(SendSig(int, const char *)), this, SLOT(RecvSig(int, const char *)));
}

void CDataset::SetVal()
{
	ui.proWdt->setVisible(false);
}

void CDataset::SetStyle()
{
	CDataset::setWindowFlags(Qt::FramelessWindowHint);//无边框   
	CDataset::setAttribute(Qt::WA_TranslucentBackground);//背景透明
	CDataset::setWindowIcon(QIcon(":/dubhe/pic/xly.png"));

	CSetStyle::SktSetBtnStyle(ui.closeBtn, ":/dubhe/pic/c2.png", ":/dubhe/pic/c2.png", ":/dubhe/pic/c2.png");
}

void CDataset::CloseBtnSlots()
{
	CDataset::close();
}

void CDataset::SetDataPath(const char* pImage, const char* pLabels, const char* pDatasets, const char *pExePath, int iType, int iNcCount)
{
	ui.picPathEt->setText(pImage);
	ui.lablePathEt->setText(pLabels);
	ui.datasetPathEt->setText(pDatasets);

	m_strExePath = pExePath;
	m_iType = iType;
	m_iNc = iNcCount;
}

void CDataset::StartBtnSlots()
{
	ui.proWdt->setVisible(true);
	std::thread thread(task, this);
	thread.detach();
}

void CDataset::task(void* pUser)
{
	CDataset* pThis = static_cast<CDataset *>(pUser);
	pThis->start();
}

void CDataset::start()
{
	QString picPath = ui.picPathEt->text();
	QString labelPath = ui.lablePathEt->text();
	QString datasetPath = ui.datasetPathEt->text();
	QString ratio = ui.ratioEt->text();

	//QString ratioתfloat
	float fRatio = ratio.toFloat();

	SplitDataset(picPath.toLocal8Bit().data(), labelPath.toLocal8Bit().data(), datasetPath.toLocal8Bit().data(), fRatio);
}

void CDataset::RecvSig(int iVal, const char* pMsg)
{
	ui.msgLr->setText(pMsg);
	ui.progressBar->setValue(iVal);
}

void CDataset::SplitDataset(
	const char* input_image_folder, 
	const char* input_label_folder, 
	const char* output_folder, 
	float test_ratio)
{
	int iImageCount = GetPicFiles(input_image_folder);
	if (iImageCount <= 0)
	{
		return;
	}

	emit SendSig(10, "正在收集图片信息.......");
	Sleep(500);

	// 创建一个随机数生成器
	std::random_device rd;  // 获取一个随机设备
	std::mt19937 g(rd());    // 使用随机设备初始化生成器

	// 使用 std::shuffle 打乱 vector 中的元素顺序
	std::shuffle(m_vtrPicName.begin(), m_vtrPicName.end(), g);

	// 计算验证集大小
	int val_size = (int)(iImageCount * test_ratio);

	// 创建输出文件夹
	char train_image_folder[MAX_PATH] = { 0 }, train_label_folder[MAX_PATH] = {0};
	char val_image_folder[MAX_PATH] = {0}, val_label_folder[MAX_PATH] = { 0 };


	snprintf(train_image_folder, MAX_PATH, "%s/train", output_folder);
	snprintf(train_label_folder, MAX_PATH, "%s/train", output_folder);
	snprintf(val_image_folder, MAX_PATH, "%s/val", output_folder);
	snprintf(val_label_folder, MAX_PATH, "%s/val", output_folder);

	// 创建输出文件夹
	emit SendSig(30, "正在创建目录.......");
	Sleep(500);
	CreateDirectoryA(train_image_folder, NULL);
	CreateDirectoryA(train_label_folder, NULL);
	CreateDirectoryA(val_image_folder, NULL);
	CreateDirectoryA(val_label_folder, NULL);

	memset(train_image_folder, 0, MAX_PATH);
	memset(train_label_folder, 0, MAX_PATH);
	memset(val_image_folder, 0, MAX_PATH);
	memset(val_label_folder, 0, MAX_PATH);

	snprintf(train_image_folder, MAX_PATH, "%s/train/images", output_folder);
	snprintf(train_label_folder, MAX_PATH, "%s/train/labels", output_folder);
	snprintf(val_image_folder, MAX_PATH, "%s/val/images", output_folder);
	snprintf(val_label_folder, MAX_PATH, "%s/val/labels", output_folder);

	// 创建输出文件夹
	CreateDirectoryA(train_image_folder, NULL);
	CreateDirectoryA(train_label_folder, NULL);
	CreateDirectoryA(val_image_folder, NULL);
	CreateDirectoryA(val_label_folder, NULL);

	PIC_NAME_VTR::iterator iter = m_vtrPicName.begin();
	if (iter == m_vtrPicName.end())
	{
		return;
	}

	char image_path[MAX_PATH] = { 0 };
	char label_path[MAX_PATH] = { 0 };
	char new_image_path[MAX_PATH] = { 0 };
	char new_label_path[MAX_PATH] = { 0 };

	// 复制验证集图像和标签
	emit SendSig(50, "正在复制验证集图像和标签.......");
	Sleep(500);
	for (int i = 0; i < val_size; i++) 
	{
		memset(image_path, 0, MAX_PATH);
		memset(label_path, 0, MAX_PATH);
		memset(new_image_path, 0, MAX_PATH);

		std::string strPic = *iter;
		size_t pos = strPic.find_last_of("/\\");
		std::string strFileName = strPic.substr(pos + 1);
		strFileName = strFileName.substr(0, strFileName.find_last_of('.'));
		std::string strLabelPath = strFileName;

		snprintf(image_path, MAX_PATH, "%s/%s", input_image_folder, iter->c_str());
		snprintf(label_path, MAX_PATH, "%s/%s.txt", input_label_folder, strLabelPath.c_str());

		snprintf(new_image_path, MAX_PATH, "%s/%s", val_image_folder, iter->c_str());
		snprintf(new_label_path, MAX_PATH, "%s/%s.txt", val_label_folder, strLabelPath.c_str());

		if (FileExists(label_path) == true)
		{
			CopyFileA(image_path, new_image_path, TRUE);
			CopyFileA(label_path, new_label_path, TRUE);
		}
		else {
			printf("Warning: Label file %s not found for image %s\n", label_path, iter->c_str());
		}

		iter++;
	}

	// 复制训练集图像和标签
	emit SendSig(70, "正在复制训练集图像和标签.......");
	Sleep(500);
	iter = m_vtrPicName.begin();
	for (int i = val_size; i < iImageCount; i++)
	{
		memset(image_path, 0, MAX_PATH);
		memset(label_path, 0, MAX_PATH);
		memset(new_image_path, 0, MAX_PATH);

		std::string strPic = *iter;
		size_t pos = strPic.find_last_of("/\\");
		std::string strFileName = strPic.substr(pos + 1);
		strFileName = strFileName.substr(0, strFileName.find_last_of('.'));
		std::string strLabelPath = strFileName;

		snprintf(image_path, MAX_PATH, "%s\\%s", input_image_folder, iter->c_str());
		snprintf(label_path, MAX_PATH, "%s\\%s.txt", input_label_folder, strLabelPath.c_str());

		snprintf(new_image_path, MAX_PATH, "%s\\%s", train_image_folder, iter->c_str());
		snprintf(new_label_path, MAX_PATH, "%s\\%s.txt", train_label_folder, strLabelPath.c_str());

		if (FileExists(label_path) == true)
		{
			CopyFileA(image_path, new_image_path, TRUE);
			CopyFileA(label_path, new_label_path, TRUE);
		}
		else {
			printf("Warning: Label file %s not found for image %s\n", label_path, iter->c_str());
		}

		iter++;
	}

	emit SendSig(70, "正在生成YAML文件.......");
	Sleep(500);

	// 生成 YAML 文件
	bool bRet = CreateYamlFile(train_image_folder, val_image_folder);
	if (false == bRet)
	{
		emit SendSig(70, "划分失败，Create YAML FILE......");
		return;
	}

	bRet = CopyYamlFile();
	if (false == bRet)
	{
		emit SendSig(80, "划分失败，Copy YAML FILE......");
		return;
	}

	emit SendSig(100, "划分完成......");


}

bool CDataset::CopyYamlFile()
{
	if (0 == m_iType)
	{
		return false;
	}

	std::string strName;
	if (1 == m_iType)
	{
		strName = "yolo11.yaml";
	}

	if (2 == m_iType)
	{
		strName = "yolo11-seg.yaml";
	}

	QString qsPicPath = ui.picPathEt->text();
	std::string strDestPath = qsPicPath.toLocal8Bit().data();
	strDestPath += "/datasets/";
	strDestPath += strName;

	std::string strYolo11Yaml = m_strExePath;
	strYolo11Yaml += "\\yolo11-yaml\\";
	strYolo11Yaml += strName;

	BOOL BRet = CopyFileA(strYolo11Yaml.c_str(), strDestPath.c_str(), TRUE);
	if (FALSE == BRet)
	{
		return false;
	}


	YAML::Node config = YAML::LoadFile(strDestPath.c_str());

	// 检查是否存在 "nc" 键并修改它的值
	if (config["nc"]) 
	{
		config["nc"] = m_iNc;  // 将 nc 的值修改为真实
	}

	// 将修改后的内容保存回原文件
	std::ofstream fout(strDestPath.c_str());
	fout << config;
	fout.close();

	return true;
}

bool CDataset::CreateYamlFile(const char *pTrain, const char *pVal)
{
	QString qsPicPath = ui.picPathEt->text();
	std::string strClassPath = qsPicPath.toLocal8Bit().data();
	strClassPath += "/labels/classes.txt";

	QFile file(strClassPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}


	YAML::Node config;
	config["train"] = pTrain;
	config["val"] = pVal;

	YAML::Node names;
	//config["names"] = YAML::Node();

	// 使用 QTextStream 逐行读取文件
	int iTargetCount = 0;
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();  // 读取一行
		//config["names"].push_back(line.toLocal8Bit().data());
		names.push_back(line.toLocal8Bit().data());
		iTargetCount++;
	}
	file.close();  // 关闭文件

	config["nc"] = iTargetCount;
	config["names"] = names;

	if (0 == m_iType)
	{
		return false;
	}

	std::string strData;
	if (1 == m_iType)
	{
		strData = "data.yaml";
	}

	if (2 == m_iType)
	{
		strData = "data-seg.yaml";
	}

	std::string strYamlPath = qsPicPath.toLocal8Bit().data();
	strYamlPath += "/datasets/";
	strYamlPath += strData;

	std::ofstream fout(strYamlPath.c_str());
	fout << config;
	fout.close();

	return true;
}

bool CDataset::FileExists(const std::string& filePath)
{
	DWORD ftyp = GetFileAttributesA(filePath.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) {
		// 如果返回无效的属性，则文件不存在
		return false;
	}

	// 如果是文件，则返回 true
	if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
		return true;
	}

	// 如果是目录而不是文件
	return false;
}

int CDataset::GetPicFiles(const char* pPicDir)
{
	std::string strPicPath = pPicDir;
	strPicPath += "\\*";

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA(strPicPath.c_str(), &findFileData);  // 修改路径为Windows风格
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		perror("Failed to open directory");
		return 0;
	}

	int file_count = 0;
	do 
	{
		if ((strstr(findFileData.cFileName, ".jpg") || 
			strstr(findFileData.cFileName, ".png")) ||
			strstr(findFileData.cFileName, ".bmp"))
		{
			//files[file_count] = strdup(findFileData.cFileName); // 复制文件名
			m_vtrPicName.push_back(findFileData.cFileName);
			file_count++;
		}
	} while (FindNextFileA(hFind, &findFileData) != 0);

	FindClose(hFind);
	return file_count;
}