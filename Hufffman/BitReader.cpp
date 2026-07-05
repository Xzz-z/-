#include "BitReader.h"

BitReader::BitReader() : m_fp(0), m_buf(0), m_pos(0), m_hasBuf(false) {}

void BitReader::Attach(FILE* fp) {
    m_fp = fp;
    m_buf = 0;
    m_pos = 0;
    m_hasBuf = false;
}

int BitReader::ReadBit() {
    if (!m_fp) return -1;

    // 如果没有缓存字节 or 已读完8位，则从文件读新字节
    if (!m_hasBuf || m_pos >= 8) {
        int c = fgetc(m_fp);
        if (c == EOF) return -1;
        m_buf = (unsigned char)c;
        m_pos = 0;
        m_hasBuf = true;
    }

    // 从最高位到最低位依次读
    int bit = (m_buf & (1 << (7 - m_pos))) ? 1 : 0;
    m_pos++;
    return bit;
}