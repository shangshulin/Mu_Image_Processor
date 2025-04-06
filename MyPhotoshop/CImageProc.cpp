#include "pch.h"
#include "CImageProc.h"
#include <afxdlgs.h>
#include <vector>

CImageProc::CImageProc()
{
    //m_hDib = NULL;
    //pDib = new BYTE;
    //pBFH = new BITMAPFILEHEADER;
    //pBIH = new BITMAPINFOHEADER;
    //pQUAD = new RGBQUAD;
    //pBits = new BYTE;
    //nWidth = 0;
    //nHeight = 0;
    //nBitCount = 0;
    //m_bIs565Format = true; // Ĭ�ϼ���Ϊ565��ʽ
    InitializeMembers();
}
CImageProc::~CImageProc()
{
    //delete pDib;
    //delete pBFH;
    //delete pBIH;
    //delete pQUAD;
    //delete pBits;
    //if (m_hDib != NULL)
    //    GlobalUnlock(m_hDib);
    CleanUp();
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
    // ��������������
    CleanUp();

    CFile file;
    CFileException e;

    // 1. ���Դ��ļ�
    if (!file.Open(stFileName, CFile::modeRead | CFile::shareDenyWrite, &e))
    {
#ifdef _DEBUG
        afxDump << "File could not be opened " << e.m_cause << "\n";
#endif
        return;
    }

    // 2. ��ȡ�ļ���С����֤��С�ߴ�
    ULONGLONG nFileSize = file.GetLength();
    if (nFileSize < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
    {
        file.Close();
        return;
    }

    // 3. ����ȫ���ڴ�
    m_hDib = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, nFileSize);
    if (!m_hDib)
    {
        file.Close();
        return;
    }

    // 4. �����ڴ�
    pDib = (BYTE*)::GlobalLock(m_hDib);
    if (!pDib)
    {
        GlobalFree(m_hDib);
        m_hDib = NULL;
        file.Close();
        return;
    }

    // 5. ��ȡ�ļ�����
    UINT nBytesRead = file.Read(pDib, (UINT)nFileSize);
    file.Close();

    if (nBytesRead != nFileSize)
    {
        CleanUp();
        return;
    }

    // 6. �����ļ�ͷָ�벢��֤
    pBFH = (BITMAPFILEHEADER*)pDib;
    if (pBFH->bfType != 0x4D42) // 'BM'��־
    {
        CleanUp();
        return;
    }

    // 7. ������Ϣͷָ�벢��֤
    pBIH = (BITMAPINFOHEADER*)&pDib[sizeof(BITMAPFILEHEADER)];
    if (pBIH->biSize < sizeof(BITMAPINFOHEADER))
    {
        CleanUp();
        return;
    }

    // 8. ���õ�ɫ��ָ��
    pQUAD = (RGBQUAD*)&pDib[sizeof(BITMAPFILEHEADER) + pBIH->biSize];

    // 9. ��֤λͼ����ƫ����
    if (pBFH->bfOffBits >= nFileSize)
    {
        CleanUp();
        return;
    }
    pBits = &pDib[pBFH->bfOffBits];

    // 10. ���û���ͼ�����
    nWidth = pBIH->biWidth;
    nHeight = abs(pBIH->biHeight); // ������ܵĵ���λͼ
    nBitCount = pBIH->biBitCount;

    // 11. ��֤λͼ���ݴ�С
    DWORD dwImageSize = ((nWidth * nBitCount + 31) / 32) * 4 * nHeight;
    if (pBFH->bfOffBits + dwImageSize > nFileSize)
    {
        CleanUp();
        return;
    }

    // 12. ����16λλͼ��ʽ
    if (pBIH->biCompression == BI_RGB && nBitCount == 16)
    {
        m_bIs565Format = false;
    }
    else if (pBIH->biCompression == BI_BITFIELDS && nBitCount == 16)
    {
        // ȷ�����㹻�Ŀռ�����ɫ����
        if (sizeof(BITMAPFILEHEADER) + pBIH->biSize + 3 * sizeof(DWORD) <= nFileSize)
        {
            DWORD* masks = reinterpret_cast<DWORD*>(&pDib[sizeof(BITMAPFILEHEADER) + pBIH->biSize]);
            DWORD redMask = masks[0];
            DWORD greenMask = masks[1];
            DWORD blueMask = masks[2];
            m_bIs565Format = (redMask == 0xF800 && greenMask == 0x07E0 && blueMask == 0x001F);
        }
        else
        {
            m_bIs565Format = false;
        }
    }
    else
    {
        m_bIs565Format = false;
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
    int rowSize = ((nWidth * nBitCount + 31) / 32) * 4;


    //����λ��ȼ����ÿ�����ص���ʼλ��

    float  bytePerPixel = float(nBitCount)/ 8;
    // ÿ������ռ�õ��ֽ�����nNumColors Ϊÿ�����ص�λ�������������ݵ���8bitλͼ��

    int offset = (nHeight - 1 - y) * rowSize + int(float(x) * bytePerPixel);
    // ƫ���� = (ͼ��߶� - 1 - ������) * ÿ���ֽ��� + ������ * ÿ������ռ�õ��ֽ���   ��y�ķ�Χ��[0,nHeight-1]�� 
    // ��ǿ������ת�������ڵ���8bitͼ��pixelָ��ǰ���������ֽڵ���ʼλ�á�

    BYTE* pixel = pBits + offset;// ��ȡ������λͼ�����е�λ�ã���ʼ��+ƫ������

    //  RGB ֵ
    BYTE red = 0, green = 0, blue = 0;

    switch (nBitCount)
    {
    case 1: // 1λλͼ
    {
        CImageProc::GetColor1bit(pixel,red,green,blue,x,y,pDC);

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
        CImageProc::GetColor16bit(pixel, red, green, blue);
        break;
    }
    case 24: // 24λλͼ
    {
        CImageProc::GetColor24bit(pixel, red, green, blue);
        break;
    }
    case 32: // 32λλͼ
    {
        CImageProc::GetColor32bit(pixel, red, green, blue);
        break;
    }
    default:
        return; // ��֧�ֵ���ɫ���
    }

    // ʹ�� GetPixel ��ȡ������ɫ
    COLORREF pixelColor = pDC->GetPixel(x, y);
    BYTE getPixelRed = GetRValue(pixelColor);
    BYTE getPixelGreen = GetGValue(pixelColor);
    BYTE getPixelBlue = GetBValue(pixelColor);

    // �����ı�������͸��
    pDC->SetBkMode(OPAQUE);

    // �����ı���ɫΪ��ɫ
    pDC->SetTextColor(RGB(0, 0, 0));

    // ��ʽ�� RGB ֵ��������Ϣ
    CString str;
    str.Format(L"RGB: (%d, %d, %d)", red, green, blue);

    CString getPixelStr;
    getPixelStr.Format(L"GetPixel RGB: (%d, %d, %d)", getPixelRed, getPixelGreen, getPixelBlue);

    CString location;
    location.Format(L"location��(%d, %d)", x, y);

    // �������λ����ʾ RGB ֵ
    pDC->TextOutW(x, y, str);

    // ��ȡ�ı��߶�
    CSize textSize = pDC->GetTextExtent(str);

    // ����һ����ʾ GetPixel ��ȡ�� RGB ֵ
    pDC->TextOutW(x, y + textSize.cy, getPixelStr);

    // ����һ����ʾ����
    pDC->TextOutW(x, y + textSize.cy * 2, location);

}
void CImageProc::GetColor1bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue, int x, int y, CDC* pDC)
{
    BYTE index = (*pixel >> (7 - x % 8)) & 0x01;
    red = pQUAD[index].rgbRed;
    green = pQUAD[index].rgbGreen;
    blue = pQUAD[index].rgbBlue;

    // ����ԭʼͼ��ֱ��ת����1bitͼ��֮�󣬺ڰ����ؽ���ֲ�������ȷ���Ƿ���ȷ��ʾ
    // ���´������ڲ鿴��ǰ����ֵ���Ӷ���֤�Ƿ���ʾ��ȷ

    //CString str2;
    //str2.Format(L"pixel��(%u);index��(%d)", *pixel, index);
    //// ��ȡ�ı��߶�
    //CSize textSize = pDC->GetTextExtent(str2);
    //pDC->TextOutW(x, y + textSize.cy * 2, str2);
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
void CImageProc::GetColor16bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue)
{
    WORD pixelValue = *((WORD*)pixel);
    if (m_bIs565Format) {
        // ��ȡ565��ʽ��RGB����
        red = (pixelValue & 0xF800) >> 11;    
        green = (pixelValue & 0x07E0) >> 5;   
        blue = pixelValue & 0x001F;           

        // ��������չ��8λ
        red = (red << 3) | (red >> 2);        
        green = (green << 2) | (green >> 4);  
        blue = (blue << 3) | (blue >> 2);    
    } else {
        // ��ȡ555��ʽ��RGB����
        red = (pixelValue & 0x7C00) >> 10;   
        green = (pixelValue & 0x03E0) >> 5;   
        blue = pixelValue & 0x001F;           

        // ��������չ��8λ
        red = (red << 3) | (red >> 2);
        green = (green << 3) | (green >> 2);
        blue = (blue << 3) | (blue >> 2);
    }
}
void CImageProc::GetColor24bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue)
{
    red = pixel[2];
    green = pixel[1];
    blue = pixel[0];
}
void CImageProc::GetColor32bit(BYTE* pixel, BYTE& red, BYTE& green, BYTE& blue)
{
    red = pixel[2];
    green = pixel[1];
    blue = pixel[0];
}

