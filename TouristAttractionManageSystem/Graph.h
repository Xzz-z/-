#ifndef GRAPH_H
#define GRAPH_H

#include <cstring>

// 最大顶点数
const int MAX_VERTEX_NUM = 20;

// 顶点结构体（景点）
struct Vex
{
    int num;                // 景点编号
    char name[20];          // 景点名字
    char desc[1024];        // 景点介绍
};

// 边结构体（道路）
struct Edge
{
    int vex1;               // 边的第一个顶点
    int vex2;               // 边的第二个顶点
    int weight;             // 权值（道路长度）
};

// 图类（邻接矩阵实现）
class CGraph
{
private:
    int m_aAdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];  // 邻接矩阵
    Vex m_aVexs[MAX_VERTEX_NUM];           // 顶点信息数组
    int m_nVexNum;             // 当前图的顶点个数

public:
    // 构造函数
    CGraph() { m_nVexNum = 0; }

    // 初始化图
    void Init();

    // 插入顶点信息
    bool InsertVex(Vex sVex);

    // 插入边信息
    bool InsertEdge(Edge sEdge);

    // 获取指定编号的顶点信息
    Vex GetVex(int v);

    // 查询与指定顶点相连的所有边
    int FindEdge(int v, Edge aEdge[]);

    // 获取顶点总数
    int GetVexNum() { return m_nVexNum; }

    // 获取邻接矩阵值
    int GetAdjMatrixValue(int i, int j) { return m_aAdjMatrix[i][j]; }
};

#endif // GRAPH_H