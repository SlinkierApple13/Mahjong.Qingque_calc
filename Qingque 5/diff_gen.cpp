#include <iostream>
#include <assert.h>
#include <array>
#include <queue>
#include <mutex>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <fstream>

#include "rules/qingque.hpp"

template<typename T>
class my_queue {
public:
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
    }
    void pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) return;
        m_queue.pop();
    }
    const T& front() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.front();
    }
    T poll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        T value = m_queue.front();
        m_queue.pop();
        return value;
    }
    bool try_poll(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty()) return false;
        value = m_queue.front();
        m_queue.pop();
        return true;
    }
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
};

using namespace mahjong;
using namespace mahjong::tile_literals;
using namespace mahjong::honours;

struct my_map {
    qingque::w_data m_data;
    mutable std::mutex m_mutex;

    void add_w(const std::bitset<qingque::code_size>& res, uint64_t weight) {
        std::lock_guard<std::mutex> lock(m_mutex);
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
        m_data.total_weight += weight;
        auto size = v.size();
        if (size == 1) add_w(v[0], weight);
        else for (unsigned i = 0; i < size; ++i) add_w_r(v, {i}, weight);
    }

};

struct meld_set : std::vector<meld> {
    meld_set(tile_t ti, meld_type ty) {
        push_back(meld(ti, ty, false, false));
        push_back(meld(ti, ty, false, true));
        if (ty == meld_type::triplet) {
            push_back(meld(ti, meld_type::kong, false, true));
            push_back(meld(ti, meld_type::kong, true, true));
        }
    }
};

const std::array<meld_set, 55> all_melds = {
    meld_set(E, meld_type::triplet),
    meld_set(S, meld_type::triplet),
    meld_set(W, meld_type::triplet),
    meld_set(N, meld_type::triplet),
    meld_set(C, meld_type::triplet),
    meld_set(F, meld_type::triplet),
    meld_set(P, meld_type::triplet),
    meld_set(1_m, meld_type::triplet),
    meld_set(2_m, meld_type::triplet),
    meld_set(3_m, meld_type::triplet),
    meld_set(4_m, meld_type::triplet),
    meld_set(5_m, meld_type::triplet),
    meld_set(6_m, meld_type::triplet),
    meld_set(7_m, meld_type::triplet),
    meld_set(8_m, meld_type::triplet),
    meld_set(9_m, meld_type::triplet),
    meld_set(1_p, meld_type::triplet),
    meld_set(2_p, meld_type::triplet),
    meld_set(3_p, meld_type::triplet),
    meld_set(4_p, meld_type::triplet),
    meld_set(5_p, meld_type::triplet),
    meld_set(6_p, meld_type::triplet),
    meld_set(7_p, meld_type::triplet),
    meld_set(8_p, meld_type::triplet),
    meld_set(9_p, meld_type::triplet),
    meld_set(1_s, meld_type::triplet),
    meld_set(2_s, meld_type::triplet),
    meld_set(3_s, meld_type::triplet),
    meld_set(4_s, meld_type::triplet),
    meld_set(5_s, meld_type::triplet),
    meld_set(6_s, meld_type::triplet),
    meld_set(7_s, meld_type::triplet),
    meld_set(8_s, meld_type::triplet),
    meld_set(9_s, meld_type::triplet),
    meld_set(2_m, meld_type::sequence),
    meld_set(3_m, meld_type::sequence),
    meld_set(4_m, meld_type::sequence),
    meld_set(5_m, meld_type::sequence),
    meld_set(6_m, meld_type::sequence),
    meld_set(7_m, meld_type::sequence),
    meld_set(8_m, meld_type::sequence),
    meld_set(2_p, meld_type::sequence),
    meld_set(3_p, meld_type::sequence),
    meld_set(4_p, meld_type::sequence),
    meld_set(5_p, meld_type::sequence),
    meld_set(6_p, meld_type::sequence),
    meld_set(7_p, meld_type::sequence),
    meld_set(8_p, meld_type::sequence),
    meld_set(2_s, meld_type::sequence),
    meld_set(3_s, meld_type::sequence),
    meld_set(4_s, meld_type::sequence),
    meld_set(5_s, meld_type::sequence),
    meld_set(6_s, meld_type::sequence),
    meld_set(7_s, meld_type::sequence),
    meld_set(8_s, meld_type::sequence)
};

struct my_task {
    hand h;
    uint64_t weight;
    my_task(const hand& h, uint64_t weight) : h(h), weight(weight) {}
    my_task(const my_task& t) : h(t.h), weight(t.weight) {}
    my_task& operator=(const my_task& t) {
        h = t.h;
        weight = t.weight;
        return *this;
    }
    my_task() : h({}, {}, 0u, 0u, true), weight(0) {}
};

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
    if (decompose_size > 1) weight /= decompose_size;
    for (auto ti : tile_set::all_tiles) {
        if (open_counter.count(ti) == 1) weight *= 4;
        if (open_counter.count(ti) == 2) weight *= 6;
        if (open_counter.count(ti) == 3) weight *= 4;
    }
    return weight;
}

