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

my_map wd;

void get_wd() {
    std::ifstream file("result_map_temp.dat", std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件 result_map.dat.\n";
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

uint64_t hand_weight(const hand& h, uint64_t wtile_size) {
    auto closed_counter = h.counter(false);
    uint64_t weight = wtile_size * 1620;
    if (!h.winning_type()(win_type::self_drawn)) weight *= 3;
    auto open_counter = h.counter();
    for (auto m : h.melds()) {
        if (m.type() == meld_type::triplet) weight *= 3;
        if (m.type() == meld_type::kong && m.concealed()) weight /= 3;
    }
    auto decompose_size = h.decompose().size() + (bool)qingque::is_seven_pairs(h);
    if (decompose_size == 0) return 0;
    // if (decompose_size > 1) weight /= decompose_size;
    for (auto ti : tile_set::all_tiles) {
        if (open_counter.count(ti) == 1) weight *= 4;
        if (open_counter.count(ti) == 2) weight *= 6;
        if (open_counter.count(ti) == 3) weight *= 4;
    }
    return weight;
}

void fix_hand(const hand& h) {
    auto weight = hand_weight(h, 14);
    wd.add_data(h, weight);
}

void fix() {
    auto m = mahjong::tile_literals::operator""_m;
    auto p = mahjong::tile_literals::operator""_p;
    auto s = mahjong::tile_literals::operator""_s;
    for (int i = 1; i <= 9; ++i)
    for (int j = i; j <= 9; ++j)
    for (tile_t k : mahjong::tile_set::all_tiles) {
        auto h = hand(std::vector<tile_t>{m(i), m(i), m(j), m(j), p(i), p(i), p(j), p(j), s(i), s(i), s(j), s(j), k}, {}, k, 0);
        if (!h.is_valid()) continue;
        auto h1 = hand(std::vector<tile_t>{m(i), m(i), m(j), m(j), p(i), p(i), p(j), p(j), s(i), s(i), s(j), s(j), k}, {}, k, win_type::self_drawn);
        fix_hand(h);
        fix_hand(h1);
    }
}

void write() {
    std::ofstream file("result_map.dat", std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开或创建文件 result_map.dat.\n";
        std::exit(1);
    }
    file.write(reinterpret_cast<const char*>(&wd.m_data.total_weight), sizeof wd.m_data.total_weight);
    for (const auto& [code, weight] : wd.m_data.weights) {
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