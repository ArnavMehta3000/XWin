#pragma once
#include <stdexcept>
#include <string>

namespace XWin
{
    class XWinException : public std::exception
    {
    public:
        XWinException() : m_message("XWin Exception") {}
        XWinException(const std::string& errorMsg) : m_message("[XWin Error] " + errorMsg) {}

        const char* what() const noexcept override
        {
            return m_message.data();
        }

    private:
        std::string m_message;
    };
}
