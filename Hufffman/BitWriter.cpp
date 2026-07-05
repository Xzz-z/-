#include "BitWriter.h"

BitWriter::BitWriter() : m_fp(0), m_buf(0), m_cnt(0) {}

void BitWriter::Attach(FILE* fp) {
    m_fp = fp;
    m_buf = 0;
    m_cnt = 0;
}

void BitWriter::WriteBit(int bit) {
    if (!m_fp) return;

    bit = (bit != 0) ? 1 : 0;

    // 左移一位，把新bit塞到最低位
    m_buf = (unsigned char)((m_buf << 1) | bit);
    m_cnt++;

    // 满 8 bit 就写出一个字节
    if (m_cnt == 8) {
        fputc(m_buf, m_fp);
        m_buf = 0;
        m_cnt = 0;
    }
}

void BitWriter::WriteBits01(const char* s) {
    if (!s) return;
    for (const char* p = s; *p; ++p) {
        if (*p == '0') WriteBit(0);
        else if (*p == '1') WriteBit(1);
        // 其它字符忽略
    }
}

void BitWriter::Flush() {
    if (!m_fp) return;

    // 若还剩不足8位，左移补0凑满写出
    if (m_cnt > 0) {
        m_buf = (unsigned char)(m_buf << (8 - m_cnt));
        fputc(m_buf, m_fp);
        m_buf = 0;
        m_cnt = 0;
    }
}