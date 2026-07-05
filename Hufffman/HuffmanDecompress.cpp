#include "HuffmanDecompress.h"
#include "HuffmanTree.h"
#include "BitReader.h"

#include <stdio.h>
#include <wchar.h>

// 读取并校验文件头：
// magic[4] + originalSize + weight[256]
static bool ReadHeader(FILE* in,
                       unsigned long long& originalSize,
                       unsigned long long weight[256]) {
    unsigned char magic[4];
    if (fread(magic, 1, 4, in) != 4) return false;

    if (!(magic[0] == 'H' && magic[1] == 'U' && magic[2] == 'F' && magic[3] == '1')) {
        return false; // 不是我们定义的格式
    }

    if (fread(&originalSize, sizeof(unsigned long long), 1, in) != 1) return false;
    if (fread(weight, sizeof(unsigned long long), 256, in) != 256) return false;

    return true;
}

bool HuffmanDecompressFile(const std::wstring& inHufPath,
                           const std::wstring& outPath) {
    // 打开输入 .huf
    FILE* fin = _wfopen(inHufPath.c_str(), L"rb");
    if (!fin) return false;

    // 读头
    unsigned long long originalSize = 0;
    unsigned long long weight[256] = {0};
    if (!ReadHeader(fin, originalSize, weight)) {
        fclose(fin);
        return false;
    }

    // 重建 Huffman 树（与压缩端一致）
    int nLeaf = 0, totalNodes = 0, root = -1;
    HNode* HT = BuildHuffmanTree(weight, nLeaf, totalNodes, root);
    if (!HT) {
        fclose(fin);
        return false;
    }

    // 打开输出文件
    FILE* fout = _wfopen(outPath.c_str(), L"wb");
    if (!fout) {
        FreeHuffmanTree(HT);
        fclose(fin);
        return false;
    }

    // 特殊情况：只有一个符号（树只有一个节点）
    if (totalNodes == 1 && nLeaf == 1 && root == 0) {
        int sym = HT[0].symbol;
        // 输出 originalSize 个相同字节
        for (unsigned long long i = 0; i < originalSize; ++i) {
            fputc((unsigned char)sym, fout);
        }
        fclose(fout);
        FreeHuffmanTree(HT);
        fclose(fin);
        return true;
    }

    // 正常情况：按 bit 解码
    BitReader br;
    br.Attach(fin);

    unsigned long long outCount = 0;
    int cur = root;

    while (outCount < originalSize) {
        int bit = br.ReadBit();
        if (bit < 0) {
            // 压缩数据提前结束：文件损坏或编码/读bit顺序不匹配
            fclose(fout);
            FreeHuffmanTree(HT);
            fclose(fin);
            return false;
        }

        // 走树：0左1右
        if (bit == 0) cur = HT[cur].lchild;
        else cur = HT[cur].rchild;

        if (cur < 0) {
            // 走到空指针：同上，通常是数据损坏或bit顺序/树不一致
            fclose(fout);
            FreeHuffmanTree(HT);
            fclose(fin);
            return false;
        }

        // 到达叶子：输出一个字节，回到根继续
        if (HT[cur].lchild == -1 && HT[cur].rchild == -1) {
            int sym = HT[cur].symbol;
            fputc((unsigned char)sym, fout);
            outCount++;
            cur = root;
        }
    }

    fclose(fout);
    FreeHuffmanTree(HT);
    fclose(fin);
    return true;
}