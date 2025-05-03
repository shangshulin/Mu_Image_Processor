#pragma once
#include <functional>

class CCommand
{
public:
    virtual ~CCommand() {}
    virtual void Execute() = 0;
    virtual void Undo() = 0;
};

// ͨ��������
class CGenericCommand : public CCommand
{
private:
    std::function<void()> m_executeFunc; // ִ�в����ĺ���
    std::function<void()> m_undoFunc;    // ���ز����ĺ���

public:
    CGenericCommand(std::function<void()> executeFunc, std::function<void()> undoFunc)
        : m_executeFunc(executeFunc), m_undoFunc(undoFunc) {}

    void Execute() override
    {
        if (m_executeFunc)
            m_executeFunc();
    }

    void Undo() override
    {
        if (m_undoFunc)
            m_undoFunc();
    }
}; 
#pragma once
