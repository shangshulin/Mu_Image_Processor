#include "pch.h"
#include "CImageProc.h"
CImageProc::CImageProc()
{
    m_hDib = NULL;
    pDib = new BYTE;
    pBFH = new BITMAPFILEHEADER;
    pBIH = new BITMAPINFOHEADER;
    pQUAD = new RGBQUAD;
    pBits = new BYTE;
    nWidth = 0;
    nHeight = 0;
    nNumColors = 0;
}
CImageProc::~CImageProc()
{
    delete pDib;
    delete pBFH;
    delete pBIH;
    delete pQUAD;
    delete pBits;
    if (m_hDib != NULL)
        GlobalUnlock(m_hDib);
}


void CImageProc::OpenFile()
{
    // TODO: �ڴ˴����ʵ�ִ���.
    CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Bmp File(*.bmp)|*.bmp|JPG File(*.jpg)|*.jpg|All Files(*.*)|*.*||", NULL);
    if (fileDlg.DoModal() == IDOK)
    {
        CString stpathname = fileDlg.GetPathName();
        LoadBmp(stpathname);
    }
    else
        return;
}

void CImageProc::LoadBmp(CString stFileName)
{
    // TODO: �ڴ˴����ʵ�ִ���.
    CFile file;
    CFileException e;
    if (!file.Open(stFileName, CFile::modeRead | CFile::shareExclusive, &e))
    {
#ifdef _DEBUG
        afxDump << "File could not be opened " << e.m_cause << "\n";
#endif
    }
    else
    {
        long nFileSize;
        nFileSize = file.GetLength();
        m_hDib = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, nFileSize);
        pDib = (BYTE*)::GlobalLock(m_hDib);
        file.Read(pDib, nFileSize);
        pBFH = (BITMAPFILEHEADER*)pDib;
        pBIH = (BITMAPINFOHEADER*)&pDib[sizeof(BITMAPFILEHEADER)];
        pQUAD = (RGBQUAD*)&pDib[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];
        pBits = (BYTE*)&pDib[pBFH->bfOffBits];
        nWidth = pBIH->biWidth;
        nHeight = pBIH->biHeight;
        nNumColors = pBIH->biBitCount;
    }
}


void CImageProc::ShowBMP(CDC* pDC)
{
    // TODO: �ڴ˴����ʵ�ִ���.
    if (m_hDib != NULL)
    {
        ::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
        ::StretchDIBits(pDC->m_hDC, 0, 0, pBIH->biWidth, pBIH->biHeight, 0, 0, pBIH->biWidth, pBIH->biHeight, pBits, (BITMAPINFO*)pBIH, DIB_RGB_COLORS, SRCCOPY);
    }
}


void CImageProc::GetColor(CClientDC* pDC, int x, int y)
{
    if (m_hDib == NULL || x < 0 || x >= nWidth || y < 0 || y >= nHeight)
    {
        return; // ��Ч�����δ����ͼ��
    }

    // ���������� pBits �е�λ��
    int bytePerPixel = nNumColors / 8;
    if (nNumColors < 8)
        bytePerPixel = 1;

    int rowSize = ((nWidth * nNumColors + 31) / 32) * 4; // ÿ�е��ֽ���
    int offset = (nHeight - 1 - y) * rowSize + x * bytePerPixel; // ����ƫ����

    BYTE* pixel = pBits + offset;

    BYTE red = 0, green = 0, blue = 0;

    switch (nNumColors)
    {
    case 1: // 1λλͼ
    {
        BYTE mask = 0x80 >> (x % 8);
        BYTE index = (*pixel & mask) ? 1 : 0;
        red = pQUAD[index].rgbRed;
        green = pQUAD[index].rgbGreen;
        blue = pQUAD[index].rgbBlue;
        break;
    }
    case 4: // 4λλͼ
    {
        BYTE index = (x % 2 == 0) ? (*pixel >> 4) : (*pixel & 0x0F);
        red = pQUAD[index].rgbRed;
        green = pQUAD[index].rgbGreen;
        blue = pQUAD[index].rgbBlue;
        break;
    }
    case 8: // 8λλͼ
    {
        BYTE index = *pixel;
        red = pQUAD[index].rgbRed;
        green = pQUAD[index].rgbGreen;
        blue = pQUAD[index].rgbBlue;
        break;
    }
    case 16: // 16λλͼ
    {
        WORD pixelValue = *((WORD*)pixel);
        red = (pixelValue & 0x7C00) >> 10;
        green = (pixelValue & 0x03E0) >> 5;
        blue = pixelValue & 0x001F;
        red <<= 3;
        green <<= 3;
        blue <<= 3;
        break;
    }
    case 24: // 24λλͼ
    {
        red = pixel[2];
        green = pixel[1];
        blue = pixel[0];
        break;
    }
    case 32: // 32λλͼ
    {
        red = pixel[2];
        green = pixel[1];
        blue = pixel[0];
        break;
    }
    default:
        return; // ��֧�ֵ���ɫ���
    }

    // �����ı�������͸��
    pDC->SetBkMode(OPAQUE);

    // �����ı���ɫΪ��ɫ
    pDC->SetTextColor(RGB(0, 0, 0));

    // ��ʽ�� RGB ֵ��������Ϣ
    CString str;
    str.Format(L"RGB: (%d, %d, %d)", red, green, blue);

    CString location;
    location.Format(L"���꣺(%d, %d)", x, y);

    // �������λ����ʾ RGB ֵ
    pDC->TextOutW(x, y, str);

    // ��ȡ�ı��߶�
    CSize textSize = pDC->GetTextExtent(str);

    // ����һ����ʾ����
    pDC->TextOutW(x, y + textSize.cy, location);
}