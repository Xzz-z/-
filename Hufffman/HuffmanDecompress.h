#ifndef HUFFMAN_DECOMPRESS_H
#define HUFFMAN_DECOMPRESS_H

#include <string>

// 解压：*.huf -> 原文件（如 bmp）
// 要求 .huf 格式为：
// "HUF1"(4B) + originalSize(8B) + weight[256](256*8B) + bitstream
bool HuffmanDecompressFile(const std::wstring& inHufPath,
                           const std::wstring& outPath);

#endif