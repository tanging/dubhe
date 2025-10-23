#include "dubhe.h"
#include <QtWidgets/QApplication>

bool GetExePath(char* pstCurPath, int iBufLen)
{
	char szCurPath[MAX_PATH] = { 0 };
	DWORD dwRet = GetModuleFileNameA(NULL, szCurPath, MAX_PATH);
	if (0 == dwRet)
	{
		return false;
	}

	char* pstChr = strrchr(szCurPath, '\\');
	if (NULL == pstChr)
	{
		return false;
	}

	size_t dwPathLen = strlen(szCurPath) - strlen(pstChr);
	memcpy_s(pstCurPath, iBufLen, szCurPath, dwPathLen);
	return true;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	char JxCurExePath[MAX_PATH] = { 0 };
	bool bRet = GetExePath(JxCurExePath, MAX_PATH);

    CDubhe w;
	w.InitDubhe(JxCurExePath);
    w.show();
    return a.exec();
}
