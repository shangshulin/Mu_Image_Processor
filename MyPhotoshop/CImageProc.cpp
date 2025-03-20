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

// ���ļ�
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

// ���� BMP �ļ�
void CImageProc::LoadBmp(CString stFileName)
{
    // TODO: �ڴ˴����ʵ�ִ���.
    CFile file;//�ļ�����
    CFileException e;//�ļ��쳣����
    if (!file.Open(stFileName, CFile::modeRead | CFile::shareExclusive, &e))
    {
#ifdef _DEBUG
        afxDump << "File could not be opened " << e.m_cause << "\n";
#endif
    }
    else
    {
        ULONGLONG nFileSize;    //ƥ��GetLength��������������
        nFileSize = file.GetLength();   //��ȡ�ļ���С
        m_hDib = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, nFileSize);   //�����ڴ�
        pDib = (BYTE*)::GlobalLock(m_hDib);   //�����ڴ�
        file.Read(pDib, nFileSize);   //��ȡ�ļ�
        pBFH = (BITMAPFILEHEADER*)pDib;     //ָ���ļ�ͷ
        pBIH = (BITMAPINFOHEADER*)&pDib[sizeof(BITMAPFILEHEADER)];   //ָ����Ϣͷ
        pQUAD = (RGBQUAD*)&pDib[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];   //ָ���ɫ��
        pBits = (BYTE*)&pDib[pBFH->bfOffBits];   //ָ��λͼ����
        nWidth = pBIH->biWidth;     // ��ȡͼ��Ŀ��
        nHeight = pBIH->biHeight;
        nNumColors = pBIH->biBitCount;   // ��ȡbmpλ���
    }
}


void CImageProc::ShowBMP(CDC* pDC)
{
    // TODO: �ڴ˴����ʵ�ִ���.
    if (m_hDib != NULL)
    {
        ::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);      // ��������ģʽΪ COLORONCOLOR
        ::StretchDIBits(pDC->m_hDC, 0, 0, pBIH->biWidth, pBIH->biHeight, 0, 0, pBIH->biWidth, pBIH->biHeight, pBits, (BITMAPINFO*)pBIH, DIB_RGB_COLORS, SRCCOPY);       // ��λͼ���ݸ��Ƶ�Ŀ��DC��ָ��λ��
    }
}

// ��ȡ������ɫ
void CImageProc::GetColor(CClientDC* pDC, int x, int y)
{
    //��������Լ�ͼ���Ƿ���Ч
    if (m_hDib == NULL || x < 0 || x >= nWidth || y < 0 || y >= nHeight)
    {
        return; // ��Ч�����δ����ͼ��
    }

    // ÿ���ֽ��� = (ÿ�е�bit�� + 31) / 32 * 4     ��ÿ���ֽ���������4�ı�������bit����32�ı���������ȡ����
    int rowSize = ((nWidth * nNumColors + 31) / 32) * 4;


    //����λ��ȼ����ÿ�����ص���ʼλ��

    float  bytePerPixel = nNumColors / 8;
    // ÿ������ռ�õ��ֽ�����nNumColors Ϊÿ�����ص�λ�������������ݵ���8bitλͼ��

    int offset = (nHeight - 1 - y) * rowSize + x * int(bytePerPixel);
    // ƫ���� = (ͼ��߶� - 1 - ������) * ÿ���ֽ��� + ������ * ÿ������ռ�õ��ֽ���   ��y�ķ�Χ��[0,nHeight-1]�� 
    // ��ǿ������ת�������ڵ���8bitͼ��pixelָ��ǰ���������ֽڵ���ʼλ�á�

    BYTE* pixel = pBits + offset;// ��ȡ������λͼ�����е�λ�ã���ʼ��+ƫ������

    //  RGB ֵ
    BYTE red = 0, green = 0, blue = 0;

    switch (nNumColors)
    {
    case 1: // 1λλͼ
    {
        CImageProc::GetColor1bit(pixel,red,green,blue,x,y,pDC);

        break;
    }
    case 2: // 2λλͼ
    {
        CImageProc::GetColor2bit(pixel, red, green, blue, x);

        break;
    }
    case 4: // 4λλͼ
    {
        CImageProc::GetColor4bit(pixel, red, green, blue, x);
        break;
    }
    case 8: // 8λλͼ
    {
        CImageProc::GetColor8bit(pixel, red, green, blue, x);
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



void CImageProc::GetColor1bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue, int x, int y, CDC* pDC)
{
    BYTE index = (*pixel >> (7 - x % 8)) & 0x01;
    red = pQUAD[index].rgbRed;
    green = pQUAD[index].rgbGreen;
    blue = pQUAD[index].rgbBlue;

    // ����ԭʼͼ��ֱ��ת����1bitͼ��֮�󣬺ڰ����ؽ���ֲ�������ȷ���Ƿ���ȷ��ʾ
    // ���´������ڲ鿴��ǰ����ֵ���Ӷ���֤�Ƿ���ʾ��ȷ

    CString str2;
    str2.Format(L"pixel��(%u);index��(%d)", *pixel, index);
    // ��ȡ�ı��߶�
    CSize textSize = pDC->GetTextExtent(str2);
    pDC->TextOutW(x, y + textSize.cy * 2, str2);
}

void CImageProc::GetColor2bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue, int x)
{
    BYTE index = (*pixel >> (7 - x % 4)) & 0x03;
    red = pQUAD[index].rgbRed;
    green = pQUAD[index].rgbGreen;
    blue = pQUAD[index].rgbBlue;
}

void CImageProc::GetColor4bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue, int x)
{
    BYTE index = (x % 2 == 0) ? (*pixel >> 4) : (*pixel & 0x0F);
    red = pQUAD[index].rgbRed;
    green = pQUAD[index].rgbGreen;
    blue = pQUAD[index].rgbBlue;
}

void CImageProc::GetColor8bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue, int x)
{
    BYTE index = *pixel;
    red = pQUAD[index].rgbRed;
    green = pQUAD[index].rgbGreen;
    blue = pQUAD[index].rgbBlue;
}