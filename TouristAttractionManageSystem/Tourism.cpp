#include "Tourism.h"
#include <algorithm>
#include <climits>
#include <vector>

// 生成数据文件（先输入景点/道路信息）
void Tourism::GenerateDataFiles()
{
    ofstream vexOut("Vex.txt");
    ofstream edgeOut("Edge.txt");

    if (!vexOut || !edgeOut)
    {
        cout << "文件创建失败！" << endl;
        return;
    }

    // 输入景点信息
    int vexNum;
    cout << "请输入景点总数：";
    cin >> vexNum;
    cin.ignore(); // 忽略换行符

    for (int i = 0; i < vexNum; i++)
    {
        Vex vex;
        vex.num = i;
        cout << "请输入第" << i << "个景点的名字：";
        cin.getline(vex.name, 20);
        cout << "请输入第" << i << "个景点的介绍：";
        cin.getline(vex.desc, 1024);

        // 写入Vex.txt
        vexOut << vex.num << " " << vex.name << " " << vex.desc << endl;
    }

    // 输入道路信息
    int edgeNum;
    cout << "请输入道路总数：";
    cin >> edgeNum;
    cin.ignore();

    for (int i = 0; i < edgeNum; i++)
    {
        Edge edge;
        cout << "请输入第" << i << "条道路的起点编号、终点编号、长度（空格分隔）：";
        cin >> edge.vex1 >> edge.vex2 >> edge.weight;

        // 写入Edge.txt
        edgeOut << edge.vex1 << " " << edge.vex2 << " " << edge.weight << endl;
    }

    vexOut.close();
    edgeOut.close();
    cout << "数据文件生成成功！" << endl;
}

// 创建景区景点图（读取文件并初始化）
void Tourism::CreateGraph()
{
    // 第一步：初始化图
    m_Graph.Init();

    // 先生成数据文件
    GenerateDataFiles();

    // 第二步：读取Vex.txt，插入顶点
    ifstream vexIn("Vex.txt");
    if (!vexIn)
    {
        cout << "Vex.txt文件不存在！" << endl;
        return;
    }

    Vex vex;
    while (vexIn >> vex.num >> vex.name >> ws) // ws忽略空格
    {
        vexIn.getline(vex.desc, 1024);
        m_Graph.InsertVex(vex);
    }
    vexIn.close();

    // 第三步：读取Edge.txt，插入边
    ifstream edgeIn("Edge.txt");
    if (!edgeIn)
    {
        cout << "Edge.txt文件不存在！" << endl;
        return;
    }

    Edge edge;
    while (edgeIn >> edge.vex1 >> edge.vex2 >> edge.weight)
    {
        m_Graph.InsertEdge(edge);
    }
    edgeIn.close();

    // 输出创建结果（贴合图片格式）
    cout << "===== 创建景区景点图 =====" << endl;
    cout << "顶点数目：" << m_Graph.GetVexNum() << endl;
    cout << "----- 顶点 -----" << endl;
    for (int i = 0; i < m_Graph.GetVexNum(); i++)
    {
        Vex v = m_Graph.GetVex(i);
        cout << v.num << "-" << v.name << endl;
    }

    cout << "----- 边 -----" << endl;
    Edge tempEdge[100];
    for (int i = 0; i < m_Graph.GetVexNum(); i++)
    {
        int cnt = m_Graph.FindEdge(i, tempEdge);
        for (int j = 0; j < cnt; j++)
        {
            // 避免重复输出（只输出i<j的边）
            if (tempEdge[j].vex1 < tempEdge[j].vex2)
            {
                cout << "<v" << tempEdge[j].vex1 << ",v" << tempEdge[j].vex2 << "> "
                     << tempEdge[j].weight << endl;
            }
        }
    }
}

