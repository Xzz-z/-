#include "pch.h"
#include "GameException.h"

CGameException::CGameException(const CString& message)
    : m_message(message)
{
}

const CString& CGameException::GetErrorMessage() const
{
    return m_message;
}
