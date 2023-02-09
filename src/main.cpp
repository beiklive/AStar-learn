#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int steps = 0;
namespace MapFunction
{
    using MapItem = std::string;
    using MapContainer = std::vector<std::vector<MapItem>>;

    const int MapSize = 20;
    int BlockCount = 110;
    const MapItem BlockItem = "\033[93m⊞\033[0m";
    const MapItem TargetItem = "\033[92m⊡\033[0m";
    const MapItem RoleItem = "\033[91m■\033[0m";
    const MapItem SpaceItem = " ";
    const MapItem PathItem = "\033[94m■\033[0m";
    const MapItem ProgressItem = "\033[97m■\033[0m";

    struct Point
    {
        int x;
        int y;
    };

    Point StartPoint;
    Point EndPoint;

    MapContainer MissionMap(MapSize);

    Point _m_RandomGen()
    {
        srand(time(NULL));
        int x = rand() % MapSize;
        int y = rand() % MapSize;
        while (MissionMap[y][x] != SpaceItem)
        {
            x = rand() % MapSize;
            y = rand() % MapSize;
        }
        Point temp;
        temp.x = x;
        temp.y = y;
        return temp;
    }

    void _m_MapItemSet(const Point &pos, const MapItem &item)
    {
        MissionMap[pos.y][pos.x] = item;
    }

    void _m_SetItemPos()
    {
        EndPoint = _m_RandomGen();
        _m_MapItemSet(EndPoint, TargetItem);

        StartPoint = _m_RandomGen();
        _m_MapItemSet(StartPoint, RoleItem);

        Point temp;
        if (BlockCount > (MapSize - 1) * (MapSize - 1))
            BlockCount = MapSize / 2;
        for (size_t i = 0; i < BlockCount; i++)
        {
            temp = _m_RandomGen();
            _m_MapItemSet(temp, BlockItem);
        }
    }
    void MapInit()
    {
        for (int i = 0; i < MapSize; i++)
        {
            MissionMap[i].resize(MapSize);
        }
        for (int i = 0; i < MapSize; i++)
        {
            for (int j = 0; j < MapSize; j++)
            {
                MissionMap[i][j] = SpaceItem;
            }
        }
        _m_SetItemPos();
    }

    void _m_FlushDelay()
    {
        usleep(200000);

    }

    void MapFlush(bool flag)
    {
        if(flag){
            std::cout << "\033c";
            MissionMap[EndPoint.y][EndPoint.x] = TargetItem;
            MissionMap[StartPoint.y][StartPoint.x] = RoleItem;
            for (int i = 0; i < MapSize; i++)
            {
                for (int j = 0; j < MapSize; j++)
                {
                    std::cout << MissionMap[i][j] << " ";
                }
                std::cout << "│\n";
            }

            for (int i = 0; i < MapSize; i++)
                std::cout << "──";
            std::cout << "┘\n";
            std::cout << steps++ << std::endl;

            _m_FlushDelay();
        }
    }

} // namespace MapFunction

namespace AStarFunction
{
    using namespace MapFunction;
    constexpr int PathCost = 10;
    class MyPoint
    {
    public:
        int F, G, H;
        int x, y;
        bool isWalked;

    public:
        bool operator==(const MyPoint &pos)
        {
            return (pos.x == x && pos.y == y);
        }
        void SetH(const MyPoint &cur, const MyPoint &end)
        {
            H = PathCost * (abs(cur.x - end.x) + abs(cur.y - end.y));
        }
        void SetG(int num) { G = num; }
        void SetF() { F = G + H; }
    };
    class TreeNode
    {
    public:
        // curren Point
        MyPoint pos;
        // container to store one or one more child point
        std::vector<TreeNode *> pChildNode;
        // parent Node;
        TreeNode *pParent;
        int TreeLength;

    public:
        TreeNode(const MyPoint &next)
        {
            pos = next;
            pParent = nullptr;
        }
    };