void add_task(my_queue<my_task>& q, const hand& h, uint64_t wtile_size) {
    if (!h.is_valid()) return;
    q.push({h, hand_weight(h, wtile_size)});
}

void hand_gen(my_queue<std::array<uint32_t, 4>>& source, my_queue<my_task>& q, std::atomic_bool& stop_flag) {
    std::array<uint32_t, 4> i;
    while (!stop_flag) if (source.try_poll(i)) {
        std::vector<std::vector<meld>> meld_sets;
        for (uint32_t j : i) meld_sets.push_back(all_melds[j]);
        std::vector<tile_t> z_and_m_tiles = {E, S, W, N, C, F, P, 1_m, 2_m, 3_m, 4_m, 5_m, 6_m, 7_m, 8_m, 9_m};
        for (auto ti : z_and_m_tiles) {      
            auto h = hand({meld_sets[0][0], meld_sets[1][0], meld_sets[2][0], meld_sets[3][0]}, ti, ti);
            if (!h.is_valid()) continue;
            tile_counter all_counter({ti, ti});
            all_counter.add(meld_sets[0][0]);
            all_counter.add(meld_sets[1][0]);
            all_counter.add(meld_sets[2][0]);
            all_counter.add(meld_sets[3][0]);
            bool full_flush = (all_counter.count(tile_set::character_tiles) == 14) ||
                            (all_counter.count(tile_set::bamboo_tiles) == 14) ||
                            (all_counter.count(tile_set::dot_tiles) == 14);
            uint64_t weight_mult = 3;
            if (tile(ti).suit() == suit_type::z)
                weight_mult = 1;
            // bool reduced_sequences = false;
            // if (tile(ti).suit() == suit_type::z || i[0] < 7)
            //     reduced_sequences = true;
            // if (reduced_sequences) {
            //     for (int k = 1; k <= 3; ++k)
            //         if (meld_sets[k][0].type() == meld_type::sequence) {
            //             weight_mult *= 2.0; // 1 + melded_sequence_mult
            //             meld_sets[k] = {meld_sets[k][0]};
            //         }
            // }
            for (auto m0 : meld_sets[0])
            for (auto m1 : meld_sets[1])
            for (auto m2 : meld_sets[2])
            for (auto m3 : meld_sets[3]) {
                tile_counter counter({ti, ti});
                if (!m0.fixed()) counter.add(m0);
                if (!m1.fixed()) counter.add(m1);
                if (!m2.fixed()) counter.add(m2);
                if (!m3.fixed()) counter.add(m3);
                std::vector<tile_t> wts, wts3, wtso;
                for (auto wt : tile_set::all_tiles) {
                    if (counter.count(wt)) {
                        wts.push_back(wt);
                        if (counter.count(wt) == 3)
                            wts3.push_back(wt);
                        else wtso.push_back(wt);
                    }
                }
                if (!full_flush) {
                    if (!wtso.empty()) {
                        add_task(q, hand({m0, m1, m2, m3}, ti, wtso[0]), weight_mult * (counter.count() - 3 * wts3.size()));
                        add_task(q, hand({m0, m1, m2, m3}, ti, wtso[0], win_type::self_drawn), weight_mult * counter.count());
                    }
                    for (auto wt : wts3)
                        add_task(q, hand({m0, m1, m2, m3}, ti, wt), weight_mult * 3);
                } else for (auto wt : wts) {
                    add_task(q, hand({m0, m1, m2, m3}, ti, wt), weight_mult * counter.count(wt));
                    add_task(q, hand({m0, m1, m2, m3}, ti, wt, win_type::self_drawn), weight_mult * counter.count(wt));
                }
            }
        }
    }
}

void seven_pairs_gen(my_queue<std::array<uint32_t, 7>>& source, my_queue<my_task>& q, std::atomic_bool& stop_flag) {
    std::array<uint32_t, 7> j;
    while (!stop_flag) if (source.try_poll(j)) {
        std::vector<tile_t> tile_list;
        for (uint32_t k : j) {
            tile_list.push_back(tile_set::all_tiles[k]);
            tile_list.push_back(tile_set::all_tiles[k]);
        }
        add_task(q, hand(tile_list, {}, tile_list[0], 0, true), 14);
        add_task(q, hand(tile_list, {}, tile_list[0], win_type::self_drawn, true), 14);
    }
}

