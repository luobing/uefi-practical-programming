// DlgFont.cpp : 实现文件
//

#include "stdafx.h"
#include "SimpleFontTool.h"
#include "DlgFont.h"
#include "afxdialogex.h"

#include <iomanip>
#include <sstream>
using namespace std;
///////////////////////////////////////////////
CFontLattice::CFontLattice()
{
    utf_16_code = width = 0;
    p_data = 0;
}

CFontLattice::~CFontLattice()
{
    if (p_data)
    {
        free(p_data);
    }
}

CFontLattice::CFontLattice(CFontLattice&& rhs)
	: utf_16_code(rhs.utf_16_code)
	, width(rhs.width)
	, p_data(rhs.p_data)
{
	rhs.p_data = 0;
}

CFontLattice& CFontLattice::operator=(CFontLattice&& rhs)
{
	utf_16_code = rhs.utf_16_code;
	width = rhs.width;
	p_data = rhs.p_data;

	rhs.p_data = 0;
	return *this;
}
///////////////////////////////////////////////////
// CDlgFont 对话框

IMPLEMENT_DYNAMIC(CDlgFont, CDialogEx)

CDlgFont::CDlgFont(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FONT, pParent)
    , m_edit_source(_T(""))
    , m_edit_unicode(_T(""))
    , m_edit_font_name(_T(""))
    , m_edit_save_name(_T(""))
{

}

CDlgFont::~CDlgFont()
{
}

void CDlgFont::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_edit_source);
	DDX_Text(pDX, IDC_EDIT3, m_edit_unicode);
	DDX_Text(pDX, IDC_EDIT4, m_edit_font_name);
	DDX_Text(pDX, IDC_EDIT1, m_edit_save_name);
}


BEGIN_MESSAGE_MAP(CDlgFont, CDialogEx)
    ON_BN_CLICKED(IDOK, &CDlgFont::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDlgFont::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_FONT, &CDlgFont::OnBnClickedButtonFont)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgFont::OnBnClickedButtonSave)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgFont 消息处理程序

