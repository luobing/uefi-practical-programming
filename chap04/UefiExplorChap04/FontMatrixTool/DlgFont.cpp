// DlgFont.cpp : 实现文件
//

#include "stdafx.h"
#include "FontMatrixTool.h"
#include "DlgFont.h"
#include "afxdialogex.h"

///////////////////////////////////////////////
CFontLattice::CFontLattice()
{
    utf_8_code = width = 0;
    p_data = 0;
}

CFontLattice::~CFontLattice()
{
    if (p_data)
    {
        free(p_data);
    }
    if (p_compressed_data)
    {
        free(p_compressed_data);
    }
}

///////////////////////////////////////////////////
// CDlgFont 对话框

IMPLEMENT_DYNAMIC(CDlgFont, CDialogEx)

CDlgFont::CDlgFont(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FONT, pParent)
    , m_edit_source(_T(""))
    , m_edit_utf8(_T(""))
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
    DDX_Text(pDX, IDC_EDIT3, m_edit_utf8);
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
	SIZE size = GetCharSize(dc, L'你', m_logFont);
	m_nCurrentFontHeight = size.cy;
	m_nMaxCurrentFontWidth = size.cx;

	GetCharInfo(dc, L'你', m_logFont, true);
}
void CDlgFont::OnBnClickedOk()
{
}


void CDlgFont::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialogEx::OnCancel();
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
			//robin dbg
			/*m_logFont.lfHeight = 19;
			m_logFont.lfWeight = 16;*/
			if (m_pSetting->SetLogFont(m_logFont))
			{
				UpdateData(TRUE);
				m_edit_font_name = m_logFont.lfFaceName;
				UpdateData(FALSE);

				SetTimer(1, 10, 0);
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


static char* include = "#include \"GuiLite.h\"\n\n";
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

    std::string utf8 = ws2utf8((LPCTSTR)m_edit_source);

    m_edit_utf8 = L"";
    for (size_t i = 0; i < utf8.size(); i++)
    {
        CString str;
        str.Format(L"\\x%02x", (unsigned char)utf8.at(i));
        m_edit_utf8 += str;
    }
    UpdateData(FALSE);

    std::fstream file(std::string("./") + ws2s((LPCTSTR)m_edit_save_name) + ".cpp", std::ios::trunc | std::ios::out | std::ios::binary);
    file.write(include, strlen(include));
    WriteLatticeDataInCppFile(file);
    file.close();

	pSave->EnableWindow(true);
	pFont->EnableWindow(true);
}

int CDlgFont::WriteLatticeDataInCppFile(std::fstream& file)
{
    if (0 == m_mCurrentFontLatticeMap.size())
    {
        return -1;
    }

	std::string data_height = "_" + (std::to_string(m_nCurrentFontHeight));

    for (auto& elem : m_mCurrentFontLatticeMap)
    {
        std::string data_type = "UINT8 ";
        std::string data_name = "_" + (std::to_string(elem.second.utf_8_code));

        std::string define_lattice_data = data_type + data_name + data_height + "[] = {\n";
        for (int i = 0; i < elem.second.p_compressed_data_length; i++)
        {
            define_lattice_data += std::to_string(elem.second.p_compressed_data[i]);
            define_lattice_data += ", ";
        }
        define_lattice_data += "};\n";
        file.write(define_lattice_data.c_str(), define_lattice_data.length());
    }

    std::string define_lattice_array = "LATTICE lattice_array[] = {\n";
    for (auto& elem : m_mCurrentFontLatticeMap)
    {
        define_lattice_array += "        {" + std::to_string(elem.second.utf_8_code) + ", " +
            std::to_string(elem.second.width) + ", " +
            "_" + (std::to_string(elem.second.utf_8_code)) + data_height + "},\n";
    }
    define_lattice_array += "};\n";
    file.write(define_lattice_array.c_str(), define_lattice_array.length());

    std::string data_type = "FONT_INFO ";
    std::string define_font_info = "extern " + data_type + ws2s((LPCTSTR)m_edit_save_name) + ";\n";
    define_font_info += data_type + ws2s((LPCTSTR)m_edit_save_name) + " ={\n";
    define_font_info += "    " + std::to_string(m_nCurrentFontHeight) + ",\n";
    define_font_info += "    " + std::to_string(m_mCurrentFontLatticeMap.size()) + ",\n";
    define_font_info += "    lattice_array\n};\n";

    file.write(define_font_info.c_str(), define_font_info.length());
    return 0;
}

