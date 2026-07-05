#include <iostream>
#include <string>
#include <stdio.h>
#include <wchar.h>

#include "HuffmanCompress.h"
#include "HuffmanDecompress.h"

static unsigned long long GetFileSizeByPath(const std::wstring& path) {
    FILE* fp = _wfopen(path.c_str(), L"rb");
    if (!fp) return 0ULL;
    fseek(fp, 0, SEEK_END);
    long end = ftell(fp);
    fclose(fp);
    return (end < 0) ? 0ULL : (unsigned long long)end;
}

static std::wstring MakeRecoverPath(const std::wstring& inBmp) {
    // 输出恢复文件名：原名后加 _recover.bmp
    return inBmp + L"_recover.bmp";
}

int main() {
    std::wcout << L"Input bmp path: ";
    std::wstring bmpPath;
    std::getline(std::wcin, bmpPath);

    std::wstring hufPath = bmpPath + L".huf";
    std::wstring recoverPath = MakeRecoverPath(bmpPath);

    // 压缩前大小
    unsigned long long originalBytes = GetFileSizeByPath(bmpPath);

    // 1) 压缩（printCodeTable 你可改 true/false）
    CompressStats st;
    bool ok1 = HuffmanCompressFile(bmpPath, hufPath, st, false);
    if (!ok1) {
        std::wcout << L"Compress FAILED.\n";
        return 0;
    }

    // 压缩后大小（也可用 st.compressedFileBytes）
    unsigned long long compressedBytes = GetFileSizeByPath(hufPath);

    // 2) 解压
    bool ok2 = HuffmanDecompressFile(hufPath, recoverPath);
    if (!ok2) {
        std::wcout << L"Decompress FAILED.\n";
        return 0;
    }

    // 解压后大小
    unsigned long long decompressedBytes = GetFileSizeByPath(recoverPath);

    // 压缩比率（按老师习惯一般=压缩后/压缩前）
    double ratio = (originalBytes == 0) ? 0.0 : (double)compressedBytes / (double)originalBytes;

    // 输出验收需要的内容
    std::wcout << L"\n========== Result ==========\n";
    std::wcout << L"Original size (bytes)    : " << originalBytes << L"\n";
    std::wcout << L"Compressed size (bytes)  : " << compressedBytes << L"\n";
    std::wcout << L"Decompressed size (bytes): " << decompressedBytes << L"\n";
    std::wcout << L"Compression ratio        : " << ratio << L"\n";
    std::wcout << L"Compressed file          : " << hufPath << L"\n";
    std::wcout << L"Recovered file           : " << recoverPath << L"\n";
    std::wcout << L"===========================\n";

    return 0;
}