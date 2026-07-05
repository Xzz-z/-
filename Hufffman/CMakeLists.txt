cmake_minimum_required(VERSION 3.29)
project(Hufffman)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Hufffman
    BitReader.cpp
    BitReader.h
    BitWriter.cpp
    BitWriter.h
    HuffmanCode.cpp
    HuffmanCode.h
    HuffmanCompress.cpp
    HuffmanCompress.h
    HuffmanDecompress.cpp
    HuffmanDecompress.h
    HuffmanTree.cpp
    HuffmanTree.h
    main.cpp)
