#include "pch.h"
#include "CSpectrumDlg.h"
#include "CImageProc.h"

IMPLEMENT_DYNAMIC(CSpectrumDlg, CDialogEx)

CSpectrumDlg::CSpectrumDlg(CWnd* pParent, CImageProc* pImageProc)
    : CDialogEx(CSpectrumDlg::IDD, pParent), m_pImageProc(pImageProc)
{
}

CSpectrumDlg::~CSpectrumDlg()
{
}

void CSpectrumDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SPECTRUM_VIEW, m_spectrumView); // ��picture control
}

BEGIN_MESSAGE_MAP(CSpectrumDlg, CDialogEx)
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CSpectrumDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    return TRUE;
}

void CSpectrumDlg::OnPaint()
{
    CDialogEx::OnPaint(); // ��֤�������

    CWnd* pWnd = GetDlgItem(IDC_SPECTRUM_VIEW);
    if (!pWnd) return;

    CRect rect;
    pWnd->GetClientRect(&rect);

    CDC* pDC = pWnd->GetDC();
    if (pDC && m_pImageProc && m_pImageProc->IsFFTPerformed()) {
        int destWidth = rect.Width();
        int destHeight = rect.Height();
        m_pImageProc->DisplayFullSpectrum(pDC, 0, 0, destWidth, destHeight);
        pWnd->ReleaseDC(pDC);
    }
    else if (pDC) {
        pDC->TextOutW(10, 10, L"�޿���Ƶ������");
        pWnd->ReleaseDC(pDC);
    }
}