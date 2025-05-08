#include "pch.h" // Ԥ����ͷ�ļ�
#include "CFilterSizeDialog.h" // ����ͷ�ļ�

// ʵ�� CFilterSizeDialog �Ķ�̬��������
IMPLEMENT_DYNAMIC(CFilterSizeDialog, CDialogEx)

// ���캯��ʵ�֣���ʼ���˲����ߴ�Ϊ 3
CFilterSizeDialog::CFilterSizeDialog(CWnd* pParent)
    : CDialogEx(IDD_FILTER_SIZE, pParent) // ���û��๹�캯����ָ���Ի�����Դ ID
    , m_nFilterSize(3) // ��ʼ����Ա����
{
}

// ���ݽ�������ʵ�֣�ʵ�ֿؼ����Ա����������ͬ��
void CFilterSizeDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX); // ���û�������ݽ���
    DDX_Text(pDX, IDC_FILTER_SIZE, m_nFilterSize); // �󶨱༭��ؼ��� m_nFilterSize
    DDV_MinMaxInt(pDX, m_nFilterSize, 3, 15); // ���� m_nFilterSize ����Ч��ΧΪ 3~15
}

// �Ի����ʼ������ʵ��
BOOL CFilterSizeDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog(); // ���û����ʼ��
    return TRUE; // ���� TRUE����ʾĬ�Ͻ��㴦��
}

// OK ��ť��Ӧ����ʵ��
void CFilterSizeDialog::OnOK()
{
    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_FILTER_SIZE); // ��ȡ�˲����ߴ�༭��ؼ�ָ��
    CString strSize; // ���ڴ洢������ַ���
    pEdit->GetWindowText(strSize); // ��ȡ�༭������

    int size = _ttoi(strSize); // ���ַ���ת��Ϊ����
    // ��������Ƿ�Ϸ���3~15֮����Ϊ����
    if (size < 3 || size > 15 || size % 2 == 0) {
        MessageBox(_T("������3-15֮�������"), _T("�������"), MB_ICONERROR); // ����������ʾ
        return; // ���رնԻ���
    }

    m_nFilterSize = size; // ���ó�Ա����
    CDialogEx::OnOK(); // ���û��� OK���رնԻ���
}

// ��Ϣӳ���ʵ�֣��������Զ�����Ϣ��Ӧ��
BEGIN_MESSAGE_MAP(CFilterSizeDialog, CDialogEx)
END_MESSAGE_MAP()