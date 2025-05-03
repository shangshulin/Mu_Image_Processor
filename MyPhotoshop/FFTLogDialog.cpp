// FFTLogDialog.cpp
#include "pch.h"
#include "framework.h"
#include "MyPhotoshop.h"
#include "FFTLogDialog.h"

// CFFTLogDialog �Ի���
IMPLEMENT_DYNAMIC(CFFTLogDialog, CDialogEx)

CFFTLogDialog::CFFTLogDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_FFTLOG_DIALOG, pParent)
    , m_dLogBase(10.0)
    , m_dScaleFactor(1.0)
{
}

CFFTLogDialog::~CFFTLogDialog()
{
}

void CFFTLogDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_LOGBASE, m_dLogBase);
    DDV_MinMaxDouble(pDX, m_dLogBase, 1.1, 100.0);
    DDX_Text(pDX, IDC_EDIT_SCALEFACTOR, m_dScaleFactor);
    DDV_MinMaxDouble(pDX, m_dScaleFactor, 0.1, 10.0);
}

BEGIN_MESSAGE_MAP(CFFTLogDialog, CDialogEx)
    // ȷ��û�� ON_EN_CHANGE ��Ϣӳ�䣨����ȷʵ��Ҫ��
END_MESSAGE_MAP()

BOOL CFFTLogDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ����Ĭ��ֵ
    SetDlgItemText(IDC_EDIT_LOGBASE, _T("10.0"));
    SetDlgItemText(IDC_EDIT_SCALEFACTOR, _T("1.0"));

    return TRUE;
}