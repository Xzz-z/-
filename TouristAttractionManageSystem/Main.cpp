#include "Tourism.h"

int main()
{
    Tourism tourism;
    int choice;

    while (true)
    {
        tourism.ShowMenu();
        cout << "请输入选择：";
        cin >> choice;

        switch (choice)
        {
            case 1:
                tourism.CreateGraph();
            break;
            case 2:
                tourism.GetSpotInfo();
            break;
            case 3:
                tourism.TouristNavigation();
            break;
            case 4:
                tourism.SearchShortestPath();
            break;
            case 5:
                tourism.CircuitPlanning();
            break;
            case 0:
                cout << "退出系统！" << endl;
                return 0;
            default:
                cout << "输入错误，请重新选择！" << endl;
            break;
        }
        cout << endl; // 换行分隔
    }
}