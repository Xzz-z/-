#ifndef BIT_READER_H
#define BIT_READER_H

#include <stdio.h>

// BitReader：从文件中按位读取（与 BitWriter 配套）
// 读出的 bit 顺序：从一个字节的最高位(MSB)到最低位(LSB)
class BitReader {
public:
    BitReader();
    void Attach(FILE* fp);

    // 读取一个 bit
    // 返回：0 或 1
    // 若到达文件末尾无法再读，返回 -1
    int ReadBit();

private:
    FILE* m_fp;
    unsigned char m_buf; // 当前字节缓存
    int m_pos;           // 下一个要读的bit位置：0..7（0表示读MSB）
    bool m_hasBuf;
};

#endif