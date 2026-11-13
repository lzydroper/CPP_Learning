#include <iostream>
#include <vector>
#include "DataStream.hpp"

int main()
{
    serialize::DataStream ds;

    // --- 模拟保存存档 ---
    ds << 100 << "PlayerOne" << std::vector<int>{1, 2, 3};
    if (ds.save("savegame.dat")) {
        std::cout << "保存成功！\n";
    }
    {
        int id;
        std::string name;
        std::vector<int> vec;
        ds >> id >> name >> vec;
        std::cout << "ID: " << id << ", Name: " << name << "\n";
    }

    // --- 模拟读取存档 ---
    serialize::DataStream loadDs;
    if (loadDs.load("savegame.dat")) {
        std::cout << "加载成功，尝试解析数据：\n";
        int id; std::string name; std::vector<int> vec;
        loadDs >> id >> name >> vec;
        std::cout << "ID: " << id << ", Name: " << name << "\n";
    }

    return 0;
    
    return 0;
}