// ����Ҷ�ֱ��ͼ
std::vector<int> CImageProc::CalculateGrayHistogram()
{
    std::vector<int> histogram(256, 0);

    if (m_hDib == NULL)
    {
        return histogram;
    }

    // ÿ���ֽ��� = (ÿ�е�bit�� + 31) / 32 * 4
    int rowSize = ((nWidth * nBitCount + 31) / 32) * 4;

    float bytePerPixel = float(nBitCount) / 8;

    for (int y = 0; y < nHeight; ++y)
    {
        for (int x = 0; x < nWidth; ++x)
        {
            int offset = (nHeight - 1 - y) * rowSize + int(float(x) * bytePerPixel);
            BYTE* pixel = pBits + offset;

            BYTE red = 0, green = 0, blue = 0;

            switch (nBitCount)
            {
            case 1:
                GetColor1bit(pixel, red, green, blue, x, y, nullptr);
                break;
            case 4:
                GetColor4bit(pixel, red, green, blue, x);
                break;
            case 8:
                GetColor8bit(pixel, red, green, blue, x);
                break;
            case 16:
                GetColor16bit(pixel, red, green, blue);
                break;
            case 24:
                GetColor24bit(pixel, red, green, blue);
                break;
            case 32:
                GetColor32bit(pixel, red, green, blue);
                break;
            default:
                continue;
            }

            // ����Ҷ�ֵ
            int gray = static_cast<int>(0.299 * red + 0.587 * green + 0.114 * blue);
            histogram[gray]++;
        }
    }

    return histogram;
}

