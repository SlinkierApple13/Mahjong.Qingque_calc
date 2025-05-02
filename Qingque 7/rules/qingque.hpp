#ifndef QINGQUE_HPP
#define QINGQUE_HPP

#include <math.h>
#include <unordered_map>

#include "../basic/mahjong.hpp"
#include "../basic/mahjong_utils.hpp"
#include "qingque_data.hpp"

namespace qingque {

    using namespace mahjong;

    namespace patterns {

        constexpr uint64_t nine_gates_m_s = 0b011001001001001001001001011000ull;
        constexpr uint64_t nine_gates_p = 0b011001001001001001001001011000ull << 32;

        using namespace tile_literals;
        using enum meld_type;

        constexpr std::initializer_list<std::initializer_list<meld_t>> mixed_shifted_triplets = {
            {triplet | 1_m, triplet | 2_p, triplet | 3_s}, {triplet | 2_m, triplet | 3_p, triplet | 4_s}, {triplet | 3_m, triplet | 4_p, triplet | 5_s}, {triplet | 4_m, triplet | 5_p, triplet | 6_s},
            {triplet | 5_m, triplet | 6_p, triplet | 7_s}, {triplet | 6_m, triplet | 7_p, triplet | 8_s}, {triplet | 7_m, triplet | 8_p, triplet | 9_s},
            {triplet | 1_p, triplet | 2_s, triplet | 3_m}, {triplet | 2_p, triplet | 3_s, triplet | 4_m}, {triplet | 3_p, triplet | 4_s, triplet | 5_m}, {triplet | 4_p, triplet | 5_s, triplet | 6_m},
            {triplet | 5_p, triplet | 6_s, triplet | 7_m}, {triplet | 6_p, triplet | 7_s, triplet | 8_m}, {triplet | 7_p, triplet | 8_s, triplet | 9_m},
            {triplet | 1_s, triplet | 2_m, triplet | 3_p}, {triplet | 2_s, triplet | 3_m, triplet | 4_p}, {triplet | 3_s, triplet | 4_m, triplet | 5_p}, {triplet | 4_s, triplet | 5_m, triplet | 6_p},
            {triplet | 5_s, triplet | 6_m, triplet | 7_p}, {triplet | 6_s, triplet | 7_m, triplet | 8_p}, {triplet | 7_s, triplet | 8_m, triplet | 9_p},
            {triplet | 1_m, triplet | 2_s, triplet | 3_p}, {triplet | 2_m, triplet | 3_s, triplet | 4_p}, {triplet | 3_m, triplet | 4_s, triplet | 5_p}, {triplet | 4_m, triplet | 5_s, triplet | 6_p},
            {triplet | 5_m, triplet | 6_s, triplet | 7_p}, {triplet | 6_m, triplet | 7_s, triplet | 8_p}, {triplet | 7_m, triplet | 8_s, triplet | 9_p},
            {triplet | 1_p, triplet | 2_m, triplet | 3_s}, {triplet | 2_p, triplet | 3_m, triplet | 4_s}, {triplet | 3_p, triplet | 4_m, triplet | 5_s}, {triplet | 4_p, triplet | 5_m, triplet | 6_s},
            {triplet | 5_p, triplet | 6_m, triplet | 7_s}, {triplet | 6_p, triplet | 7_m, triplet | 8_s}, {triplet | 7_p, triplet | 8_m, triplet | 9_s},
            {triplet | 1_s, triplet | 2_p, triplet | 3_m}, {triplet | 2_s, triplet | 3_p, triplet | 4_m}, {triplet | 3_s, triplet | 4_p, triplet | 5_m}, {triplet | 4_s, triplet | 5_p, triplet | 6_m},
            {triplet | 5_s, triplet | 6_p, triplet | 7_m}, {triplet | 6_s, triplet | 7_p, triplet | 8_m}, {triplet | 7_s, triplet | 8_p, triplet | 9_m}
        };

        constexpr std::initializer_list<std::initializer_list<meld_t>> mixed_shifted_sequences = {
            {2_m, 3_p, 4_s}, {3_m, 4_p, 5_s}, {4_m, 5_p, 6_s}, {5_m, 6_p, 7_s}, {6_m, 7_p, 8_s},
            {2_p, 3_s, 4_m}, {3_p, 4_s, 5_m}, {4_p, 5_s, 6_m}, {5_p, 6_s, 7_m}, {6_p, 7_s, 8_m},
            {2_s, 3_m, 4_p}, {3_s, 4_m, 5_p}, {4_s, 5_m, 6_p}, {5_s, 6_m, 7_p}, {6_s, 7_m, 8_p},
            {2_m, 3_s, 4_p}, {3_m, 4_s, 5_p}, {4_m, 5_s, 6_p}, {5_m, 6_s, 7_p}, {6_m, 7_s, 8_p},
            {2_p, 3_m, 4_s}, {3_p, 4_m, 5_s}, {4_p, 5_m, 6_s}, {5_p, 6_m, 7_s}, {6_p, 7_m, 8_s},
            {2_s, 3_p, 4_m}, {3_s, 4_p, 5_m}, {4_s, 5_p, 6_m}, {5_s, 6_p, 7_m}, {6_s, 7_p, 8_m}
        };

        constexpr std::initializer_list<std::initializer_list<meld_t>> mixed_chained_sequences = {
            {2_m, 4_p, 6_s}, {3_m, 5_p, 7_s}, {4_m, 6_p, 8_s},
            {2_p, 4_s, 6_m}, {3_p, 5_s, 7_m}, {4_p, 6_s, 8_m},
            {2_s, 4_m, 6_p}, {3_s, 5_m, 7_p}, {4_s, 6_m, 8_p},
            {2_m, 4_s, 6_p}, {3_m, 5_s, 7_p}, {4_m, 6_s, 8_p},
            {2_p, 4_m, 6_s}, {3_p, 5_m, 7_s}, {4_p, 6_m, 8_s},
            {2_s, 4_p, 6_m}, {3_s, 5_p, 7_m}, {4_s, 6_p, 8_m}
        };

        constexpr std::initializer_list<std::initializer_list<meld_t>> mixed_straight = {
            {2_m, 5_p, 8_s}, {2_p, 5_s, 8_m}, {2_s, 5_m, 8_p},
            {2_m, 5_s, 8_p}, {2_p, 5_m, 8_s}, {2_s, 5_p, 8_m}
        };

