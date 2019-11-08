﻿
// AOEIITrainerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AOEIITrainer.h"
#include "AOEIITrainerDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include <exception>
#include "MemoryOpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAOEIITrainerDlg 对话框



CAOEIITrainerDlg::CAOEIITrainerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AOEIITRAINER_DIALOG, pParent)
	, unitHp(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAOEIITrainerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_UnitHp, unitHp);
}

DWORD CAOEIITrainerDlg::getProcId(CString gameProcName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	DWORD procId = 0;
	bool ret = Process32First(hSnapShot, &pe32);
	while (ret)
	{
		CString procName = pe32.szExeFile;
		//procName.MakeLower();
		if (gameProcName == procName)
		{
			procId = pe32.th32ProcessID;
			break;
		}
		ret = Process32Next(hSnapShot, &pe32);
	}
	CloseHandle(hSnapShot);

	return procId;
}

BEGIN_MESSAGE_MAP(CAOEIITrainerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_ModifyHp, &CAOEIITrainerDlg::OnBnClickedModifyHp)
END_MESSAGE_MAP()


// CAOEIITrainerDlg 消息处理程序

BOOL CAOEIITrainerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAOEIITrainerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAOEIITrainerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAOEIITrainerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAOEIITrainerDlg::OnBnClickedModifyHp()
{
	UpdateData(TRUE);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcId(GAME_PROC_NAME));
	int unitOffset[] = { UNIT_OFFSET1 };
	MemoryOpt memOpt(hProc, UNIT_BASE, unitOffset, 1);
	try
	{
		unitOffset[0] = HP_OFFSET;
		if (!memOpt.writeOffsetMemory(unitOffset, 1, (PVOID)unitHp, sizeof(unitHp)))
			throw text_exception("修改单位生命值失败");
	}
	catch (const std::exception & e)
	{
		CString error;
		error = e.what();
		MessageBox(error);
	}
	if (hProc != NULL)
		CloseHandle(hProc);
}

void CAOEIITrainerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcId(GAME_PROC_NAME));
	int unitOffset[] = { UNIT_OFFSET1 };
	MemoryOpt memOpt(hProc, UNIT_BASE, unitOffset, 1);
	try
	{
		unitOffset[0] = HP_OFFSET;
		if (!memOpt.readOffsetMemory(unitOffset, 1, (PVOID)unitHp, sizeof(unitHp)))
			throw text_exception("读取单位生命值失败");
	}
	catch (const std::exception & e)
	{
		CString error;
		error = e.what();
		MessageBox(error);
	}
	if (hProc != NULL)
		CloseHandle(hProc);

	UpdateData(FALSE);
}
