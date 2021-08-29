#include "stdafx.h"
#include "Setting.h"

#include <cassert>

CSetting::CSetting(LPCTSTR lpszFilePath)
	: m_strFilePath(lpszFilePath)
	, m_bValid(false)
{
	assert((lpszFilePath != nullptr) && (wcslen(lpszFilePath) > 0));
	memset(&m_hSettingData, 0, sizeof(SETTING_DATA));
	FlushFromFile();
}


CSetting::~CSetting()
{
}


bool CSetting::SetLogFont(LOGFONT& logFont)
{
	m_hSettingData.m_logFont = logFont;
	return FlushToFile();
}

bool CSetting::SetFileName(CString strFileName)
{
	wcscpy_s(m_hSettingData.m_strFileName, MAX_PATH, (LPCTSTR)strFileName);
	return FlushToFile();
}

void CSetting::FlushFromFile()
{
	CFile hFile;
	if (hFile.Open(m_strFilePath, CFile::modeRead))
	{
		hFile.Read(&m_hSettingData, sizeof(SETTING_DATA));
		hFile.Close();
		m_bValid = true;
	}
}

bool CSetting::FlushToFile()
{
	CFile hFile;
	if (hFile.Open(m_strFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		hFile.Write(&m_hSettingData, sizeof(SETTING_DATA));
		hFile.Close();
		return true;
	}
	else
	{
		return false;
	}
}