    enum Dir
    {
        cur_up,
        cur_down,
        cur_left,
        cur_right
    };
    // check border and block
    bool walkCheck(const MapContainer &box, const MyPoint &pos)
    {
        if (pos.y < 0 || pos.x < 0 || pos.y >= MapSize || pos.x >= MapSize)
        {
            return false;
        }

        if (box[pos.y][pos.x] == TargetItem)
        {
            return true;
        }

        // if (box[pos.y][pos.x] == BlockItem || box[pos.y][pos.x] == RoleItem)
        if (box[pos.y][pos.x] != SpaceItem)
        {
            return false;
        }
        return true;
    }

    void StartAStar()
    {
        MapFunction::MapInit();
        MyPoint StartPos;
        StartPos.x = StartPoint.x;
        StartPos.y = StartPoint.y;

        MyPoint EndPos;
        EndPos.x = EndPoint.x;
        EndPos.y = EndPoint.y;
        // root init
        TreeNode *pRoot = new TreeNode(StartPos);
        pRoot->pParent == nullptr;
        pRoot->TreeLength = 0;
        TreeNode *pCur = pRoot;
        TreeNode *pTemp = nullptr;

        std::vector<TreeNode *> buff;
        std::vector<TreeNode *> FinishList;
        std::vector<TreeNode *> OpenList;
        bool FindFlag = false;
        while (1)
        {
            if (pCur != nullptr)
            {
                // check all directions and sava the direction that can move to;
                for (size_t i = 0; i < 4; i++)
                {
                    pTemp = new TreeNode(pCur->pos);
                    switch (i)
                    {
                    case cur_up:
                        pTemp->pos.y--;
                        break;
                    case cur_down:
                        pTemp->pos.y++;
                        break;
                    case cur_left:
                        pTemp->pos.x--;
                        break;
                    case cur_right:
                        pTemp->pos.x++;
                        break;
                    default:
                        break;
                    }

                    if (walkCheck(MissionMap, pTemp->pos))
                    {
                        if (pTemp->pos == EndPos)
                        {
                            buff.clear();
                        }
                        pCur->pChildNode.push_back(pTemp);
                        pTemp->pParent = pCur;
                        pTemp->TreeLength = pCur->TreeLength + 1;
                        pTemp->pos.SetG(PathCost);
                        pTemp->pos.SetH(pTemp->pos, EndPos);
                        pTemp->pos.SetF();
                        buff.push_back(pTemp);

                        if (pTemp->pos == EndPos)
                        {
                            break;
                        }
                    }
                    else
                    {
                        delete pTemp;
                        pTemp = nullptr;
                    }
                }
            }
            // push qualified data into openlist and clean buffer list.
            for (auto it = buff.begin(); it != buff.end(); ++it)
            {
                OpenList.push_back(*it);
            }
            buff.clear();

            if (OpenList.size() > 0)
            {
                /* code */
                // find all minimum score
                auto minF = OpenList.begin();
                for (auto it = OpenList.begin(); it != OpenList.end(); ++it)
                {
                    minF = (*minF)->pos.F < (*it)->pos.F ? minF : it;
                }
                pCur = *minF;
                OpenList.erase(minF);
                MissionMap[pCur->pos.y][pCur->pos.x] = ProgressItem;

                if (pCur->pos == EndPos)
                {
                    FindFlag = true;
                    FinishList.push_back(pCur);
                    pCur = nullptr;
                    break;
                }

                MapFunction::MapFlush(true);
                if (OpenList.size() == 0)
                {
                    break;
                }
            }
        }
        if (FindFlag)
        {
            auto MinSize = FinishList.begin();
            for (auto it = FinishList.begin(); it != FinishList.end(); ++it)
            {
                MinSize = (*MinSize)->TreeLength < (*it)->TreeLength ? MinSize : it;
            }
                auto pTemp2 = *MinSize;
                while (pTemp2->pParent != nullptr)
                {
                    MissionMap[pTemp2->pos.y][pTemp2->pos.x] = PathItem;
                    pTemp2 = pTemp2->pParent;
                }

            MapFunction::MapFlush(true);
        }
    }

} // namespace A

int main(int argc, char **argv)
{
    AStarFunction::StartAStar();
    return 0;
}
