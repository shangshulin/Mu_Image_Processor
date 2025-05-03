// FFTLogDialog.h
#pragma once
#include "afxdialogex.h"

class CFFTLogDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CFFTLogDialog)

public:
    CFFTLogDialog(CWnd* pParent = nullptr);
    virtual ~CFFTLogDialog();

    // �Ի�������
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FFTLOG_DIALOG };
#endif

    double m_dLogBase;    // ��������
    double m_dScaleFactor; // ��������

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    // �Ƴ� OnEnChangeFilterSize ������������ڣ�
};