// 查询景点信息
void Tourism::GetSpotInfo()
{
    // 1. 显示所有景点列表
    cout << "===== 景点列表 =====" << endl;
    for (int i = 0; i < m_Graph.GetVexNum(); i++)
    {
        Vex v = m_Graph.GetVex(i);
        cout << "编号：" << v.num << "  名称：" << v.name << endl;
    }

    // 2. 输入要查询的景点编号
    int vNum;
    cout << "请输入要查询的景点编号：";
    cin >> vNum;

    if (vNum < 0 || vNum >= m_Graph.GetVexNum())
    {
        cout << "景点编号不存在！" << endl;
        return;
    }

    // 3. 显示景点详细信息
    Vex targetVex = m_Graph.GetVex(vNum);
    cout << "===== 景点详细信息 =====" << endl;
    cout << "编号：" << targetVex.num << endl;
    cout << "名称：" << targetVex.name << endl;
    cout << "介绍：" << targetVex.desc << endl;

    // 4. 查询相邻景点
    Edge adjEdges[100];
    int edgeCnt = m_Graph.FindEdge(vNum, adjEdges);
    cout << "===== 相邻景点及距离 =====" << endl;
    for (int i = 0; i < edgeCnt; i++)
    {
        Vex adjVex = m_Graph.GetVex(adjEdges[i].vex2);
        cout << "相邻景点：" << adjVex.name << "  距离：" << adjEdges[i].weight << "m" << endl;
    }
}

// DFS辅助函数：枚举从起点出发、无回路游览全部景点的所有路线
void Tourism::DFSHelper(int v, bool visited[], int path[], int depth, int &routeCount)
{
    visited[v] = true;
    path[depth] = v;

    if (depth == m_Graph.GetVexNum() - 1)
    {
        routeCount++;
        cout << "路线" << routeCount << "：";
        for (int i = 0; i <= depth; i++)
        {
            cout << m_Graph.GetVex(path[i]).name;
            if (i < depth)
            {
                cout << " → ";
            }
        }
        cout << endl;
        visited[v] = false;
        return;
    }

    for (int i = 0; i < m_Graph.GetVexNum(); i++)
    {
        if (m_Graph.GetAdjMatrixValue(v, i) > 0 && !visited[i])
        {
            DFSHelper(i, visited, path, depth + 1, routeCount);
        }
    }

    visited[v] = false;
}

// 旅游景点导航（DFS）
void Tourism::TouristNavigation()
{
    if (m_Graph.GetVexNum() == 0)
    {
        cout << "请先创建景区景点图！" << endl;
        return;
    }

    int startNum;
    cout << "请输入起始景点编号：";
    cin >> startNum;

    if (startNum < 0 || startNum >= m_Graph.GetVexNum())
    {
        cout << "景点编号不存在！" << endl;
        return;
    }

    bool visited[MAX_VERTEX_NUM] = {false};
    int path[MAX_VERTEX_NUM];
    int routeCount = 0;

    cout << "===== 旅游导航路线 =====" << endl;
    DFSHelper(startNum, visited, path, 0, routeCount);

    if (routeCount == 0)
    {
        cout << "未找到从该起点出发、无回路游览全部景点的路线！" << endl;
        return;
    }

    cout << "共找到" << routeCount << "条符合要求的路线。" << endl;
}

