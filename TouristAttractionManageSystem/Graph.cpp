#include "Graph.h"

// 初始化图：权值全部置0（表示无边）
void CGraph::Init()
{
    // 邻接矩阵初始化
    for (int i = 0; i < MAX_VERTEX_NUM; i++)
    {
        for (int j = 0; j < MAX_VERTEX_NUM; j++)
        {
            m_aAdjMatrix[i][j] = 0;
        }
    }
    m_nVexNum = 0; // 顶点数置0
}

// 插入顶点信息
bool CGraph::InsertVex(Vex sVex)
{
    if (m_nVexNum == MAX_VERTEX_NUM)
    {
        // 顶点已满
        return false;
    }
    m_aVexs[m_nVexNum++] = sVex; // 插入顶点信息
    return true;
}

// 插入边信息（无向图，双向赋值）
bool CGraph::InsertEdge(Edge sEdge)
{
    // 检查下标越界
    if (sEdge.vex1 < 0 || sEdge.vex1 >= m_nVexNum ||
        sEdge.vex2 < 0 || sEdge.vex2 >= m_nVexNum)
    {
        return false;
    }
    // 插入边的信息
    m_aAdjMatrix[sEdge.vex1][sEdge.vex2] = sEdge.weight;
    m_aAdjMatrix[sEdge.vex2][sEdge.vex1] = sEdge.weight;
    return true;
}

// 获取指定编号的顶点信息
Vex CGraph::GetVex(int v)
{
    return m_aVexs[v];
}

// 查询与指定顶点相连的所有边，返回边的条数
int CGraph::FindEdge(int v, Edge aEdge[])
{
    int k = 0;
    for (int i = 0; i < m_nVexNum; i++)
    {
        // 找到有边的顶点（权值>0）
        if (m_aAdjMatrix[v][i] > 0)
        {
            aEdge[k].vex1 = v;
            aEdge[k].vex2 = i;
            aEdge[k].weight = m_aAdjMatrix[v][i];
            k++;
        }
    }
    return k; // 返回边的条数
}