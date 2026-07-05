#include "HuffmanTree.h"

// 在 [0, end) 中选出两个 parent==-1 且权值最小的节点下标 s1,s2
static void SelectMin(HNode* HT, int end, int& s1, int& s2) {
    s1 = -1; s2 = -1;
    unsigned long long min1 = 0, min2 = 0;

    for (int i = 0; i < end; ++i) {
        if (HT[i].parent != -1) continue; // 已经被合并过的节点跳过

        unsigned long long w = HT[i].w;

        if (s1 == -1 || w < min1) {
            // 原最小下沉为第二小
            s2 = s1; min2 = min1;
            s1 = i;  min1 = w;
        } else if (s2 == -1 || w < min2) {
            s2 = i;  min2 = w;
        }
    }
}

HNode* BuildHuffmanTree(const unsigned long long weight[256],
                        int& out_nLeaf,
                        int& out_totalNodes,
                        int& out_root) {
    out_nLeaf = 0;
    out_totalNodes = 0;
    out_root = -1;

    // 1) 收集出现过的符号（权值>0）
    int symbolOfLeaf[256];
    for (int s = 0; s < 256; ++s) {
        if (weight[s] > 0) {
            symbolOfLeaf[out_nLeaf] = s;
            out_nLeaf++;
        }
    }

    if (out_nLeaf == 0) return 0; // 空文件

    // 特殊情况：只有一种字节
    if (out_nLeaf == 1) {
        out_totalNodes = 1;
        out_root = 0;
        HNode* HT = new HNode[1];
        int sym = symbolOfLeaf[0];
        HT[0].w = weight[sym];
        HT[0].parent = -1;
        HT[0].lchild = -1;
        HT[0].rchild = -1;
        HT[0].symbol = sym;
        return HT;
    }

    // 2) 分配 2n-1 个节点
    out_totalNodes = 2 * out_nLeaf - 1;
    out_root = out_totalNodes - 1;

    HNode* HT = new HNode[out_totalNodes];

    // 3) 初始化叶子节点 0..n-1
    for (int i = 0; i < out_nLeaf; ++i) {
        int sym = symbolOfLeaf[i];
        HT[i].w = weight[sym];
        HT[i].parent = -1;
        HT[i].lchild = -1;
        HT[i].rchild = -1;
        HT[i].symbol = sym;
    }

    // 4) 初始化非叶子节点 n..m-1
    for (int i = out_nLeaf; i < out_totalNodes; ++i) {
        HT[i].w = 0;
        HT[i].parent = -1;
        HT[i].lchild = -1;
        HT[i].rchild = -1;
        HT[i].symbol = -1;
    }

    // 5) 构建 Huffman 树：循环 n-1 次，每次合并两棵最小权子树
    for (int i = out_nLeaf; i < out_totalNodes; ++i) {
        int s1, s2;
        SelectMin(HT, i, s1, s2);
        if (s1 == -1 || s2 == -1) {
            delete[] HT;
            return 0;
        }

        HT[s1].parent = i;
        HT[s2].parent = i;

        HT[i].lchild = s1;
        HT[i].rchild = s2;
        HT[i].w = HT[s1].w + HT[s2].w;
        // HT[i].parent 已是 -1（根会保持-1直到最后）
    }

    return HT;
}

void FreeHuffmanTree(HNode* HT) {
    delete[] HT;
}