// 最短路径（Dijkstra）
void Tourism::SearchShortestPath()
{
    int start, end;
    cout << "请输入起点编号：";
    cin >> start;
    cout << "请输入终点编号：";
    cin >> end;

    if (start < 0 || start >= m_Graph.GetVexNum() || end < 0 || end >= m_Graph.GetVexNum())
    {
        cout << "景点编号不存在！" << endl;
        return;
    }

    int dist[MAX_VERTEX_NUM];   // 最短距离
    int path[MAX_VERTEX_NUM];   // 路径前驱
    bool visited[MAX_VERTEX_NUM]; // 访问标记

    // 初始化
    for (int i = 0; i < m_Graph.GetVexNum(); i++)
    {
        dist[i] = m_Graph.GetAdjMatrixValue(start, i) == 0 ? INT_MAX : m_Graph.GetAdjMatrixValue(start, i);
        path[i] = (m_Graph.GetAdjMatrixValue(start, i) > 0) ? start : -1;  // 【修复】直接邻接点的前驱是 start
        visited[i] = false;
    }
    dist[start] = 0;
    // Deleted:visited[start] = true;  // 【修复】删除这行，起点不应立即标记

    // Dijkstra 核心
    for (int i = 1; i < m_Graph.GetVexNum(); i++)
    {
        // 找未访问的最小距离顶点
        int min = INT_MAX, u = -1;
        for (int j = 0; j < m_Graph.GetVexNum(); j++)
        {
            if (!visited[j] && dist[j] < min)
            {
                min = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        visited[u] = true;

        // 松弛操作
        for (int v = 0; v < m_Graph.GetVexNum(); v++)
        {
            int weight = m_Graph.GetAdjMatrixValue(u, v);
            if (!visited[v] && weight > 0 && dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                path[v] = u;
            }
        }
    }

    // 输出结果
    if (dist[end] == INT_MAX)
    {
        cout << "无可达路径！" << endl;
        return;
    }

    cout << "===== 最短路径 =====" << endl;
    cout << "最短距离：" << dist[end] << "m" << endl;
    cout << "路径：";
    // 回溯路径
    int temp = end;
    vector<string> pathVec;  // 【修复】使用 vector 存储路径
    while (temp != -1)
    {
        Vex vex = m_Graph.GetVex(temp);
        pathVec.push_back(vex.name);
        temp = path[temp];
    }

    // 反向输出路径
    for (int i = pathVec.size() - 1; i >= 0; i--)
    {
        cout << pathVec[i];
        if (i > 0)
        {
            cout << " → ";
        }
    }
    cout << endl;
}

// 铺设电路规划（Prim）

void Tourism::CircuitPlanning()
{
    int lowcost[MAX_VERTEX_NUM]; // 记录当前生成树到剩余顶点的最小权值
    int adjvex[MAX_VERTEX_NUM];  // 记录最小权值边对应的另一个顶点（前驱）
    bool visited[MAX_VERTEX_NUM];// 访问标记

    int n = m_Graph.GetVexNum();
    if (n == 0) {
        cout << "图为空，无法规划！" << endl;
        return;
    }

    // 【修复】正确初始化
    for (int i = 0; i < n; i++)
    {
        lowcost[i] = INT_MAX;      // 初始化为无穷大
        adjvex[i] = -1;            // 初始化为无效索引
        visited[i] = false;
    }

    // 从顶点 0 开始
    lowcost[0] = 0;
    // 注意：此时还没有将 0 标记为 visited，而是在第一轮循环中选出来

    int total = 0; // 总长度
    int edgeCount = 0; // 记录已选边数

    cout << "===== 电路规划（最小生成树） =====" << endl;

    // 循环 n 次，每次选择一个顶点加入生成树
    for (int i = 0; i < n; i++)
    {
        // 1. 找未访问的、lowcost 最小的顶点 k
        int min = INT_MAX;
        int k = -1;

        for (int j = 0; j < n; j++)
        {
            if (!visited[j] && lowcost[j] < min)
            {
                min = lowcost[j];
                k = j;
            }
        }

        // 2. 如果找不到有效顶点（min 仍为 INT_MAX），说明图不连通
        if (k == -1 || min == INT_MAX)
        {
            // 如果还没选够 n 个点就断了，说明图不连通
            if (i > 0) {
                cout << "警告：图不连通，无法生成包含所有景点的电路！" << endl;
            }
            break;
        }

        // 3. 将顶点 k 加入生成树
        visited[k] = true;

        // 如果不是起始点（起始点 lowcost 为 0，没有前驱边），则输出边并累加
        if (adjvex[k] != -1)
        {
            Vex v1 = m_Graph.GetVex(adjvex[k]);
            Vex v2 = m_Graph.GetVex(k);
            cout << v1.name << " - " << v2.name << "：" << min << "m" << endl;
            total += min;
            edgeCount++;
        }

        // 4. 更新 lowcost 数组：以 k 为中心，更新其邻接点
        for (int j = 0; j < n; j++)
        {
            if (!visited[j])
            {
                int weight = m_Graph.GetAdjMatrixValue(k, j);
                // 如果 k 和 j 之间有边，且这条边比 j 当前记录的 lowcost 更小
                if (weight > 0 && weight < lowcost[j])
                {
                    lowcost[j] = weight;
                    adjvex[j] = k;
                }
            }
        }
    }

    if (edgeCount == n - 1 || n == 1) {
        cout << "电路总长度：" << total << "m" << endl;
    } else {
        cout << "最终统计：部分连通，总长度：" << total << "m" << endl;
    }
}


// 显示菜单（严格贴合图片格式）
void Tourism::ShowMenu()
{
    cout << "===景区信息管理系统====" << endl;
    cout << "1.创建景区景点图" << endl;
    cout << "2.查询景点信息" << endl;
    cout << "3.旅游景点导航" << endl;
    cout << "4.搜索最短路径" << endl;
    cout << "5.铺设电路规划" << endl;
    cout << "0.退出" << endl;
}
