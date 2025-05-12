#include "rules/qingque.hpp"
#include <fstream>
#include <iostream>

using namespace mahjong::tile_literals;
using namespace mahjong::honours;

qingque::w_data wd;

void get_wd() {
    std::ifstream file("result_map.dat", std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件 result_map.dat.\n";
        std::exit(1);
    }
    uint64_t total_weight;
    file.read(reinterpret_cast<char*>(&total_weight), sizeof total_weight);
    wd.total_weight = total_weight;
    while (!file.eof()) {
        std::bitset<qingque::code_size> code;
        uint64_t weight;
        file.read(reinterpret_cast<char*>(&code), sizeof code);
        file.read(reinterpret_cast<char*>(&weight), sizeof weight);
        wd.weights.insert({code, weight});
    }
    file.close();
}

void write() {
    std::ofstream file("result_map.h", std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开或创建文件 result_map.h.\n";
        std::exit(1);
    }
    file << "#include \"rules/qingque.hpp\"\n\n";
    file << "namespace qingque_wd {\n";
    file << "    qingque::w_data get_wd() {\n";
    file << "        qingque::w_data wd;\n";
    file << "        wd.total_weight = " << wd.total_weight << "ull;\n";
    file << "        wd.weights = {\n";
    for (const auto& [code, weight] : wd.weights) {
        file << "            {std::bitset<" << qingque::code_size << ">(\"" << code << "\"), " << weight << "ull},\n";
    }
    file << "        };\n";
    file << "        return wd;\n";
    file << "    }\n";
    file << "}\n";
    file.close();
}

int main() {
    get_wd();
    write();
    return 0;
}