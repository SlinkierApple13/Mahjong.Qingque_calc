#include <fstream>
#include <iostream>
#include <assert.h>
#include <unordered_set>

#include "rules/qingque.hpp"
#include "result_map.h"

std::ofstream file("all_compositions.csv", std::ios::out);

qingque::w_data wd;
std::unordered_set<std::bitset<qingque::code_size>> used_codes;

std::string get_readable_code(const std::bitset<qingque::code_size>& code, std::size_t max_count = 10, bool derepellenise = true) {
    std::vector<std::string> fan_names;
    auto code1 = derepellenise ? qingque::derepellenise(code) : code;
    using enum qingque::indices;
    if (used_codes.find(code1) != used_codes.end()) return "";
    used_codes.insert(code1);
    for (int i = 0; i < qingque::fans.size(); ++i)
        if (code1[i]) fan_names.push_back(qingque::fans[i].name);
    assert(fan_names.size() <= max_count);
    std::string result;
    for (int i = 0; i < max_count; ++i) {
        if (i < fan_names.size()) result += fan_names[i];
        result += ',';
    }
    return result;
}

int main() {
    const int max_count = 9;
    // std::cin >> max_count;
    wd = qingque_wd::get_wd();
    for (const auto& [code, weight] : wd.weights) {
        auto s = get_readable_code(code, max_count);
        if (s.empty()) continue;
        if (s == "十二归,,,,,,,,,") {
            std::cout << get_readable_code(code, max_count, false) << "\n";
            continue;
        }
        file << s << ',' << qingque::get_fan(wd, qingque::derepellenise(code)) << ',' << qingque::get_weight(wd, qingque::derepellenise(code)) << '\n';
    }
    file.close();
    return 0;
}