BOOL CDlgFont::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect hRect;
	GetDlgItem(IDC_STATIC_FONT_SHOW)->GetWindowRect(&hRect);
	ScreenToClient(&hRect);
	m_nShowLeft = hRect.left + 30;
	m_nShowTop = hRect.top + 30;

	m_pSetting.reset(new CSetting(L"configure.conf"));
	if (m_pSetting->is_valid())
	{
		m_logFont = m_pSetting->GetLogFont();
		m_edit_save_name = m_pSetting->GetFileName();
		m_edit_font_name = m_logFont.lfFaceName;
		SetTimer(1, 10, 0);
	}
	else
	{
		//memset(&m_logFont, 0, sizeof(m_logFont));
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDlgFont::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	switch (nIDEvent)
	{
	case 1:
		ShowFontFlush();
		SetTimer(2, 10, 0);
		break;
	case 2:
		ShowFontSample();
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CDlgFont::ShowFontFlush()
{
	CRect hRect;
	GetDlgItem(IDC_STATIC_FONT_SHOW)->GetWindowRect(&hRect);
	ScreenToClient(&hRect);
	InvalidateRect(&hRect);
}

void CDlgFont::ShowFontSample()
{
	CClientDC dc(this);

	GetCharInfo(dc, L'你', m_logFont, true);
}
void CDlgFont::OnBnClickedOk()
{
}


void CDlgFont::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    //CDialogEx::OnCancel();
}


void CDlgFont::OnBnClickedButtonFont()
{
	CWnd* pSave = GetDlgItem(IDC_BUTTON_SAVE);
	pSave->EnableWindow(false);

    CFontDialog fdlg(&m_logFont);
    if (IDOK == fdlg.DoModal())
    {
        fdlg.GetCurrentFont(&m_logFont);
        if (0 != wcslen(m_logFont.lfFaceName))
        {
			// 唯一设置宽度和高度的地方
			m_logFont.lfWidth = 8;    
			m_logFont.lfHeight = 19;
			if (m_pSetting->SetLogFont(m_logFont))
			{
				UpdateData(TRUE);
				m_edit_font_name = m_logFont.lfFaceName;
				UpdateData(FALSE);
			}
			else
			{
				MessageBox(L"保存配置信息失败。");
			}
        }
        else
        {
            MessageBox(L"字体不正确。");
        }
    }

	pSave->EnableWindow(true);
}

void CDlgFont::OnBnClickedButtonSave()
{
	UpdateData(TRUE);
	if (!m_pSetting->SetFileName(m_edit_save_name))
	{
		MessageBox(L"保存配置信息失败。");
		return;
	}

	CWnd* pFont = GetDlgItem(IDC_BUTTON_FONT);
	CWnd* pSave = GetDlgItem(IDC_BUTTON_SAVE);
	pSave->EnableWindow(false);
	pFont->EnableWindow(false);

    GetStringInfo((LPCTSTR)m_edit_source, m_logFont);


    m_edit_unicode = L"";
    for (int i = 0; i < m_edit_source.GetLength(); i++)
    {
        CString str;
        str.Format(L"\\x%04x", m_edit_source.GetAt(i));   // chenxh 2020-09-19
        m_edit_unicode += str;
    }
    UpdateData(FALSE);

	string strFilePath = ws2s((LPCTSTR)m_edit_save_name) + ".cpp";
    ofstream file(strFilePath);
	if (file.is_open())
	{
		WriteLatticeDataToCppFile(file);
	}
	else
	{
		MessageBox(wstring(L"无法创建文件：" + s2ws(strFilePath)).c_str());
	}

	pSave->EnableWindow(true);
	pFont->EnableWindow(true);
}

static const string g_strArrayName[] = {
	"EFI_WIDE_GLYPH  gSimpleFontWideGlyphData[] = {",
	"EFI_NARROW_GLYPH  gSimpleFontNarrowGlyphData[] = {"
};

static const string g_strArraySize[] = {
	"UINT32 gSimpleFontWideBytes=  sizeof (gSimpleFontWideGlyphData);",
	"UINT32 gSimpleFontNarrowBytes=  sizeof (gSimpleFontNarrowGlyphData);",
	"UINT32 gSimpleFontWideBytes=  0;",
	"UINT32 gSimpleFontNarrowBytes=  0;"
};

static const unsigned char g_strBit[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
int CDlgFont::WriteLatticeDataToCppFile(ofstream& file)
{
	file << g_strArrayName[0] << endl;
	if (m_mCurrentWideMap.size() > 0)
	{
		for (auto& elem : m_mCurrentWideMap)
		{
			file << StringFromFontLattice(elem.second, 0, WIDEFONT) << endl;
		}
		//robin: 全0数据结构
		SIZE size = { m_logFont.lfWidth * 2, m_logFont.lfHeight };
		file << StringFromFontLattice(GetEmptyFont(size), 0, WIDEFONT) << endl;

		file << "};" << endl;
		file << g_strArraySize[0] << endl << endl;;
	}
	else
	{
		file << " 0 ";
		file << "};" << endl;
		file << g_strArraySize[2] << endl << endl;
	}
	

	file << g_strArrayName[1] << endl;
	if (m_mCurrentNarrowMap.size() > 0)
	{
		for (auto& elem : m_mCurrentNarrowMap)
		{
			file << StringFromFontLattice(elem.second, 1, NARROWFONT) << endl;
		}

		//SIZE size = { m_logFont.lfWidth, m_logFont.lfHeight };
		//file << StringFromFontLattice(GetEmptyFont(size), 1, NARROWFONT) << endl;
		file << "};" << endl;
		file << g_strArraySize[1] << endl << endl;
	}
	else
	{
		file << " 0 ";
		file << "};" << endl;
		file << g_strArraySize[3] << endl << endl;
	}
	
    return 0;
}

string CDlgFont::StringFromFontLattice(const CFontLattice& elem, int nFontSize, int flagOfNarrowWide)
{
	assert((nFontSize == 0) || (nFontSize == 1));
	int nFontWidth = nFontSize ? m_logFont.lfWidth : (m_logFont.lfWidth * 2);
	int nFontHeight = m_logFont.lfHeight;
	assert(nFontWidth % 8 == 0);

	ostringstream file;
	file << "{ 0x";
	file << setw(4) << hex << setfill('0') << (unsigned int)elem.utf_16_code;  // chenxh 2020-09-19 解决排序问题
	file << ", 0x00,";

	unsigned char* pdata = elem.p_data;

	for (int k = 0; k < nFontWidth / 8; k++)
	{
		file << "{";
		for (int i = 0; i < nFontHeight; i++)
		{
			if (i != 0)
				file << ",";
			unsigned int ch = 0x00;
			for (int j = 0; j < 8; j++)
			{
				//if (pdata[i * nFontWidth + k * 8 + j] > 0)
				if (pdata[i * nFontWidth + k * 8 + 7 - j] > 0)   // chenxh 2020-09-19 解决字体问题
				{
					ch |= g_strBit[j];
				}
			}
			file << " 0x" << setw(2) << hex << setfill('0') << ch;
		}
		//file << " }, {";
		file << " }, ";
	}
	if(flagOfNarrowWide == NARROWFONT)
		file << "},";
	if(flagOfNarrowWide == WIDEFONT)
		file << " { 0x00, 0x00, 0x00 } },";

	return file.str();
}

int CDlgFont::GetStringInfo(const wchar_t* str, const LOGFONT& logFont)
{
    CClientDC dc(this);

	m_mCurrentWideMap.clear();
	m_mCurrentNarrowMap.clear();
    for (int i = 0; i < (int)wcslen(str); i++)
    {
        GetCharInfo(dc, str[i], logFont);
    }

    return 0;
}


int CDlgFont::GetCharInfo(CClientDC& dc, wchar_t character, const LOGFONT& logFont, bool bTestSample)
{
    wchar_t characterBuffer[2] = { character, 0 };

	CFont font;
	font.CreateFontIndirectW(&logFont);
	dc.SelectObject(&font);

	CRect hRect = { m_nShowLeft, m_nShowTop, m_nShowLeft + logFont.lfWidth*2, m_nShowTop + logFont.lfHeight };
	CBrush hBrush(RGB(255, 255, 255));
    dc.FillRect(&hRect, &hBrush);
    dc.TextOut(m_nShowLeft, m_nShowTop, characterBuffer, 1);

	SIZE size;
	GetTextExtentPoint(dc, characterBuffer, 1, &size);
    int nWidth = size.cx <= logFont.lfWidth+3 ? logFont.lfWidth : logFont.lfWidth * 2; // chenxh 2020-09-19 解决粗细体问题
    int nHeight = 19;
    unsigned char* pixel_buffer = (unsigned char*)malloc(nWidth * nHeight);
    if (NULL == pixel_buffer)
    {
        return -1;
    }

    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            //int value = 0xff - (GetPixel(dc, m_nShowLeft+x, m_nShowTop+y) & 0xff);
			DWORD value = GetPixel(dc, m_nShowLeft + x, m_nShowTop + y);
			if (value == 0x00ffffff)
				value = 0;
			else
				value = 1;
            pixel_buffer[y * nWidth + x] = (unsigned char)value;
        }
    }

	if (!bTestSample)
	{
		if (nWidth == logFont.lfWidth * 2)  // 2020-09-19 解决粗细体问题
		{
            if (m_mCurrentWideMap.find(character) == m_mCurrentWideMap.end())
            {
                m_mCurrentWideMap[character].utf_16_code = character;
                m_mCurrentWideMap[character].width = (unsigned char)size.cx;
                m_mCurrentWideMap[character].p_data = pixel_buffer;
            }
            else
            {
                free(pixel_buffer);
            }
		}
		else if (nWidth == logFont.lfWidth)
		{
            if (m_mCurrentNarrowMap.find(character) == m_mCurrentNarrowMap.end())
            {
                m_mCurrentNarrowMap[character].utf_16_code = character;
                m_mCurrentNarrowMap[character].width = (unsigned char)size.cx;
                m_mCurrentNarrowMap[character].p_data = pixel_buffer;
            }
            else
            {
                free(pixel_buffer);
            }
		}
        else
        {
            throw std::exception("字体大小错误。");
        }
	}
	else
	{
		free(pixel_buffer);
	}
    TRACE(_T("font name = %s, utf8 = 0x%x, width = %d, height = %d\n"), logFont.lfFaceName, character, size.cx, size.cy);
    return 0;
}

