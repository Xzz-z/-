#include "HuffmanCode.h"
#include <algorithm>

// 从叶子往根走：左边记0，右边记1；最后反转得到编码
void GenerateHuffmanCodes(const HNode* HT,
                          int totalNodes,
                          int nLeaf,
                          int root,
                          std::string outCode[256]) {
    // 清空
    for (int i = 0; i < 256; ++i) outCode[i].clear();

    if (!HT || totalNodes <= 0 || nLeaf <= 0 || root < 0) return;

    // 特殊情况：只有一个符号
    if (totalNodes == 1 && nLeaf == 1 && root == 0) {
        int sym = HT[0].symbol;
        if (sym >= 0 && sym <= 255) outCode[sym] = "0";
        return;
    }

    // 叶子节点默认是 0..nLeaf-1
    for (int leaf = 0; leaf < nLeaf; ++leaf) {
        int sym = HT[leaf].symbol;
        if (sym < 0 || sym > 255) continue;

        std::string tmp;
        int cur = leaf;
        int p = HT[cur].parent;

        while (p != -1) {
            if (HT[p].lchild == cur) tmp.push_back('0');
            else tmp.push_back('1');

            cur = p;
            p = HT[cur].parent;
        }

        std::reverse(tmp.begin(), tmp.end());
        outCode[sym] = tmp;
    }
}