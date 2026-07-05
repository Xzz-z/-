#ifndef HUFFMAN_COMPRESS_H
#define HUFFMAN_COMPRESS_H

#include <string>

struct CompressStats {
    unsigned long long originalSizeBytes;      // 原文件字节数
    unsigned long long compressedFileBytes;    // .huf 总字节数
    unsigned long long headerBytes;            // 头部字节数
    unsigned long long compressedDataBytes;    // 压缩数据区字节数（不含头）
    unsigned long long totalEncodedBits;       // 理论编码总bit数
    double compressionRatio;                   // compressedFileBytes / originalSizeBytes
};

// printCodeTable=true 则打印 Huffman 编码表
bool HuffmanCompressFile(const std::wstring& inPath,
                         const std::wstring& outPath,
                         CompressStats& stats,
                         bool printCodeTable);

#endif