//int CDlgFont::GetStringInfo(const wchar_t* str, LOGFONT& logFont)
//{
//	CClientDC dc(this);
//
//	SIZE size = GetCharSize(dc, L'你', m_logFont);
//	m_nCurrentFontHeight = size.cy;
//	m_nMaxCurrentFontWidth = size.cx;
//
//	m_mCurrentFontLatticeMap.clear();
//	int length = (int)wcslen(str);
//	for (int i = 0; i < length; i++)
//	{
//		GetCharInfo(dc, str[i], logFont);
//	}
//
//	return 0;
//}

int CDlgFont::GetStringInfo(const wchar_t* str, LOGFONT& logFont)
{

    CClientDC dc(this);

	CRect hDstRect;
	GetClientRect(&hDstRect);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);          // 创建一个兼容的DC
	CBitmap hMemBitmap;
	hMemBitmap.CreateCompatibleBitmap(&dc, hDstRect.Width(), hDstRect.Height());
	CBitmap* pOldMemBitmap = dcMem.SelectObject(&hMemBitmap);

    SIZE size = GetCharSize(dc, L'你', m_logFont);
    m_nCurrentFontHeight = size.cy;
	m_nMaxCurrentFontWidth = size.cx;

	m_mCurrentFontLatticeMap.clear();
    int length = (int)wcslen(str);
    for (int i = 0; i < length; i++)
    {
        GetCharInfo(dcMem, str[i], logFont);
		dc.BitBlt(m_nShowLeft, m_nShowTop, m_nMaxCurrentFontWidth, m_nMaxCurrentFontWidth, &dcMem, m_nShowLeft, m_nShowTop, SRCCOPY);
    }

	dcMem.SelectObject(pOldMemBitmap);
	dcMem.DeleteDC();
    return 0;
}

int CDlgFont::GetCharInfo(CDC& dc, wchar_t character, LOGFONT& logFont, bool bTestSample)
{
    wchar_t characterBuffer[2] = { character, 0 };
    CFont font;
    font.CreateFontIndirectW(&logFont);
    dc.SelectObject(&font);

	CRect hRect = { m_nShowLeft, m_nShowTop, m_nShowLeft + m_nMaxCurrentFontWidth, m_nShowTop + m_nCurrentFontHeight };
	CBrush hBrush(RGB(255, 255, 255));
    dc.FillRect(&hRect, &hBrush);
    dc.TextOut(m_nShowLeft, m_nShowTop, characterBuffer, 1);

    SIZE size;
    GetTextExtentPoint(dc, characterBuffer, 1, &size);
    unsigned char* pixel_buffer = (unsigned char*)malloc(size.cx * size.cy);
    if (NULL == pixel_buffer)
    {
        return -1;
    }

    for (int y = 0; y < size.cy; y++)
    {
        for (int x = 0; x < size.cx; x++)
        {
            int value = 0xff - (GetPixel(dc, m_nShowLeft+x, m_nShowTop+y) & 0xff);
            pixel_buffer[y * size.cx + x] = value;
        }
    }

    unsigned char utf8_buffer[32];
    memset(utf8_buffer, 0, sizeof(utf8_buffer));
    int len = WideCharToMultiByte(CP_UTF8, 0, characterBuffer, -1, (char*)utf8_buffer, sizeof(utf8_buffer), NULL, NULL);

    unsigned int utf8_code = 0;
    switch (len)
    {
    case 0:
    case 1:
        MessageBox(L"WideCharToMultiByte error!", L"✖✖✖");
        return -1;
    case 2:
        utf8_code = utf8_buffer[0];
        break;
    case 3:
        utf8_code = (utf8_buffer[0] << 8) | (utf8_buffer[1]);
        break;
    case 4:
        utf8_code = (utf8_buffer[0] << 16) | (utf8_buffer[1] << 8) | utf8_buffer[2];
        break;
    case 5:
        utf8_code = (utf8_buffer[0] << 24) | (utf8_buffer[1] << 16) | (utf8_buffer[2] << 8) | utf8_buffer[3];
        break;
    default:
        MessageBox(L"GuiLite could not support unicode over 4 bytes", L"✖✖✖");
        return -1;
    }

	if (!bTestSample)
	{
		m_mCurrentFontLatticeMap[utf8_code].utf_8_code = utf8_code;
		m_mCurrentFontLatticeMap[utf8_code].width = (unsigned char)size.cx;
		m_mCurrentFontLatticeMap[utf8_code].p_data = pixel_buffer;
		m_mCurrentFontLatticeMap[utf8_code].p_compressed_data = CompressFontLattice(pixel_buffer, size.cx * size.cy, m_mCurrentFontLatticeMap[utf8_code].p_compressed_data_length);
	}
	else
	{
		free(pixel_buffer);
	}
    TRACE(_T("font name = %s, utf8 = 0x%x, width = %d, height = %d\n"), logFont.lfFaceName, utf8_code, size.cx, size.cy);
    return 0;
}


