#pragma once

class CSetting
{
public:
	CSetting(LPCTSTR lpszFilePath);
	virtual ~CSetting();

	bool is_valid() const { return m_bValid; }

	int GetFontSize() const { return m_hSettingData.m_nFontSize; }
	LOGFONT GetLogFont() const { return m_hSettingData.m_logFont; }
	const wchar_t* GetFileName() const { return m_hSettingData.m_strFileName; }

	bool SetLogFont(LOGFONT& logFont);
	bool SetFileName(CString strFileName);
	bool SetFontSize(int nFontSize);
private:
	void FlushFromFile();
	bool FlushToFile();

private:
	CString m_strFilePath;
	typedef struct _SETTING_DATA {
		LOGFONT m_logFont;
		wchar_t m_strFileName[MAX_PATH];
		int     m_nFontSize;     // 0  (16 * 19); 1 (8 * 19)
	}SETTING_DATA, *PSETTING_DATA;
	SETTING_DATA m_hSettingData;
	bool m_bValid;
};