void CDlgFont::UpdateFontSize()
{
	m_logFont.lfWidth = 8;
	m_logFont.lfHeight = 19;

	SetTimer(1, 10, 0);
}

CFontLattice CDlgFont::GetEmptyFont(SIZE size)
{
	unsigned char* pixel_buffer = (unsigned char*)malloc(size.cx * size.cy);
	for (int y = 0; y < size.cy; y++)
	{
		for (int x = 0; x < size.cx; x++)
		{
			pixel_buffer[y * size.cx + x] = 0;
		}
	}

	CFontLattice hFontLattice;
	hFontLattice.utf_16_code = 0;
	hFontLattice.width = (unsigned char)size.cx;
	hFontLattice.p_data = pixel_buffer;
	
	return hFontLattice;
}
/////////////////////////////////////////////////////////////////////////////////////////
string ws2s(const wstring& ws)
{
    const wchar_t* _Source = ws.c_str();
    int _Dsize = WideCharToMultiByte(CP_ACP, 0, _Source, -1, NULL, 0, NULL, NULL);
    char *_Dest = new char[_Dsize];
    WideCharToMultiByte(CP_ACP, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
    string result = _Dest;
    delete[]_Dest;
    return result;
}

wstring s2ws(const string& s)
{
    const char* _Source = s.c_str();
    int _Dsize = MultiByteToWideChar(CP_ACP, 0, _Source, -1, NULL, 0);
    wchar_t *_Dest = new wchar_t[_Dsize];
    MultiByteToWideChar(CP_ACP, 0, _Source, -1, _Dest, _Dsize);
    wstring result = _Dest;
    delete[]_Dest;
    return result;
}



string ws2utf8(const wstring& ws)
{
    const wchar_t* _Source = ws.c_str();
    int _Dsize = WideCharToMultiByte(CP_UTF8, 0, _Source, -1, NULL, 0, NULL, NULL);
    char *_Dest = new char[_Dsize];
    WideCharToMultiByte(CP_UTF8, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
    string result = _Dest;
    delete[]_Dest;
    return result;
}

wstring utf82ws(const string& s)
{
    const char* _Source = s.c_str();
    int _Dsize = MultiByteToWideChar(CP_UTF8, 0, _Source, -1, NULL, 0);
    wchar_t *_Dest = new wchar_t[_Dsize];
    MultiByteToWideChar(CP_UTF8, 0, _Source, -1, _Dest, _Dsize);
    wstring result = _Dest;
    delete[]_Dest;
    return result;
}

