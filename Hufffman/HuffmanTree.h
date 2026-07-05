#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

// Huffman 节点（结构体数组存树）
struct HNode {
    unsigned long long w; // 权值
    int parent;           // 父结点下标，-1表示无父（根）
    int lchild;           // 左孩子下标
    int rchild;           // 右孩子下标
    int symbol;           // 叶子:0..255，非叶子:-1（便于调试）
};

// 根据权值表构建 Huffman 树
// - weight[256]：各字节出现次数（权值）
// - out_nLeaf：出现过的符号数 n
// - out_totalNodes：总节点数 m=2n-1（n==1则m=1）
// - out_root：根节点下标（m-1 或 0）
// 返回：动态分配的树数组 HT（用完 delete[] / FreeHuffmanTree）
// 失败返回 nullptr
HNode* BuildHuffmanTree(const unsigned long long weight[256],
                        int& out_nLeaf,
                        int& out_totalNodes,
                        int& out_root);

void FreeHuffmanTree(HNode* HT);

#endif