void CImageProc::InitializeMembers()
{
    m_hDib = NULL;
    pDib = NULL;
    pBFH = NULL;
    pBIH = NULL;
    pQUAD = NULL;
    pBits = NULL;
    nWidth = nHeight = nBitCount = 0;
    m_bIs565Format = true;
}

void CImageProc::CleanUp()
{
    if (m_hDib) {
        if (pDib) {
            GlobalUnlock(m_hDib);
        }
        GlobalFree(m_hDib);
    }
    InitializeMembers();
}

// ���ŷ��Ч��
void CImageProc::ApplyVintageStyle()
{
    if (!IsValid()) return;

    if (nBitCount <= 8) {        // 1/4/8λ��ɫ��ͼ��
        ApplyVintageToPalette();
    }
    else if (nBitCount == 16) {  // 16λ�߲�ɫ
        ApplyVintageTo16Bit();
    }
    else {                       // 24/32λ���ɫ
        ApplyVintageToTrueColor();
    }
}
//���ŷ���ɫ��ͼ����(1/4/8λ)
void CImageProc::ApplyVintageToPalette()
{
    if (!pQUAD) return;

    // �������ŷ���ɫ��
    CreateVintagePalette();

    // ����1λͼ����Ҫ���⴦��
    if (nBitCount == 1) {
        // ת��Ϊ�Ҷȸ���Ч��
        for (int i = 0; i < 2; i++) {
            BYTE gray = static_cast<BYTE>(0.299 * pQUAD[i].rgbRed +
                0.587 * pQUAD[i].rgbGreen +
                0.114 * pQUAD[i].rgbBlue);
            pQUAD[i] = {
                static_cast<BYTE>(gray * 0.7),  // ƫ�ƵĻҶ�
                static_cast<BYTE>(gray * 0.6),
                static_cast<BYTE>(gray * 0.4),
                0
            };
        }
    }
}

