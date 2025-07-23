#include "pch.h"
#include "CImageProc.h"
#include <afxdlgs.h>

// 测试SetColor函数对不同位深图像的支持
void TestSetColor()
{
    CImageProc imgProc;
    
    // 测试1位图像
    CString path1bit = L"实验一图片\\1_1bit.bmp";
    imgProc.LoadBmp(path1bit);
    if (imgProc.IsValid() && imgProc.nBitCount == 1)
    {
        // 在图像中央绘制一个红色方块
        int centerX = imgProc.nWidth / 2;
        int centerY = imgProc.nHeight / 2;
        int size = 20;
        
        for (int y = centerY - size; y < centerY + size; ++y)
        {
            for (int x = centerX - size; x < centerX + size; ++x)
            {
                if (x >= 0 && x < imgProc.nWidth && y >= 0 && y < imgProc.nHeight)
                {
                    BYTE* pixel = imgProc.GetPixelPtr(x, y);
                    imgProc.SetColor(pixel, x, y, 255, 0, 0); // 红色
                }
            }
        }
        
        // 保存修改后的图像
        // 这里需要实现保存功能，或者在UI中显示
    }
    
    // 测试4位图像
    CString path4bit = L"实验一图片\\7_4bit.bmp";
    imgProc.LoadBmp(path4bit);
    if (imgProc.IsValid() && imgProc.nBitCount == 4)
    {
        // 在图像中央绘制一个绿色方块
        int centerX = imgProc.nWidth / 2;
        int centerY = imgProc.nHeight / 2;
        int size = 20;
        
        for (int y = centerY - size; y < centerY + size; ++y)
        {
            for (int x = centerX - size; x < centerX + size; ++x)
            {
                if (x >= 0 && x < imgProc.nWidth && y >= 0 && y < imgProc.nHeight)
                {
                    BYTE* pixel = imgProc.GetPixelPtr(x, y);
                    imgProc.SetColor(pixel, x, y, 0, 255, 0); // 绿色
                }
            }
        }
        
        // 保存修改后的图像
        // 这里需要实现保存功能，或者在UI中显示
    }
    
    // 测试8位图像
    CString path8bit = L"实验一图片\\1_8bit.bmp";
    imgProc.LoadBmp(path8bit);
    if (imgProc.IsValid() && imgProc.nBitCount == 8)
    {
        // 在图像中央绘制一个蓝色方块
        int centerX = imgProc.nWidth / 2;
        int centerY = imgProc.nHeight / 2;
        int size = 20;
        
        for (int y = centerY - size; y < centerY + size; ++y)
        {
            for (int x = centerX - size; x < centerX + size; ++x)
            {
                if (x >= 0 && x < imgProc.nWidth && y >= 0 && y < imgProc.nHeight)
                {
                    BYTE* pixel = imgProc.GetPixelPtr(x, y);
                    imgProc.SetColor(pixel, x, y, 0, 0, 255); // 蓝色
                }
            }
        }
        
        // 保存修改后的图像
        // 这里需要实现保存功能，或者在UI中显示
    }
}