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
    CPaintDC dc(this);

    CRect rect;
    GetDlgItem(IDC_SPECTRUM_VIEW)->GetClientRect(&rect);

    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    // ���Ʊ���
    memDC.FillSolidRect(&rect, RGB(255, 255, 255));

    if (m_pImageProc)
    {
        // ��ȡ��ʾ����ߴ�
        int width = rect.Width();
        int height = rect.Height();

        // ����CImageProc��Ƶ����ʾ����
        m_pImageProc->DisplayFullSpectrum(&memDC, 0, 0, width, height);
    }

    dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
        &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBitmap);
    bitmap.DeleteObject();
    memDC.DeleteDC();
}