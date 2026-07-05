#ifndef TOURISM_H
#define TOURISM_H

#include "Graph.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Tourism
{
private:
    CGraph m_Graph; // 图对象

    // 生成Vex.txt和Edge.txt文件（先输入信息再创建文件）
    void GenerateDataFiles();

    // DFS旅游导航辅助函数：枚举从起点出发的所有无回路完整游览路线
    void DFSHelper(int v, bool visited[], int path[], int depth, int &routeCount);

public:
    // 创建景区景点图（读取文件）
    void CreateGraph();

    // 查询景点信息
    void GetSpotInfo();

    // 旅游景点导航（DFS）
    void TouristNavigation();

    // 搜索最短路径（Dijkstra）
    void SearchShortestPath();

    // 铺设电路规划（Prim）
    void CircuitPlanning();

    // 显示系统菜单
    void ShowMenu();
};

#endif // TOURISM_H
