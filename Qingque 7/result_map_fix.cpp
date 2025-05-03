#include "rules/qingque.hpp"
#include <fstream>
#include <iostream>

using namespace mahjong::tile_literals;
using namespace mahjong::honours;

using namespace qingque;

struct my_map {
    qingque::w_data m_data;

    void add_w(const std::bitset<qingque::code_size>& res, uint64_t weight) {
        if (m_data.weights.find(res) == m_data.weights.end()) m_data.weights[res] = weight;
        m_data.weights[res] += weight;
    }

    void add_w_r(const std::vector<std::bitset<qingque::code_size>>& v, std::vector<unsigned> ind, uint64_t weight) {
        uint64_t pm = (ind.size() & 1) ? weight : -weight;
        std::bitset<qingque::code_size> res = v[ind[0]];
        for (int i = 1; i < ind.size(); ++i)
            res &= v[ind[i]];
        add_w(res, pm);
        for (int i = ind.back() + 1; i < v.size(); ++i) {
            ind.push_back(i);
            add_w_r(v, ind, weight);
            ind.pop_back();
        }
    }  

    void add_data(const hand& h, uint64_t weight) {
        auto v = qingque::evaluate_fans(h, true);
        std::cout << v[0] << std::endl;
        // m_data.total_weight += weight;
        auto size = v.size();
        if (size == 1) add_w(v[0], weight);
        else for (unsigned i = 0; i < size; ++i) add_w_r(v, {i}, weight);
        for (auto& vv: v)
            vv[indices::two_triple_pairs] = false;
        if (size == 1) add_w(v[0], -weight);
        else for (unsigned i = 0; i < size; ++i) add_w_r(v, {i}, -weight);
    }

};

my_map wd, wd1;

void get_wd() {
    std::ifstream file("result_map_temp.dat", std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件 result_map_temp.dat.\n";
        std::exit(1);
    }
    uint64_t total_weight;
    file.read(reinterpret_cast<char*>(&total_weight), sizeof total_weight);
    wd.m_data.total_weight = total_weight;
    while (!file.eof()) {
        std::bitset<qingque::code_size> code;
        uint64_t weight;
        file.read(reinterpret_cast<char*>(&code), sizeof code);
        file.read(reinterpret_cast<char*>(&weight), sizeof weight);
        wd.m_data.weights.insert({code, weight});
    }
    file.close();
}

void fix() {
    using enum qingque::indices;
    wd1.m_data.total_weight = wd.m_data.total_weight;
    for (const auto& [code, weight] : wd.m_data.weights) {
        auto code1 = code;    
        if (code[fan_tile_2p]) code1[fan_tile_1p] = true;
        if (wd1.m_data.weights.find(code1) == wd1.m_data.weights.end())
            wd1.m_data.weights[code1] = weight;
        else wd1.m_data.weights[code1] += weight;
    }
}

void write() {
    std::ofstream file("result_map.dat", std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开或创建文件 result_map.dat.\n";
        std::exit(1);
    }
    file.write(reinterpret_cast<const char*>(&wd1.m_data.total_weight), sizeof wd1.m_data.total_weight);
    for (const auto& [code, weight] : wd1.m_data.weights) {
        file.write(reinterpret_cast<const char*>(&code), sizeof code);
        file.write(reinterpret_cast<const char*>(&weight), sizeof weight);
    }
    file.close();
}

int main() {
    get_wd();
    fix();
    write();
    return 0;
}