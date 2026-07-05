#include "HuffmanCompress.h"
#include "HuffmanTree.h"
#include "HuffmanCode.h"
#include "BitWriter.h"

#include <stdio.h>
#include <wchar.h>

// 统计权值：逐字节读入并计数
static bool ComputeWeights(const std::wstring& inPath,
                           unsigned long long weight[256],
                           unsigned long long& fileSize) {
    for (int i = 0; i < 256; ++i) weight[i] = 0;
    fileSize = 0;

    FILE* fp = _wfopen(inPath.c_str(), L"rb");
    if (!fp) return false;

    int c;
    while ((c = fgetc(fp)) != EOF) {
        weight[(unsigned char)c]++;
        fileSize++;
    }

    fclose(fp);
    return true;
}

// [4字节] "HUF1"
// [8字节] 原文件大小 fileSize
// [256*8字节] weight[256]（用于解压重建同样的树）
// 后面跟压缩比特流
static void WriteHeader(FILE* out,
                        unsigned long long fileSize,
                        const unsigned long long weight[256]) {
    const unsigned char magic[4] = { 'H','U','F','1' };
    fwrite(magic, 1, 4, out);
    fwrite(&fileSize, sizeof(unsigned long long), 1, out);
    fwrite(weight, sizeof(unsigned long long), 256, out);
}

bool HuffmanCompressFile(const std::wstring& inPath,
                         const std::wstring& outPath,
                         CompressStats& stats,
                         bool printCodeTable)
 {
    // (1) 统计权值
    unsigned long long weight[256];
    unsigned long long fileSize = 0;
    if (!ComputeWeights(inPath, weight, fileSize)) return false;

    // (2) 构建 Huffman 树
    int nLeaf = 0, totalNodes = 0, root = -1;
    HNode* HT = BuildHuffmanTree(weight, nLeaf, totalNodes, root);
    if (!HT) return false;

    // 根权值必须等于原文件大小（强校验）
    if (HT[root].w != fileSize) {
        FreeHuffmanTree(HT);
        return false;
    }

    // (3) 生成 Huffman 编码
    std::string code[256];
    GenerateHuffmanCodes(HT, totalNodes, nLeaf, root, code);

    // (4)(5) 压缩并保存
    FILE* fin = _wfopen(inPath.c_str(), L"rb");
    if (!fin) { FreeHuffmanTree(HT); return false; }

    FILE* fout = _wfopen(outPath.c_str(), L"wb");
    if (!fout) { fclose(fin); FreeHuffmanTree(HT); return false; }

    // 先写头（这样解压方能还原）
    WriteHeader(fout, fileSize, weight);

    // 再写压缩比特流：边读边编码边写
    BitWriter bw;
    bw.Attach(fout);

    int c;
    while ((c = fgetc(fin)) != EOF) {
        const std::string& bits = code[(unsigned char)c];
        // 正常应非空；若为空说明编码生成有问题（除非文件为空）
        bw.WriteBits01(bits.c_str());
    }
    bw.Flush();

    fclose(fin);
    fclose(fout);
    FreeHuffmanTree(HT);
    return true;
}