void other_gen(my_queue<my_task>& q) {
    std::vector<tile_t> list_131 = {1_m, 9_m, 1_p, 9_p, 1_s, 9_s, E, S, W, N, C, F, P};
    for (auto ti : list_131) {
        add_task(q, hand(list_131, {}, ti, 0, false), 14);
        add_task(q, hand(list_131, {}, ti, win_type::self_drawn, false), 14);
    }
    for (int i = 0; i < 720; ++i) {
        add_task(q, hand(std::vector<tile_t>{1_m, 4_m, 7_m, 2_p, 5_p, 8_p, 3_s, 6_s, 9_s, E, S, W, N}, {}, C, 0, false), 14);
        add_task(q, hand(std::vector<tile_t>{1_m, 4_m, 7_m, 2_p, 5_p, 8_p, 3_s, 6_s, 9_s, E, S, W, N}, {}, C, win_type::self_drawn, false), 14);
    }
}

my_queue<my_task> task_queue;
my_queue<std::array<uint32_t, 4>> source_queue;
my_queue<std::array<uint32_t, 7>> seven_pairs_source_queue;
my_map result_map;
std::atomic_bool stop_flag = false;

void src_gen(my_queue<std::array<uint32_t, 4>>& q, my_queue<std::array<uint32_t, 7>>& spq, my_queue<my_task>& tq) {
    std::array<uint32_t, 4> i;
    for (i[0] = 0; i[0] < 55; ++i[0])
        for (i[1] = i[0] + (i[0] < 34); i[1] < 55; ++i[1]) 
            for (i[2] = i[1] + (i[1] < 34); i[2] < 55; ++i[2])
                for (i[3] = i[2] + (i[2] < 34); i[3] < 55; ++i[3])
                    q.push(i);
    std::array<uint32_t, 7> j;
    for (j[0] = 0; j[0] < 34; ++j[0])
    for (j[1] = j[0]; j[1] < 34; ++j[1])
    for (j[2] = j[1] + (j[1] == j[0]); j[2] < 34; ++j[2])
    for (j[3] = j[2] + (j[2] == j[1]); j[3] < 34; ++j[3])
    for (j[4] = j[3] + (j[3] == j[2]); j[4] < 34; ++j[4])
    for (j[5] = j[4] + (j[4] == j[3]); j[5] < 34; ++j[5])
    for (j[6] = j[5] + (j[5] == j[4]); j[6] < 34; ++j[6])
        spq.push(j);
    // other_gen(tq);
}

void process_task(const my_task& t, my_map& m) {
    uint64_t weight = t.weight;
    m.add_data(t.h, weight);
}

void process_tasks(my_queue<my_task>& q, my_map& m, std::atomic_bool& stop_flag) {
    my_task t;
    while (!stop_flag) if (q.try_poll(t))
        process_task(t, m);
}

void save_result(const my_map& m, const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(&m.m_data.total_weight), sizeof m.m_data.total_weight);
    auto res = m.m_data.weights;
    for (auto& e : res) {
        auto code = e.first;
        uint64_t weight = e.second;
        file.write(reinterpret_cast<char*>(&code), sizeof code);
        file.write(reinterpret_cast<char*>(&weight), sizeof weight);
    }
}

void monitor(my_queue<std::array<uint32_t, 4>>& sq, my_queue<std::array<uint32_t, 7>>& spq, my_queue<my_task>& tq, std::atomic_bool& stop_flag) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << sq.size() << " " << spq.size() << " " << tq.size() << std::endl;
        try { save_result(result_map, "result_map_temp.dat"); } catch (...) {}
        if (sq.empty() && tq.empty() && spq.empty()) {
            std::cout << "Calculation complete" << std::endl;
            stop_flag = true;
            return;
        }
    }
}

int main() {
    int a, b;
    std::cin >> a >> b;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::thread src_thread(src_gen, std::ref(source_queue), std::ref(seven_pairs_source_queue), std::ref(task_queue));
    std::vector<std::thread> hand_threads(a);
    for (auto& t : hand_threads) t = std::thread(hand_gen, std::ref(source_queue), std::ref(task_queue), std::ref(stop_flag));
    std::thread seven_pairs_thread(seven_pairs_gen, std::ref(seven_pairs_source_queue), std::ref(task_queue), std::ref(stop_flag));
    std::vector<std::thread> proc_threads(b);
    for (auto& t : proc_threads) t = std::thread(process_tasks, std::ref(task_queue), std::ref(result_map), std::ref(stop_flag));
    std::thread monitor_thread(monitor, std::ref(source_queue), std::ref(seven_pairs_source_queue), std::ref(task_queue), std::ref(stop_flag));
    src_thread.join();
    seven_pairs_thread.join();
    for (auto& t : hand_threads) t.join();
    for (auto& t : proc_threads) t.join();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    save_result(result_map, "result_map.dat");
    while (getchar()); // Keep the console window open
    return 0;
}