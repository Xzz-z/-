#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <stdio.h>

// BitWriter：把 0/1 比特流打包成字节写入文件（实验(4)关键工具）
class BitWriter {
public:
    BitWriter();
    void Attach(FILE* fp);             // 绑定输出文件
    void WriteBit(int bit);            // 写 1 bit
    void WriteBits01(const char* s);   // 写 "0101..." 字符串
    void Flush();                      // 补0凑满最后一个字节并写出

private:
    FILE* m_fp;
    unsigned char m_buf; // 当前正在填充的字节
    int m_cnt;           // 已填充bit数（0..7）
};

#endif