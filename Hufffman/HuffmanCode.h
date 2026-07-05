#ifndef HUFFMAN_CODE_H
#define HUFFMAN_CODE_H

#include <string>
#include "HuffmanTree.h"

// 由 Huffman 树生成 256 个符号的 Huffman 编码（实验(3)）
// - HT: Huffman树数组
// - totalNodes: 节点总数
// - nLeaf: 叶子数（前 nLeaf 个节点为叶子）
// - root: 根节点下标
// - outCode[256]: 输出编码字符串；未出现的符号保持空串
void GenerateHuffmanCodes(const HNode* HT,
                          int totalNodes,
                          int nLeaf,
                          int root,
                          std::string outCode[256]);

#endif