        constexpr std::initializer_list<std::initializer_list<meld_t>> mirrored_short_straights = {
            {2_m, 5_m, 2_p, 5_p}, {3_m, 6_m, 3_p, 6_p}, {4_m, 7_m, 4_p, 7_p}, {5_m, 8_m, 5_p, 8_p}, {2_m, 8_m, 2_p, 8_p},
            {2_p, 5_p, 2_s, 5_s}, {3_p, 6_p, 3_s, 6_s}, {4_p, 7_p, 4_s, 7_s}, {5_p, 8_p, 5_s, 8_s}, {2_p, 8_p, 2_s, 8_s},
            {2_s, 5_s, 2_m, 5_m}, {3_s, 6_s, 3_m, 6_m}, {4_s, 7_s, 4_m, 7_m}, {5_s, 8_s, 5_m, 8_m}, {2_s, 8_s, 2_m, 8_m}
        };

        constexpr uint64_t knitted_tiles = 0b001000000001000000001ull;
        constexpr uint64_t honours = 0b001001001001001001001000ull << 32;

        const std::initializer_list<tile_counter> honours_and_knitted_tiles = {
            tile_counter((knitted_tiles << 3) + (knitted_tiles << (32 + 6)), honours + (knitted_tiles << 9)),
            tile_counter((knitted_tiles << 6) + (knitted_tiles << (32 + 9)), honours + (knitted_tiles << 3)),
            tile_counter((knitted_tiles << 9) + (knitted_tiles << (32 + 3)), honours + (knitted_tiles << 6)),
            tile_counter((knitted_tiles << 3) + (knitted_tiles << (32 + 9)), honours + (knitted_tiles << 6)),
            tile_counter((knitted_tiles << 6) + (knitted_tiles << (32 + 3)), honours + (knitted_tiles << 9)),
            tile_counter((knitted_tiles << 9) + (knitted_tiles << (32 + 6)), honours + (knitted_tiles << 3))
        };

    }

    struct tag {
        bool special_compatible = false;
        bool is_special = false;
        bool is_occasional = false;
        uint8_t weight_20 = 20u; // 20 * fan_weight
        uint8_t fan_value = 0u;

        tag(bool special_compatible = false, bool is_special = false, bool is_occasional = false, uint8_t weight_20 = 20u, uint8_t fan_value = 0u) :
            special_compatible(special_compatible), is_special(is_special), is_occasional(is_occasional), weight_20(weight_20), fan_value(fan_value) {}
        tag(unsigned int weight_20) : special_compatible(false), is_special(false), is_occasional(false), weight_20(weight_20), fan_value(0u) {}
    };

    using fan = scoring_element<uint8_t, tag>;

    const verifier is_seven_pairs([](const hand& h) {
        if (h.melds().size()) return false;
        for (tile_t ti : tile_set::all_tiles)
            if (h.counter().count(ti) & 1) return false;
        return true;
    });

    const verifier is_thirteen_orphans([](const hand& h) {
        if (h.melds().size()) return false;
        for (tile_t ti : tile_set::terminal_honour_tiles)
            if (!h.counter().count(ti)) return false;
        for (tile_t ti : tile_set::simple_tiles)
            if (h.counter().count(ti)) return false;
        return true;
    });

    const verifier is_honours_and_knitted_tiles([](const hand& h) {
        if (h.melds().size()) return false;
        for (const auto& p : patterns::honours_and_knitted_tiles)
            if (h.counter() <= p) return true;
        return false;
    });

    const verifier input_verifier([](const hand& h) {
        if (!h.is_valid()) return false;
        uint8_t kong_count = 0;
        for (const auto& m : h.melds())
            if (m.type() == meld_type::kong) ++kong_count;
        if (kong_count == 0 && h.winning_type()(win_type::kong_related | win_type::self_drawn)) return false;
        if (h.melds().size() && h.winning_type()(win_type::heavenly_or_earthly_hand)) return false;
        if (h.counter().count(h.winning_tile()) > 1 && h.winning_type()(win_type::kong_related, win_type::self_drawn)) return false;
        if (h.winning_type()(win_type::final_tile | win_type::heavenly_or_earthly_hand)) return false;
        return true;
    });

    const verifier is_winning_hand([](const hand& h) {
        if (!input_verifier(h)) return false;
        if (h.decompose().size()) return true;
        if (is_seven_pairs(h)) return true;
        return false;
    });

    using namespace tile_literals;

    namespace criteria {

        using res_t = uint8_t;
        using res_v = std::vector<uint8_t>;
        constexpr res_t heavenly_hand(const hand& h) {
            return h.winning_type()(win_type::heavenly_or_earthly_hand | win_type::self_drawn);
        }

        constexpr res_t earthly_hand(const hand& h) {
            return h.winning_type()(win_type::heavenly_or_earthly_hand, win_type::self_drawn);
        }

        constexpr res_t out_with_replacement_tile(const hand& h) {
            return h.winning_type()(win_type::kong_related | win_type::self_drawn);
        }

        constexpr res_t last_tile_draw(const hand& h) {
            return h.winning_type()(win_type::final_tile | win_type::self_drawn);
        }

        constexpr res_t last_tile_claim(const hand& h) {
            return h.winning_type()(win_type::final_tile, win_type::self_drawn);
        }

        constexpr res_t robbing_the_kong(const hand& h) {
            return h.winning_type()(win_type::kong_related, win_type::self_drawn);
        }

        constexpr res_t self_drawn(const hand& h) {
            return h.winning_type()(win_type::self_drawn);
        }

        constexpr res_t concealed_hand(const hand& h) {
            for (const auto& m : h.melds())
                if (!m.concealed()) return false;
            return true;
        }