unsigned char* CDlgFont::CompressFontLattice(unsigned char* p_data, int length, int& out_length)
{
    class CLatticeBlock
    {
    public:
        CLatticeBlock()
        {
            m_value = m_count = 0;
        }
        unsigned char m_value;
        unsigned char m_count;
    };

    std::vector<CLatticeBlock> blocks;
    CLatticeBlock cur_blk;
    cur_blk.m_value = p_data[0];
    for (int i = 0; i < length; i++)
    {
        if (cur_blk.m_value == p_data[i])
        {
            cur_blk.m_count++;
            if (0xFF == cur_blk.m_count && ((i + 1) < length) && (cur_blk.m_value == p_data[i + 1]))
            {//avoiding cur_blk.m_count overflow, add new block
                blocks.push_back(cur_blk);//truncate current block
                cur_blk.m_count = 0;//new block
            }
        }
        else
        {
            blocks.push_back(cur_blk);
            cur_blk.m_value = p_data[i];
            cur_blk.m_count = 1;
        }
    }
    if (cur_blk.m_count)
    {
        blocks.push_back(cur_blk);
    }

    out_length = (int)blocks.size() * 2;
    unsigned char* ret = (unsigned char*)malloc(out_length);
    int index = 0;
    for (auto& elem : blocks)
    {
        ret[index++] = elem.m_value;
        ret[index++] = elem.m_count;
    }
    return ret;
}

int CDlgFont::GetCompressionRatio()
{
    if (0 == m_mCurrentFontLatticeMap.size())
    {
        return 0;
    }

    int raw_data_cnt = 0;
    int compressed_data_cnt = 0;
    for (auto& elem : m_mCurrentFontLatticeMap)
    {
        raw_data_cnt += elem.second.width * m_nCurrentFontHeight;
        compressed_data_cnt += elem.second.p_compressed_data_length;
    }
    TRACE(_T("raw_data_cnt = %d, compressed_data_cnt = %d\n"), raw_data_cnt, compressed_data_cnt);
    return (100 * compressed_data_cnt / raw_data_cnt);
}

SIZE CDlgFont::GetCharSize(CDC& dc, wchar_t character, LOGFONT& logFont)
{
    wchar_t characterBuffer[2] = { character, 0 };
    CFont font;
    font.CreateFontIndirectW(&logFont);

    CFont* pOldFont = dc.SelectObject(&font);

    SIZE size;
    GetTextExtentPoint(dc, characterBuffer, 1, &size);

    dc.SelectObject(pOldFont);

    return size;
}

/////////////////////////////////////////////////////////////////////////////////////////
std::string ws2s(const std::wstring& ws)
{
    const wchar_t* _Source = ws.c_str();
    int _Dsize = WideCharToMultiByte(CP_ACP, 0, _Source, -1, NULL, 0, NULL, NULL);
    char *_Dest = new char[_Dsize];
    WideCharToMultiByte(CP_ACP, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
    std::string result = _Dest;
    delete[]_Dest;
    return result;
}

std::wstring s2ws(const std::string& s)
{
    const char* _Source = s.c_str();
    int _Dsize = MultiByteToWideChar(CP_ACP, 0, _Source, -1, NULL, 0);
    wchar_t *_Dest = new wchar_t[_Dsize];
    MultiByteToWideChar(CP_ACP, 0, _Source, -1, _Dest, _Dsize);
    std::wstring result = _Dest;
    delete[]_Dest;
    return result;
}



std::string ws2utf8(const std::wstring& ws)
{
    const wchar_t* _Source = ws.c_str();
    int _Dsize = WideCharToMultiByte(CP_UTF8, 0, _Source, -1, NULL, 0, NULL, NULL);
    char *_Dest = new char[_Dsize];
    WideCharToMultiByte(CP_UTF8, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
    std::string result = _Dest;
    delete[]_Dest;
    return result;
}

std::wstring utf82ws(const std::string& s)
{
    const char* _Source = s.c_str();
    int _Dsize = MultiByteToWideChar(CP_UTF8, 0, _Source, -1, NULL, 0);
    wchar_t *_Dest = new wchar_t[_Dsize];
    MultiByteToWideChar(CP_UTF8, 0, _Source, -1, _Dest, _Dsize);
    std::wstring result = _Dest;
    delete[]_Dest;
    return result;
}


