#pragma once

#include <afx.h>

class CGameException
{
public:
    explicit CGameException(const CString& message);

    const CString& GetErrorMessage() const;

private:
    CString m_message;
};