        res_v four_concealed_kongs(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (const meld& m : d.melds())
                    if (!m.concealed() || m.type() != meld_type::kong) return false;
                return true;
            });
        }

        res_v three_concealed_kongs(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() == meld_type::kong) ++counter;
                return counter >= 3;
            });
        }

        res_v two_concealed_kongs(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() == meld_type::kong) ++counter;
                return counter >= 2;
            });
        }

        res_v concealed_kong(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() == meld_type::kong) ++counter;
                return counter >= 1;
            });
        }

        constexpr res_t four_kongs(const hand& h) {
            return h.counter().count() >= 18;
        }

        constexpr res_t three_kongs(const hand& h) {
            return h.counter().count() >= 17;
        }

        constexpr res_t two_kongs(const hand& h) {
            return h.counter().count() >= 16;
        }

        constexpr res_t kong(const hand& h) {
            return h.counter().count() >= 15;
        }

        res_v four_concealed_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (const meld& m : d.melds())
                    if (!m.concealed() || m.type() == meld_type::sequence) return false;
                return true;
            });
        }

        res_v three_concealed_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() > meld_type::sequence) ++counter;
                return counter >= 3;
            });
        }

        res_v two_concealed_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() > meld_type::sequence) ++counter;
                return counter >= 2;
            });
        }

        res_v concealed_triplet(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t counter = 0;
                for (const meld& m : d.melds())
                    if (m.concealed() && m.type() > meld_type::sequence) ++counter;
                return counter >= 1;
            });
        }

        res_v all_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (const meld& m : d.melds())
                    if (m.type() == meld_type::sequence) return false;
                return true;
            });
        }

        constexpr res_t all_honours(const hand& h) {
            for (tile_t ti : tile_set::numbered_tiles)
            if (h.counter().count(ti)) return false;
            return true;
        }

        constexpr res_t big_four_winds(const hand& h) {
            for (tile_t ti : tile_set::wind_tiles)
            if (h.counter().count(ti) < 3) return false;
            return true;
        }

        constexpr res_t little_four_winds(const hand& h) {
            uint8_t count = 0u;
            for (tile_t ti : tile_set::wind_tiles)
            count += (h.counter().count(ti) >= 3) + (h.counter().count(ti) >= 2);
            return count == 7;
        }

        constexpr res_t big_three_dragons(const hand& h) {
            for (tile_t ti : tile_set::dragon_tiles)
            if (h.counter().count(ti) < 3) return false;
            return true;
        }

        constexpr res_t little_three_dragons(const hand& h) {
            uint8_t count = 0u;
            for (tile_t ti : tile_set::dragon_tiles)
            count += (h.counter().count(ti) >= 3) + (h.counter().count(ti) >= 2);
            return count == 5;
        }

        constexpr res_t fan_tile_1p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            uint8_t cnt = 0;
            for (tile_t ti : fan_tiles)
                cnt += (h.counter().count(ti) == 2);
            return cnt >= 1;
        }

        constexpr res_t fan_tile_2p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 2;
        }

        constexpr res_t fan_tile_3p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 3;
        }

        constexpr res_t fan_tile_4p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 4;
        }

        constexpr res_t fan_tile_5p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 5;
        }

        constexpr res_t fan_tile_6p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 6;
        }

        constexpr res_t fan_tile_7p(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            return utils::count_pair_of(h, fan_tiles) >= 7;
        }

        constexpr res_t fan_tile_1t(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            uint8_t cnt = 0;
            for (tile_t ti : fan_tiles)
                cnt += (h.counter().count(ti) >= 3);
            return cnt >= 1;
        }

        constexpr res_t fan_tile_2t(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            uint8_t cnt = 0;
            for (tile_t ti : fan_tiles)
                cnt += (h.counter().count(ti) >= 3);
            return cnt >= 2;
        }

        constexpr res_t fan_tile_3t(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            uint8_t cnt = 0;
            for (tile_t ti : fan_tiles)
                cnt += (h.counter().count(ti) >= 3);
            return cnt >= 3;
        }

        constexpr res_t fan_tile_4t(const hand& h) {
            const std::array<tile_t, 4> fan_tiles = {h.winning_type().seat_wind(), honours::C, honours::F, honours::P};
            uint8_t cnt = 0;
            for (tile_t ti : fan_tiles)
                cnt += (h.counter().count(ti) >= 3);
            return cnt >= 4;
        }

        constexpr res_t all_terminals(const hand& h) {
            for (tile_t ti : tile_set::all_tiles)
            if (h.counter().count(ti) && tile(ti).num() != 1 && tile(ti).num() != 9) return false;
            return true;
        }

        constexpr res_t all_terminals_and_honours(const hand& h) {
            for (tile_t ti : tile_set::simple_tiles)
                if (h.counter().count(ti)) return false;
            return true;
        }

        res_v pure_outside_hand(const hand& h) {
            auto poh_check = [](const hand::decomposition& d) {
                if (!d.pair().is_in(tile_set::terminal_tiles)) return false;
                for (const meld& m : d.melds())
                    if (!m.contains(tile_set::terminal_tiles)) return false;
                return true;
            };
            return utils::for_all_decompositions<res_t>(h, poh_check);
        }

        res_v mixed_outside_hand(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                if (!d.pair().is_in(tile_set::terminal_honour_tiles)) return false;
                for (const meld& m : d.melds())
                    if (!m.contains(tile_set::terminal_honour_tiles)) return false;
                return true;
            });
        }

        constexpr res_t nine_gates(const hand& h) {
            auto c = h.counter(false);
            c.add(h.winning_tile(), -1);
            return (c == std::make_pair(patterns::nine_gates_m_s, 0ull) || c == std::make_pair(0ull, patterns::nine_gates_m_s) || c == std::make_pair(patterns::nine_gates_p, 0ull));
        }

        constexpr res_t full_flush(const hand& h) {
            for (tile_t ti : tile_set::honour_tiles)
                if (h.counter().count(ti)) return false;
            auto check_suit = [](const hand& h, suit_type st) {
                for (tile_t ti : tile_set::tiles_of_suit(st))
                    if (h.counter().count(ti)) return true;
                return false;
            };
            return (check_suit(h, suit_type::m) + check_suit(h, suit_type::p) + check_suit(h, suit_type::s) == 1);
        }

        constexpr res_t half_flush(const hand& h) {
            auto check_suit = [](const hand& h, suit_type st) {
                for (tile_t ti : tile_set::tiles_of_suit(st))
                    if (h.counter().count(ti)) return true;
                return false;
            };
            return (check_suit(h, suit_type::m) + check_suit(h, suit_type::p) + check_suit(h, suit_type::s) <= 1);
        }

        // constexpr res_t two_suits(const hand& h) {
        //     // for (tile_t ti : tile_set::honour_tiles)
        //     //     if (h.counter().count(ti)) return false;
        //     auto check_suit = [](const hand& h, suit_type st) {
        //         for (tile_t ti : tile_set::tiles_of_suit(st))
        //             if (h.counter().count(ti)) return true;
        //         return false;
        //     };
        //     return (check_suit(h, suit_type::m) + check_suit(h, suit_type::p) + check_suit(h, suit_type::s) <= 2);
        // }

        constexpr res_t all_types(const hand& h) {
            if (!h.counter().count(tile_set::character_tiles)) return false;
            if (!h.counter().count(tile_set::bamboo_tiles)) return false;
            if (!h.counter().count(tile_set::dot_tiles)) return false;
            if (!h.counter().count(tile_set::wind_tiles)) return false;
            if (!h.counter().count(tile_set::dragon_tiles)) return false;
            return true;
        }

        constexpr res_t two_numbers(const hand& h) {
            uint16_t num_table = 0u;
            for (tile_t ti : tile_set::all_tiles)
                if (h.counter().count(ti)) {
                    if (tile(ti).num() == 0) return false;
                    num_table |= 1 << tile(ti).num();
                }
            return utils::popcount(num_table) == 2u;
        }

        constexpr res_t three_consecutive_numbers(const hand& h) {
            uint16_t num_table = 0u;
            for (tile_t ti : tile_set::all_tiles)
                if (h.counter().count(ti))
                    num_table |= 1 << tile(ti).num();
            return num_table == 0b1110u || num_table == 0b11100u || num_table == 0b111000u || num_table == 0b1110000u || 
                num_table == 0b11100000u || num_table == 0b111000000u || num_table == 0b1110000000u;
        }

        constexpr res_t four_consecutive_numbers(const hand& h) {
            uint16_t num_table = 0u;
            for (tile_t ti : tile_set::all_tiles)
                if (h.counter().count(ti))
                    num_table |= 1 << tile(ti).num();
            return num_table == 0b11110u || num_table == 0b111100u || num_table == 0b1111000u || num_table == 0b11110000u || num_table == 0b111100000u || 
                num_table == 0b1111000000u || num_table == 0b1110u || num_table == 0b11100u || num_table == 0b111000u || num_table == 0b1110000u || 
                num_table == 0b11100000u || num_table == 0b111000000u || num_table == 0b1110000000u;
        }

        res_v nine_numbers(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint32_t num_table = 0u;
                for (const meld& m : d.melds())
                    if (m.type() == meld_type::sequence)
                        num_table += 0b001001001u << ((m.tile().num() - 1) * 3);
                    else num_table += 0b001u << (m.tile().num() * 3);
                num_table += (0b001u << (d.pair().num() * 3));
                return num_table == 153391688u;
            });
        }

        res_v common_number(const hand& h) {
            for (tile_t ti : tile_set::honour_tiles)
            if (h.counter().count(ti)) return std::vector<uint8_t>(h.decompose().size(), 0u);
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                num_t num = d.pair().num();
                for (const meld& m : d.melds())
                    if (!m.contains(tile_set::tiles_of_number(num))) return false;
                return true;
            });
        }

        res_v quadruple_sequence(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (const meld& m : d.melds())
                    if (!utils::is_equivalent(m, d.melds()[0])) return false;
                return true;
            });
        }

        res_v triple_sequence(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_equivalent(d.melds()[i], d.melds()[j])) ++count;
                return (count >= 3u);
            });
        }

        res_v two_double_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_equivalent(d.melds()[i], d.melds()[j])) ++count;
                return (count >= 2u);
            });
        }

        res_v double_sequence(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_equivalent(d.melds()[i], d.melds()[j])) ++count;
                return (count >= 1u);
            });
        }
        
        res_v mixed_triple_triplet(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_triplet(d.melds()[i], d.melds()[j])) ++count;
                return count == 3u;
            });
        }

        res_v mixed_double_triplet(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_triplet(d.melds()[i], d.melds()[j])) ++count;
                return count >= 1u;
            });
        }

        res_v two_mixed_double_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_triplet(d.melds()[i], d.melds()[j])) ++count;
                return count >= 2u;
            });
        }

        res_v mixed_triple_sequence(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 0b00100000u, meld_type::sequence), meld(ti + 0b10000000u, meld_type::sequence)})) 
                        return true;
                return false;
            });
        }

        res_v two_mixed_double_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                uint8_t visited = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_sequence(d.melds()[i], d.melds()[j])) {
                            count += !(visited & ((1 << i) + (1 << j)));
                            if (!(visited & ((1 << i) + (1 << j)))) visited |= (1 << i) | (1 << j);
                        }
                return count >= 2u;
            });
        }

        res_v mixed_double_sequence(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                uint8_t visited = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_sequence(d.melds()[i], d.melds()[j])) {
                            count += !(visited & ((1 << i) + (1 << j)));
                            if (!(visited & ((1 << i) + (1 << j)))) visited |= (1 << i) | (1 << j);
                        }
                return count >= 1u;
            });
        }

        res_v four_shifted_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 1_p, 2_p, 3_p, 4_p, 5_p, 6_p, 1_s, 2_s, 3_s, 4_s, 5_s, 6_s})
                    if (utils::contains(d, {meld(ti, meld_type::triplet), meld(ti + 1, meld_type::triplet), meld(ti + 2, meld_type::triplet), meld(ti + 3, meld_type::triplet)})) 
                        return true;
                return false;
            });
        }

        res_v three_shifted_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 1_p, 2_p, 3_p, 4_p, 5_p, 6_p, 7_p, 1_s, 2_s, 3_s, 4_s, 5_s, 6_s, 7_s})
                    if (utils::contains(d, {meld(ti, meld_type::triplet), meld(ti + 1, meld_type::triplet), meld(ti + 2, meld_type::triplet)})) 
                        return true;
                return false;
            });
        }

        res_v four_shifted_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 3_m, 4_m, 5_m, 2_p, 3_p, 4_p, 5_p, 2_s, 3_s, 4_s, 5_s})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 1, meld_type::sequence), meld(ti + 2, meld_type::sequence), meld(ti + 3, meld_type::sequence)})) 
                    return true;
                return false;
            });
        }

        res_v three_shifted_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 3_m, 4_m, 5_m, 6_m, 2_p, 3_p, 4_p, 5_p, 6_p, 2_s, 3_s, 4_s, 5_s, 6_s, 7_s})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 1, meld_type::sequence), meld(ti + 2, meld_type::sequence)})) 
                        return true;
                return false;
            });
        }

        res_v four_chained_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 2_p, 2_s})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 2, meld_type::sequence), meld(ti + 4, meld_type::sequence), meld(ti + 6, meld_type::sequence)})) 
                        return true;
                return false;
            });
        }

        res_v three_chained_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 2_p, 2_s, 3_m, 3_p, 3_s, 4_m, 4_p, 4_s})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 2, meld_type::sequence), meld(ti + 4, meld_type::sequence)})) 
                        return true;
                return false;
            });
        }

        res_v pure_straight(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (tile_t ti : {2_m, 2_p, 2_s})
                    if (utils::contains(d, {meld(ti, meld_type::sequence), meld(ti + 3, meld_type::sequence), meld(ti + 6, meld_type::sequence)})) 
                        return true;
                return false;
            });
        }

        res_v two_short_straights(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                uint8_t visited = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_shifted_sequences(d.melds()[i], d.melds()[j], 3)) {
                            count += !(visited & ((1 << i) + (1 << j)));
                            if (!(visited & ((1 << i) + (1 << j)))) visited |= (1 << i) | (1 << j);
                        }
                if (count == 2u) return true;
                return false;
            });
        }

        res_v short_straight(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                uint8_t count = 0u;
                uint8_t visited = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_shifted_sequences(d.melds()[i], d.melds()[j], 3)) {
                            count += !(visited & ((1 << i) + (1 << j)));
                            visited |= (1 << i) | (1 << j);
                        }
                return count >= 1u;
            });
        }

        res_v two_terminal_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_shifted_sequences(d.melds()[i], d.melds()[j], 6))
                            return true;
                return false;
            });
        }

        res_v mixed_shifted_triplets(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (auto& seq : patterns::mixed_shifted_triplets)
                    if (utils::contains(d, seq)) return true;
                return false;
            });
        }

        res_v mixed_shifted_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (auto& seq : patterns::mixed_shifted_sequences)
                    if (utils::contains(d, seq)) return true;
                return false;
            });
        }

        res_v mixed_chained_sequences(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (auto& seq : patterns::mixed_chained_sequences)
                    if (utils::contains(d, seq)) return true;
                return false;
            });
        }

        res_v mixed_straight(const hand& h) {
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d) {
                for (auto& seq : patterns::mixed_straight)
                    if (utils::contains(d, seq)) return true;
                return false;
            });
        }

        constexpr res_t seven_pairs(const hand& h) {
            return is_seven_pairs(h);
        }

        constexpr res_t big_seven_honours(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            return utils::contains_pair_of(h, tile_set::honour_tiles);
        }

        constexpr res_t four_wind_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            return utils::contains_pair_of(h, tile_set::wind_tiles);
        }

        constexpr res_t three_dragon_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            return utils::contains_pair_of(h, tile_set::dragon_tiles);
        }

        constexpr res_t six_dragon_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            return utils::count_pair_of(h, tile_set::dragon_tiles) == 6u;
        }

        constexpr res_t seven_shifted_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            for (tile_t ti : {1_m, 2_m, 3_m, 1_p, 2_p, 3_p, 1_s, 2_s, 3_s})
                if (utils::contains_pair_of(h, {
                    static_cast<tile_t>(ti), static_cast<tile_t>(ti + 1), static_cast<tile_t>(ti + 2), static_cast<tile_t>(ti + 3), 
                    static_cast<tile_t>(ti + 4), static_cast<tile_t>(ti + 5), static_cast<tile_t>(ti + 6)
                })) return true;
            return false;
        }

        constexpr res_t reflected_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            const auto& c = h.counter();
            for (tile_t ti : tile_set::honour_tiles)
                if (c.count(ti)) return false;
            uint8_t min_num = 10u, max_num = 0u;
            for (tile_t ti : tile_set::numbered_tiles)
                if (c.count(ti)) {
                    min_num = std::min(min_num, tile(ti).num());
                    max_num = std::max(max_num, tile(ti).num());
                }
            const uint8_t ref = min_num + max_num;
            for (tile_t t : tile_set::numbered_tiles)
                if (c.count(t) && c.count(t) != c.count((t & 0b11110000) | (ref - (t & 0b00001111)))) return false;
            return true;
        }

        constexpr res_t two_triple_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            auto m = tile_literals::operator""_m;
            auto p = tile_literals::operator""_p;
            auto s = tile_literals::operator""_s;
            for (uint8_t i = 1; i <= 8; ++i)
                for (uint8_t j = i + 1; j <= 9; ++j)
                    if (utils::contains_pair_of(h, {m(i), p(i), s(i), m(j), p(j), s(j)})) return true;
            for (uint8_t i = 1; i <= 9; ++i)
                if (utils::count_pair_of(h, {m(i), p(i), s(i)}) == 6u) return true;
            return false;
        }

        constexpr res_t three_mirrored_pairs(const hand& h) {
            if (!is_seven_pairs(h)) return false;
            auto m = tile_literals::operator""_m;
            auto p = tile_literals::operator""_p;
            auto s = tile_literals::operator""_s;
            for (uint8_t i = 1; i <= 7; ++i)
                for (uint8_t j = i + 1; j <= 8; ++j)
                    for (uint8_t k = j + 1; k <= 9; ++k) {
                        if (utils::contains_pair_of(h, {m(i), m(j), m(k), p(i), p(j), p(k)})) return true;
                        if (utils::contains_pair_of(h, {s(i), s(j), s(k), p(i), p(j), p(k)})) return true;
                        if (utils::contains_pair_of(h, {m(i), m(j), m(k), s(i), s(j), s(k)})) return true;
                    }
            for (uint8_t i = 1; i <= 8; ++i)
                for (uint8_t j = i + 1; j <= 9; ++j) {
                    if (utils::contains_pair_of(h, {m(i), p(i)}) && utils::count_pair_of(h, {m(j), p(j)}) == 4u) return true;
                    if (utils::contains_pair_of(h, {s(i), p(i)}) && utils::count_pair_of(h, {s(j), p(j)}) == 4u) return true;
                    if (utils::contains_pair_of(h, {m(i), s(i)}) && utils::count_pair_of(h, {m(j), s(j)}) == 4u) return true;
                    if (utils::contains_pair_of(h, {m(j), p(j)}) && utils::count_pair_of(h, {m(i), p(i)}) == 4u) return true;
                    if (utils::contains_pair_of(h, {s(j), p(j)}) && utils::count_pair_of(h, {s(i), p(i)}) == 4u) return true;
                    if (utils::contains_pair_of(h, {m(j), s(j)}) && utils::count_pair_of(h, {m(i), s(i)}) == 4u) return true;
                }
            return false;
        }

        constexpr res_t thirteen_orphans(const hand& h) {
            return is_thirteen_orphans(h);
        }

        constexpr res_t honours_and_knitted_tiles(const hand& h) {
            return is_honours_and_knitted_tiles(h);
        }

        constexpr res_t eight_hog(const hand& h) {
            uint8_t count = 0u;
            for (tile_t ti : tile_set::all_tiles)
                if (h.counter().count(ti) == 4) ++count;
            for (const auto& m : h.melds())
                if (m.type() == meld_type::kong) --count;
            return count >= 2u;
        }

        constexpr res_t twelve_hog(const hand& h) {
            uint8_t count = 0u;
            for (tile_t ti : tile_set::all_tiles)
                if (h.counter().count(ti) == 4) ++count;
            for (const auto& m : h.melds())
                if (m.type() == meld_type::kong) --count;
            return count == 3u;
        }

        constexpr res_t disabled(const hand&) {
            return false;
        }

        res_v reflected_hand(const hand& h) {
            if (h.decompose().size() == 0 && is_seven_pairs(h)) return {reflected_pairs(h)};
            for (tile_t ti : tile_set::honour_tiles)
                if (h.counter().count(ti)) return std::vector<uint8_t>(h.decompose().size(), 0u);
            uint8_t min_num = 10u, max_num = 0u;
            for (tile_t ti : tile_set::numbered_tiles)
                if (h.counter().count(ti)) {
                    min_num = std::min(min_num, tile(ti).num());
                    max_num = std::max(max_num, tile(ti).num());
                }
            const uint8_t ref = min_num + max_num;
            return utils::for_all_decompositions<res_t>(h, [ref](const hand::decomposition& d, const hand& hand) {
                for (const hand::decomposition& d2 : hand.decompose())
                    if (utils::is_reflection(d, d2, ref)) return true;
                return false;
            });
        }

        res_v mirrored_hand(const hand& h) {
            if (h.decompose().size() == 0 && is_seven_pairs(h)) return {three_mirrored_pairs(h)};
            return utils::for_all_decompositions<res_t>(h, [](const hand::decomposition& d, const hand& hand) {
                uint8_t suit_distribution = 0u;
                for (const meld& m : d.melds())
                    suit_distribution |= (1 << (m.tile().suit() >> 5));
                if (utils::popcount(suit_distribution) != 2u) return false;
                uint8_t count = 0u;
                uint8_t visited = 0u;
                for (uint8_t i = 0; i < d.melds().size(); ++i)
                    for (uint8_t j = i + 1; j < d.melds().size(); ++j)
                        if (utils::is_mixed_double_sequence(d.melds()[i], d.melds()[j]) || utils::is_mixed_double_triplet(d.melds()[i], d.melds()[j])) {
                            count += !(visited & ((1 << i) + (1 << j)));
                            if (!(visited & ((1 << i) + (1 << j)))) visited |= (1 << i) | (1 << j);
                        }
                return count == 2u;
            });
        }

    }

    const std::vector<fan> fans = {
        fan("天和", {1, 0, 1, 20, 15}, criteria::heavenly_hand),
        fan("地和", {1, 0, 1, 20, 15}, criteria::earthly_hand),
        fan("岭上开花", {0, 0, 1, 20, 3}, criteria::out_with_replacement_tile),
        fan("海底捞月", {1, 0, 1, 20, 3}, criteria::last_tile_draw),
        fan("河底捞鱼", {1, 0, 1, 20, 3}, criteria::last_tile_claim),
        fan("抢杠和", {1, 0, 1, 20, 3}, criteria::robbing_the_kong),
        fan("七对", {1, 1, 0, 20, 0}, criteria::seven_pairs),
        fan("门前清", {20u}, criteria::concealed_hand),
        fan("四暗杠", {23u}, criteria::four_concealed_kongs),
        fan("三暗杠", {20u}, criteria::three_concealed_kongs),
        fan("双暗杠", {16u}, criteria::two_concealed_kongs),
        fan("暗杠", {14u}, criteria::concealed_kong),
        fan("四杠", {22u}, criteria::four_kongs),
        fan("三杠", {20u}, criteria::three_kongs),
        fan("双杠", {15u}, criteria::two_kongs),
        fan("四暗刻", {21u}, criteria::four_concealed_triplets),
        fan("三暗刻", {20u}, criteria::three_concealed_triplets),
        fan("对对和", {19u}, criteria::all_triplets),
        fan("十二归", {1, 0, 0, 21, 0}, criteria::twelve_hog),
        fan("八归", {1, 0, 0, 16, 0}, criteria::eight_hog),
        fan("字一色", {1, 0, 0, 22, 0}, criteria::all_honours),
        fan("大四喜", {22u}, criteria::big_four_winds),
        fan("小四喜", {21u}, criteria::little_four_winds),
        fan("四喜对", {1, 1, 0, 21, 0}, criteria::four_wind_pairs),
        fan("大三元", {21u}, criteria::big_three_dragons),
        fan("小三元", {21u}, criteria::little_three_dragons),
        fan("三元六对", {1, 1, 0, 21, 0}, criteria::six_dragon_pairs),
        fan("三元对", {1, 1, 0, 20, 0}, criteria::three_dragon_pairs),
        fan("番牌刻", {15u}, criteria::fan_tile_1t),
        fan("番牌二刻", {16u}, criteria::fan_tile_2t),
        fan("番牌三刻", {18u}, criteria::fan_tile_3t),
        fan("番牌四刻", {21u}, criteria::fan_tile_4t),
        fan("番牌对", {1, 0, 0, 14, 0}, criteria::fan_tile_1p),
        fan("番牌二对", {1, 1, 0, 15, 0}, criteria::fan_tile_2p),
        fan("番牌三对", {1, 1, 0, 17, 0}, criteria::fan_tile_3p),
        fan("番牌四对", {1, 1, 0, 19, 0}, criteria::fan_tile_4p),
        fan("番牌五对", {1, 1, 0, 20, 0}, criteria::fan_tile_5p),
        fan("番牌六对", {1, 1, 0, 21, 0}, criteria::fan_tile_6p),
        fan("番牌七对", {1, 1, 0, 22, 0}, criteria::fan_tile_7p),
        fan("清幺九", {1, 0, 0, 22, 0}, criteria::all_terminals),
        fan("混幺九", {1, 0, 0, 20, 0}, criteria::all_terminals_and_honours),
        fan("清带幺", {20u}, criteria::pure_outside_hand),
        fan("混带幺", {20u}, criteria::mixed_outside_hand),
        fan("九莲宝灯", {23u}, criteria::nine_gates),
        fan("清一色", {1, 0, 0, 21, 0}, criteria::full_flush),
        fan("混一色", {1, 0, 0, 20, 0}, criteria::half_flush),
        fan("五门齐", {19u}, criteria::all_types),
        fan("二数", {1, 0, 0, 20, 0}, criteria::two_numbers),
        fan("三聚", {1, 0, 0, 20, 0}, criteria::three_consecutive_numbers),
        fan("四聚", {1, 0, 0, 20, 0}, criteria::four_consecutive_numbers),
        fan("九数贯通", {20u}, criteria::nine_numbers),
        fan("满庭芳", {20u}, criteria::common_number),
        fan("镜数", {1, 0, 0, 20, 0}, criteria::reflected_hand),
        fan("四同顺", {23u}, criteria::quadruple_sequence),
        fan("三同顺", {21u}, criteria::triple_sequence),
        fan("二般高", {20u}, criteria::two_double_sequences),
        fan("一般高", {18u}, criteria::double_sequence),
        fan("三色同刻", {20u}, criteria::mixed_triple_triplet),
        fan("三色同顺", {20u}, criteria::mixed_triple_sequence),
        fan("三色二对", {1, 1, 0, 20, 0}, criteria::two_triple_pairs),
        fan("镜同", {1, 0, 0, 20, 0}, criteria::mirrored_hand),
        fan("四连刻", {21u}, criteria::four_shifted_triplets),
        fan("三连刻", {20u}, criteria::three_shifted_triplets),
        fan("四步高", {21u}, criteria::four_shifted_sequences),
        fan("三步高", {20u}, criteria::three_shifted_sequences),
        fan("四连环", {21u}, criteria::four_chained_sequences),
        fan("三连环", {19u}, criteria::three_chained_sequences),
        fan("一气贯通", {21u}, criteria::pure_straight),
        fan("双龙会", {20u}, criteria::two_short_straights),
        fan("三色连刻", {17u}, criteria::mixed_shifted_triplets),
        fan("三色贯通", {19u}, criteria::mixed_straight)
    };

    enum indices {
        heavenly_hand, 
        earthly_hand,
        out_with_replacement_tile,
        last_tile_draw,
        last_tile_claim,
        robbing_the_kong,
        seven_pairs,
        concealed_hand,
        four_concealed_kongs,
        three_concealed_kongs,
        two_concealed_kongs,
        concealed_kong,
        four_kongs,
        three_kongs,
        two_kongs,
        four_concealed_triplets,
        three_concealed_triplets,
        all_triplets,
        twelve_hog,
        eight_hog,
        all_honours,
        big_four_winds,
        little_four_winds,
        four_wind_pairs,
        big_three_dragons,
        little_three_dragons,
        six_dragon_pairs,
        three_dragon_pairs,
        fan_tile_1t,
        fan_tile_2t,
        fan_tile_3t,
        fan_tile_4t,
        fan_tile_1p,
        fan_tile_2p,
        fan_tile_3p,
        fan_tile_4p,
        fan_tile_5p,
        fan_tile_6p,
        fan_tile_7p,
        all_terminals,
        all_terminals_and_honours,
        pure_outside_hand,
        mixed_outside_hand,
        nine_gates,
        full_flush,
        half_flush,
        all_types,
        two_numbers,
        three_consecutive_numbers,
        four_consecutive_numbers,
        nine_numbers,
        common_number,
        reflected_hand,
        quadruple_sequence,
        triple_sequence,
        two_double_sequences,
        double_sequence,
        mixed_triple_triplet,
        mixed_triple_sequence,
        two_triple_pairs,
        mirrored_hand,
        four_shifted_triplets,
        three_shifted_triplets,
        four_shifted_sequences,
        three_shifted_sequences,
        four_chained_sequences,
        three_chained_sequences,
        pure_straight,
        two_short_straights,
        mixed_shifted_triplets,
        mixed_straight
    };

    std::vector<std::bitset<code_size>> evaluate_fans(const hand& h, bool ignore_occ = false) {
        std::vector<criteria::res_v> fan_results;
        std::vector<std::bitset<code_size>> results;
        for (const auto& fan : fans)
            fan_results.push_back(fan(h));
        for (int i = 0; i < h.decompose().size(); ++i) {
            std::bitset<code_size> res;
            for (int j = 0; j < fan_results.size(); ++j) {
                if (fans[j].tag.is_special || (ignore_occ && fans[j].tag.is_occasional)) continue;
                if (fan_results[j].size() == 1) res[j] = fan_results[j][0];
                else res[j] = fan_results[j][i];   
            }
            results.push_back(res);
        }
        if (is_seven_pairs(h)) {
            std::bitset<code_size> res;
            for (int j = 0; j < fan_results.size(); ++j)
                if (fans[j].tag.special_compatible && (!ignore_occ || !fans[j].tag.is_occasional)) 
                    res[j] = fan_results[j][0];
            results.push_back(res);
        }
        return results;
    }

    void add_to_data(w_data& data, const hand& h, uint64_t weight) {
        auto v = evaluate_fans(h, true);
        data.total_weight += weight;
        for (const auto& res : v) {
            if (data.weights.find(res) == data.weights.end()) data.weights[res] = weight;
            data.weights[res] += weight;
        }
    }

    uint64_t get_weight(const w_data& data, const std::bitset<code_size>& res) {
        uint64_t weight = 0;
        for (const auto& [key, w] : data.weights)
            if ((res & key) == res) weight += w;
        return weight;
    }

    std::pair<uint64_t, std::bitset<code_size>> get_weight(const w_data& data, const hand& h) {
        auto v0 = evaluate_fans(h, false);
        auto v = v0;
        for (int i = 0; i <= indices::robbing_the_kong; ++i)
            for (int j = 0; j < v0.size(); ++j)
                v[j][i] = false;
        uint64_t min_weight = data.total_weight;
        std::bitset<code_size> min_res;
        for (const auto& res : v) {
            uint64_t weight = get_weight(data, res);
            if (weight < min_weight) { 
                min_weight = weight;
                min_res = res;
            }
        }
        for (int i = 0; i <= indices::robbing_the_kong; ++i)
            min_res[i] = v0[0][i];
        return {min_weight, min_res};
    }

    double get_fan(const w_data& data, const std::bitset<code_size>& res) {
        double fan = 0.0;
        auto res1 = res;
        for (int i = 0; i <= indices::robbing_the_kong; ++i) {
            fan += res1[i] ? fans[i].tag.fan_value : 0.0;
            res1[i] = false;
        }
        for (auto a = 14; a <= 23; ++a) {
            for (int i = 0; i <= indices::mixed_straight; ++i)
                if (fans[i].tag.weight_20 == a - 1) res1[i] = 0;
            double fan0 = std::log2(data.total_weight) - std::log2(get_weight(data, res1));
            fan += fan0 * (a == 14 ? 0.7 : 0.05);
        }
        return fan;
    }

    std::pair<double, std::bitset<code_size>> get_fan(const w_data& data, const hand& h) {
        auto v0 = evaluate_fans(h, false);
        auto v = v0;
        for (int i = 0; i <= indices::robbing_the_kong; ++i)
            for (int j = 0; j < v0.size(); ++j)
                v[j][i] = false;
        double max_fan = 0.0;
        std::bitset<code_size> max_res;
        for (const auto& res : v) {
            double fan = get_fan(data, res);
            if (fan > max_fan) { 
                max_fan = fan;
                max_res = res;
            }
        }
        for (int i = 0; i <= indices::robbing_the_kong; ++i) {
            max_res[i] = v0[0][i];
            max_fan += v0[0][i] ? fans[i].tag.fan_value : 0.0;
        }
        return {max_fan, max_res};
    }

    std::bitset<code_size> derepellenise(const std::bitset<code_size>& res) {
        std::bitset<code_size> new_res = res;
        using enum indices;
        new_res[concealed_hand] = new_res[concealed_hand] && (!res[four_concealed_triplets] && !res[nine_gates]);
        new_res[all_triplets] = new_res[all_triplets] && (!res[four_kongs] && !res[four_concealed_triplets] && !res[big_four_winds] && !res[four_shifted_triplets]);
        // new_res[concealed_triplet] = new_res[concealed_triplet] && (!res[four_concealed_triplets] && !res[three_concealed_triplets] && !res[two_concealed_triplets]);
        // new_res[two_concealed_triplets] = new_res[two_concealed_triplets] && (!res[four_concealed_triplets] && !res[three_concealed_triplets]);
        new_res[three_concealed_triplets] = new_res[three_concealed_triplets] && !res[four_concealed_triplets] && !res[three_concealed_kongs];
        new_res[kong] = new_res[kong] && (!res[four_kongs] && !res[three_kongs] && !res[two_kongs] && !res[concealed_kong]);
        new_res[two_kongs] = new_res[two_kongs] && (!res[four_kongs] && !res[three_kongs] && !res[two_concealed_kongs]);
        new_res[three_kongs] = new_res[three_kongs] && !res[four_kongs] && !res[three_concealed_kongs];
        new_res[four_kongs] = new_res[four_kongs] && !res[four_concealed_kongs];
        new_res[four_concealed_triplets] = new_res[four_concealed_triplets] && !res[four_concealed_kongs];
        new_res[concealed_kong] = new_res[concealed_kong] && !res[two_concealed_kongs];
        new_res[two_concealed_kongs] = new_res[two_concealed_kongs] && !res[three_concealed_kongs];
        new_res[three_concealed_kongs] = new_res[three_concealed_kongs] && !res[four_concealed_kongs];
        new_res[fan_tile_1p] = new_res[fan_tile_1p] && !res[little_three_dragons];
        new_res[fan_tile_3t] = new_res[fan_tile_3t] && !res[big_three_dragons] && !res[fan_tile_4t];
        new_res[fan_tile_2t] = new_res[fan_tile_2t] && !res[fan_tile_3t] && !res[fan_tile_4t] && !res[little_three_dragons];
        new_res[fan_tile_1t] = new_res[fan_tile_1t] && !res[big_four_winds] && !res[fan_tile_4t] && !res[fan_tile_3t] && !res[fan_tile_2t];
        new_res[fan_tile_1t] = new_res[fan_tile_1t] && !res[all_honours] && !res[fan_tile_4t] && !res[fan_tile_3t] && !res[fan_tile_2t];
        // if (res[fan_tile_4t]) {
        //     new_res[fan_tile_4t] = false;
        //     new_res[fan_tile_1t] = true;
        // }
        new_res[pure_outside_hand] = new_res[pure_outside_hand] && !res[all_terminals];
        new_res[mixed_outside_hand] = new_res[mixed_outside_hand] && (!res[pure_outside_hand] && !res[all_terminals_and_honours]);
        new_res[all_terminals_and_honours] = new_res[all_terminals_and_honours] && (!res[all_terminals] && !res[all_honours]);
        new_res[full_flush] = new_res[full_flush] && !res[nine_gates];
        new_res[half_flush] = new_res[half_flush] && !res[full_flush] && !res[all_honours] && !res[big_four_winds] && !res[little_four_winds] && !res[six_dragon_pairs];
        new_res[two_numbers] = new_res[two_numbers] && !res[all_terminals];
        new_res[four_consecutive_numbers] = new_res[four_consecutive_numbers] && !res[three_consecutive_numbers];
        new_res[triple_sequence] = new_res[triple_sequence] && !res[quadruple_sequence];
        new_res[two_double_sequences] = new_res[two_double_sequences] && !res[quadruple_sequence] && !res[triple_sequence];
        new_res[double_sequence] = new_res[double_sequence] && (!res[triple_sequence] && !res[two_double_sequences]);
        // new_res[mixed_double_triplet] = new_res[mixed_double_triplet] && (!res[two_numbers] && !res[mixed_triple_triplet] && !res[two_mixed_double_triplets]);
        // new_res[two_mixed_double_triplets] = new_res[two_mixed_double_triplets] && !res[mixed_triple_triplet] && !res[two_numbers];
        // new_res[mixed_double_sequence] = new_res[mixed_double_sequence] && (!res[mixed_triple_sequence] && !res[two_mixed_double_sequences]);
        // new_res[two_mixed_double_sequences] = new_res[two_mixed_double_sequences] && !res[double_sequence];
        new_res[three_shifted_triplets] = new_res[three_shifted_triplets] && !res[four_shifted_triplets];
        new_res[three_shifted_sequences] = new_res[three_shifted_sequences] && !res[four_shifted_sequences];
        new_res[three_chained_sequences] = new_res[three_chained_sequences] && !res[four_chained_sequences];
        // new_res[short_straight] = new_res[short_straight] && (!res[two_short_straights] && !res[pure_straight] && !res[four_shifted_sequences]);
        // new_res[two_short_straights] = new_res[two_short_straights] && !res[pure_straight];
        // new_res[mixed_double_triplet] = 0;
        // new_res[mixed_double_sequence] = 0;
        // new_res[two_mixed_double_triplets] = 0;
        // new_res[two_mixed_double_sequences] = 0;
        new_res[fan_tile_1p] = new_res[fan_tile_1p] && !res[fan_tile_2p];
        new_res[fan_tile_2p] = new_res[fan_tile_2p] && !res[fan_tile_3p];
        new_res[fan_tile_3p] = new_res[fan_tile_3p] && !res[fan_tile_4p];
        new_res[fan_tile_4p] = new_res[fan_tile_4p] && !res[fan_tile_5p];
        new_res[fan_tile_5p] = new_res[fan_tile_5p] && !res[fan_tile_6p];
        new_res[fan_tile_6p] = new_res[fan_tile_6p] && !res[fan_tile_7p];
        new_res[fan_tile_1p] = new_res[fan_tile_1p] && !res[four_wind_pairs];
        new_res[fan_tile_3p] = new_res[fan_tile_3p] && !res[three_dragon_pairs];
        new_res[fan_tile_6p] = new_res[fan_tile_6p] && !res[six_dragon_pairs];
        new_res[eight_hog] = new_res[eight_hog] && !res[twelve_hog];
        new_res[twelve_hog] = new_res[twelve_hog] && !res[six_dragon_pairs];
        new_res[twelve_hog] = new_res[twelve_hog] && !res[fan_tile_7p];
        new_res[twelve_hog] = new_res[twelve_hog] && !res[quadruple_sequence];
        new_res[three_dragon_pairs] = new_res[three_dragon_pairs] && !res[six_dragon_pairs];
        return new_res;
    }

}

#endif