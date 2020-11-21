#include "ResourceFree.h"
#include <Windows.h>
#include <cstdio>
#include <tchar.h>
#include <strsafe.h>

void FreeRes_ShowError(LPCTSTR pszText)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)pszText) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		pszText, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}



BOOL FreeMyResource(UINT uiResouceName, LPCTSTR lpszResourceType, LPCTSTR lpszSaveFileName)
{
	HRSRC hRsrc = ::FindResource(NULL, MAKEINTRESOURCE(uiResouceName), lpszResourceType);
	if (NULL == hRsrc)
	{
		FreeRes_ShowError(TEXT("FindResource"));
		return FALSE;
	}
	DWORD dwSize = ::SizeofResource(NULL, hRsrc);
	if (0 >= dwSize)
	{
		FreeRes_ShowError(TEXT("SizeofResource"));
		return FALSE;
	}
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		FreeRes_ShowError(TEXT("LoadResource"));
		return FALSE;
	}
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		FreeRes_ShowError(TEXT("LockResource"));
		return FALSE;
	}

	FILE* fp = NULL;
	_tfopen_s(&fp, lpszSaveFileName, TEXT("wb+"));
	if (NULL == fp)
	{
		FreeRes_ShowError(lpszSaveFileName);
		return FALSE;
	}
	fwrite(lpVoid, sizeof(TCHAR), dwSize, fp);
	fclose(fp);

	return TRUE;
}


BOOL GetResourceInfo(UINT uiResouceName, TCHAR* lpszResourceType, PVOID* ppVoid, DWORD* pdwResSize)
{
	HRSRC hRsrc = ::FindResource(NULL, MAKEINTRESOURCE(uiResouceName), lpszResourceType);
	if (NULL == hRsrc)
	{
		FreeRes_ShowError(TEXT("FindResource"));
		return FALSE;
	}
	DWORD dwSize = ::SizeofResource(NULL, hRsrc);
	if (0 >= dwSize)
	{
		FreeRes_ShowError(TEXT("SizeofResource"));
		return FALSE;
	}
	HGLOBAL hGlobal = ::LoadResource(NULL, hRsrc);
	if (NULL == hGlobal)
	{
		FreeRes_ShowError(TEXT("LoadResource"));
		return FALSE;
	}
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		FreeRes_ShowError(TEXT("LockResource"));
		return FALSE;
	}

	*ppVoid = lpVoid;
	*pdwResSize = dwSize;

	return TRUE;
}


BOOL ChangeIcon(TCHAR* pszChangedIconExeFileName, TCHAR* pszSrcIconExeFileName)
{
	HMODULE hEXE = ::LoadLibrary(pszSrcIconExeFileName);
	if (NULL == hEXE)
	{
		FreeRes_ShowError(TEXT("LoadLibrary"));
		return FALSE;
	}
	HRSRC hRsrc = ::FindResource(hEXE, (LPCTSTR)1, RT_ICON);
	if (NULL == hRsrc)
	{
		FreeRes_ShowError(TEXT("FindResource"));
		return FALSE;
	}
	DWORD dwSize = ::SizeofResource(hEXE, hRsrc);
	if (0 >= dwSize)
	{
		FreeRes_ShowError(TEXT("SizeofResource"));
		return FALSE;
	}
	HGLOBAL hGlobal = ::LoadResource(hEXE, hRsrc);
	if (NULL == hGlobal)
	{
		FreeRes_ShowError(TEXT("LoadResource"));
		return FALSE;
	}
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (NULL == lpVoid)
	{
		FreeRes_ShowError(TEXT("LockResource"));
		return FALSE;
	}

	HANDLE hUpdate = ::BeginUpdateResource(pszChangedIconExeFileName, FALSE);
	if (NULL == hUpdate)
	{
		FreeRes_ShowError(TEXT("BeginUpdateResource"));
		return FALSE;
	}
	BOOL bRet = ::UpdateResource(hUpdate, RT_ICON, (LPCTSTR)1, LANG_NEUTRAL, lpVoid, dwSize);
	if (FALSE == bRet)
	{
		FreeRes_ShowError(TEXT("UpdateResource"));
		return FALSE;
	}

	::EndUpdateResource(hUpdate, FALSE);
	::FreeLibrary(hEXE);

	return TRUE;
}


BOOL ChangeExeRes(TCHAR* pszSrcFileName, TCHAR* pszInstallFileName, UINT uiDestResId, TCHAR* pszDestResType)
{
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;
	HANDLE hFile = ::CreateFile(pszInstallFileName, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		FreeRes_ShowError(TEXT("CreateFile"));
		return FALSE;
	}
	dwDataSize = ::GetFileSize(hFile, NULL);
	pData = new BYTE[dwDataSize];
	if (NULL == pData)
	{
		FreeRes_ShowError(TEXT("new"));
		return FALSE;
	}
	DWORD dwRet = 0;
	if (FALSE == ::ReadFile(hFile, pData, dwDataSize, &dwRet, NULL))
	{
		FreeRes_ShowError(TEXT("ReadFile"));
		return FALSE;
	}
	::CloseHandle(hFile);

	HANDLE hUpdate = ::BeginUpdateResource(pszSrcFileName, FALSE);
	if (NULL == hUpdate)
	{
		FreeRes_ShowError(TEXT("BeginUpdateResource"));
		return FALSE;
	}

	BOOL bRet = ::UpdateResource(hUpdate, pszDestResType, (LPCTSTR)uiDestResId, LANG_NEUTRAL, pData, dwDataSize);
	if (FALSE == bRet)
	{
		FreeRes_ShowError(TEXT("UpdateResource"));
		return FALSE;
	}

	::EndUpdateResource(hUpdate, FALSE);

	// ÊÍ·ÅÄÚ´æ
	delete[]pData;
	pData = NULL;

	return TRUE;
}
