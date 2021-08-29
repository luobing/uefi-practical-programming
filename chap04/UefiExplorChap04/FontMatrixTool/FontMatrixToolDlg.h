
// FontMatrixToolDlg.h : 头文件
//

#pragma once

#include "DlgFont.h"

// CFontMatrixToolDlg 对话框
class CFontMatrixToolDlg : public CDialogEx
{
// 构造
public:
	CFontMatrixToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FontMatrixTool_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

private:
    void InitChildDlgRect();
    void InitDlgRect();
private:
    CDlgFont m_hDlgFont;

};
