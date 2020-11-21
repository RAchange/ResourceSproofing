#pragma once
#ifndef _RESOURCE_FREE_H_
#define _RESOURCE_FREE_H_


#include <windows.h>


BOOL FreeMyResource(UINT uiResouceName, LPCTSTR lpszResourceType, LPCTSTR lpszSaveFileName);

BOOL GetResourceInfo(UINT uiResouceName, TCHAR* lpszResourceType, PVOID* ppVoid, DWORD* pdwResSize);

BOOL ChangeIcon(TCHAR* pszChangedIconExeFileName, TCHAR* pszSrcIconExeFileName);

BOOL ChangeExeRes(TCHAR* pszSrcFileName, TCHAR* pszInstallFileName, UINT uiDestResId, TCHAR* pszDestResType);

#endif