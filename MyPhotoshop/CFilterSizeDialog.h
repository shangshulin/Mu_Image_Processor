#pragma once // ��ֹͷ�ļ�����ΰ���
#include "pch.h" // Ԥ����ͷ�ļ�
#include "resource.h" // ������Դ����
#include "CImageProc.h" // ͼ������ͷ�ļ�
#include "MyPhotoshop.h" // ������ͷ�ļ�
#include "afxdialogex.h" // MFC �Ի�����չ֧��

// CFilterSizeDialog ���������̳��� CDialogEx
class CFilterSizeDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CFilterSizeDialog) // ������̬����֧�֣�MFC �꣩

public:
    // ���캯��������ָ��������ָ�룬Ĭ��Ϊ nullptr
    CFilterSizeDialog(CWnd* pParent = nullptr);

    // ��ȡ�˲����ߴ�Ĺ��з���
    int GetFilterSize() const { return m_nFilterSize; }

protected:
    // �������ݽ������������ڿؼ������֮�������ͬ��
    virtual void DoDataExchange(CDataExchange* pDX);

    // ���ضԻ����ʼ������
    virtual BOOL OnInitDialog();

    // ���� OK ��ť��Ӧ����
    virtual void OnOK();

    int m_nFilterSize = 3; // �˲����ߴ��Ա������Ĭ��ֵΪ 3��3x3��

    DECLARE_MESSAGE_MAP() // ������Ϣӳ���
};