#include <iostream>
// #include <fstream>

#include "rules/qingque.hpp"
#include "result_map.h"

using namespace mahjong::tile_literals;
using namespace mahjong::honours;

qingque::w_data wd;

// namespace qingque_wd {
//     qingque::w_data wd;
//     auto get_wd() {
//         std::ifstream file("result_map.dat", std::ios::in | std::ios::binary);
//         if (!file.is_open()) {
//             std::cerr << "无法打开文件 result_map.dat.\n";
//             std::exit(1);
//         }
//         uint64_t total_weight;
//         file.read(reinterpret_cast<char*>(&total_weight), sizeof total_weight);
//         wd.total_weight = total_weight;
//         while (!file.eof()) {
//             std::bitset<qingque::code_size> code;
//             uint64_t weight;
//             file.read(reinterpret_cast<char*>(&code), sizeof code);
//             file.read(reinterpret_cast<char*>(&weight), sizeof weight);
//             wd.weights.insert({code, weight});
//         }
//         file.close();
//         return wd;
//     }
// }

bool calculate(const std::string& s0 = "") {
    std::string str;
    if (s0 == "") {
        std::cout << "> ";
        std::getline(std::cin, str);
    } else str = s0;
    if (str.empty()) return false;
    auto h = mahjong::utils::parse_hand(str, 0u, 0u, true, [](char c, mahjong::win_t& w) {
        switch (c) {
            case '%': w |= mahjong::win_type::self_drawn; return true;
            case '^': w |= mahjong::win_type::kong_related; return true;
            case '&': w |= mahjong::win_type::final_tile; return true;
            case '*': w |= mahjong::win_type::heavenly_or_earthly_hand; return true;
            case '!': w = (w & 0b1111111111111100u) | 0b00u; return true;
            case '@': w = (w & 0b1111111111111100u) | 0b01u; return true;
            case '#': w = (w & 0b1111111111111100u) | 0b10u; return true;
            case '$': w = (w & 0b1111111111111100u) | 0b11u; return true;
            default: return false;
        }
    });
    if (!qingque::input_verifier(h)) {
        std::cout << "- 无效输入.\n" << std::endl;
        return true;
    }
    if (!qingque::is_winning_hand(h)) {
        std::cout << "- 此牌不能和牌.\n" << std::endl;
        return true;
    }
    // auto res = qingque::get_weight(wd, h);
    // double fan = std::log2((double)wd.total_weight) - std::log2((double)res.first);
    // auto res2 = qingque::derepellenise(res.second);
    // // auto res2 = res.second;
    // using enum qingque::indices;
    // fan += 18.0 * (res2[heavenly_hand] + res2[earthly_hand]) + 3.0 * (res2[out_with_replacement_tile] + res2[last_tile_draw] + res2[last_tile_claim] + res2[robbing_the_kong]);
    auto res = qingque::get_fan(wd, h);
    auto res2 = qingque::derepellenise(res.second);
    double fan = res.first;
    int fan_count = 0;
    for (int i = 0; i < qingque::code_size; ++i)
        if (res2[i]) {
            std::cout << qingque::fans[i].name << ", ";
            ++fan_count;
        }
    if (fan_count) std::cout << "\b\b; " << std::endl;
    else std::cout << "平和; " << std::endl;
    int sd_pt = std::round(fan * fan);
    int oo_pt = std::round(fan * 10.0 * std::tanh(fan * 0.1));
    int so_pt = 3 * sd_pt - 2 * oo_pt;
    std::string pt_display = h.winning_type()(mahjong::win_type::self_drawn) ? ("各 " + std::to_string(sd_pt) + "\'") : 
        (std::to_string(oo_pt) + "\'·" + std::to_string(so_pt) + "\'");
    std::printf("共 %.2f 番 (%s).\n\n", fan, pt_display.c_str());
    return true;
}

int main() {
#ifdef _WIN32
    setlocale(LC_ALL, "zh_CN.UTF-8");
#endif
    wd = qingque_wd::get_wd();
    std::cout << "[青雀 第 9 版 计算器]\n\n";
    std::cout << "牌张：123456789m/p/s, ESWNCFP\n";
    std::cout << "门风:  ! - 東,    @ - 南,    # - 西,    $ - 北\n";
    std::cout << "和牌:  % - 自摸,  ^ - 岭上开花/抢杠,  & - 海底捞月/河底捞鱼,  * - 天和/地和\n";
    std::cout << "副露: [] - 暗杠, () - 其余\n\n输入的最后一张牌视为和张.\n\n";
    std::cout << "例: [WWWW](123m)789m78999s#%&\n";
    calculate("[WWWW](123m)789m78999s#%&");
    while (calculate());
    return 0;
}