//16λͼ����
void CImageProc::ApplyVintageTo16Bit()
{
    if (!IsValid() || nBitCount != 16) return;

    int rowSize = ((nWidth * 16 + 31) / 32) * 4;

    for (int y = 0; y < nHeight; y++) {
        BYTE* pPixel = pBits + (nHeight - 1 - y) * rowSize;

        for (int x = 0; x < nWidth; x++) {
            WORD* pixel = reinterpret_cast<WORD*>(&pPixel[x * 2]);
            WORD oldPixel = *pixel;

            // ת��Ϊ8λRGB��������չ���ȣ�
            BYTE r, g, b;
            if (m_bIs565Format) {
                // 565��ʽת����5-6-5 �� 8-8-8
                r = (oldPixel & 0xF800) >> 11 ;  // 5λ��ɫ
                g = (oldPixel & 0x07E0) >> 5;  // 6λ��ɫ
                b = oldPixel         & 0x001F;  // 5λ��ɫ
                
                // ��չ��8λ�����ֱ�����
                r = (r << 3) | (r >> 2);  // 5��8λ�����Ƹ�λ����λ
                g = (g << 2) | (g >> 4);  // 6��8λ�����Ƹ�λ����λ
                b = (b << 3) | (b >> 2);
            } else {
                // 555��ʽת����5-5-5 �� 8-8-8
                r = (oldPixel & 0x7C00) >> 10;
                g = (oldPixel & 0x03E0) >> 5;
                b = oldPixel & 0x001F;
                
                // ��չ��8λ
                r = (r << 3) | (r >> 2);
                g = (g << 3) | (g >> 2);
                b = (b << 3) | (b >> 2);
            }

            // Ӧ�ø���Ч����ʹ��8λ���ȼ��㣩
            int newR = min(255, static_cast<int>(r * 1.15 + 15));
            int newG = min(255, static_cast<int>(g * 0.85 + 5));
            int newB = min(255, static_cast<int>(b * 0.7));

            // �����㣨��3��Χ����24λͼ��С��
            int noise = (rand() % 7) - 3; // -3��3�������
            newR = max(0, min(255, newR + noise));
            newG = max(0, min(255, newG + noise));
            newB = max(0, min(255, newB + noise));

            // ת����16λ��ʽ
            if (m_bIs565Format) {
                // 8��5λ��ɫ��ȡ��5λ | 8��6λ��ɫ��ȡ��6λ | 8��5λ��ɫ
                *pixel = ((newR >> 3) << 11) | 
                         ((newG >> 2) << 5)  | 
                         (newB >> 3);
            } else {
                // 555��ʽ��ȫ��ȡ��5λ
                *pixel = ((newR >> 3) << 10) | 
                         ((newG >> 3) << 5)  | 
                         (newB >> 3);
            }
        }
    }
}

//���ŷ�����ɫ����(24/32λ)
void CImageProc::ApplyVintageToTrueColor()
{
    if (!IsValid() || (nBitCount != 24 && nBitCount != 32)) return;

    int rowSize = ((nWidth * nBitCount + 31) / 32) * 4;
    int bytesPerPixel = nBitCount / 8; // 3 for 24-bit, 4 for 32-bit

    for (int y = 0; y < nHeight; y++) {
        BYTE* pPixel = pBits + (nHeight - 1 - y) * rowSize;

        for (int x = 0; x < nWidth; x++) {
            // ��ȡ����λ�ã�����32λ������Alphaͨ����
            BYTE* pixel = &pPixel[x * bytesPerPixel];

            // ��ȡ��ɫ������BGR˳��
            BYTE& blue = pixel[0];
            BYTE& green = pixel[1];
            BYTE& red = pixel[2];

            // ���ŷ������㷨 -----------------------------------

            // 1. ����ɫ����������ǿ��/�ƣ���������
            int newRed = min(255, static_cast<int>(red * 1.15));
            int newGreen = min(255, static_cast<int>(green * 0.85));
            int newBlue = min(255, static_cast<int>(blue * 0.7));

            // 2. ��Ӻ�ɫɫ��ƫ��
            newRed = min(255, newRed + 25);
            newGreen = min(255, newGreen + 15);

            // 3. �������Ƭ��㣨ģ�⽺Ƭ������
            int noise = (rand() % 11) - 5; // -5��5�������
            newRed = max(0, min(255, newRed + noise));
            newGreen = max(0, min(255, newGreen + noise));
            newBlue = max(0, min(255, newBlue + noise));

            // 4. �����΢��ɫЧ��
            newRed = static_cast<int>(newRed * 0.95 + 10);
            newGreen = static_cast<int>(newGreen * 0.95 + 5);
            newBlue = static_cast<int>(newBlue * 0.95);

            // д�봦������ɫ -----------------------------------
            red = static_cast<BYTE>(newRed);
            green = static_cast<BYTE>(newGreen);
            blue = static_cast<BYTE>(newBlue);

            // ����Alphaͨ�����䣨�����32λ��
            // pixel[3] ���ֲ���
        }
    }
}

//��ɫ�崴������
void CImageProc::CreateVintagePalette()
{
    if (nBitCount > 8) return;

    int paletteSize = 1 << nBitCount;
    RGBQUAD* newPalette = new RGBQUAD[paletteSize];

    // �������ŷ���ɫ��
    for (int i = 0; i < paletteSize; i++) {
        BYTE r = pQUAD[i].rgbRed;
        BYTE g = pQUAD[i].rgbGreen;
        BYTE b = pQUAD[i].rgbBlue;

        // ����Ч���㷨
        newPalette[i].rgbRed = min(255, static_cast<int>(r * 1.1 + 15));
        newPalette[i].rgbGreen = min(255, static_cast<int>(g * 0.9 + 10));
        newPalette[i].rgbBlue = min(255, static_cast<int>(b * 0.8));
        newPalette[i].rgbReserved = 0;
    }

    // Ӧ���µ�ɫ��
    memcpy(pQUAD, newPalette, paletteSize * sizeof(RGBQUAD));
    delete[] newPalette;
}

