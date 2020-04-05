//
// Created by Liam Heeger on 2020-03-03.
//

#include "utils.h"
#include "../QuEST/src/mt19937ar.h"

#include <iostream>
#include <unordered_map>
#include <map>

qreal measure_subset(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits) {
    int mask = 0x0;
    for (int qubit : qubit_subset) {
        mask |= 0x1 << (qubit);
    }

    std::unordered_map<int, qreal> amp_map;
    int num_amps = getNumAmps(qubits);
    for (int i = 0; i < num_amps; i++) {
        int masked_state = mask & i;
        auto amp_search = amp_map.find(masked_state);

        qreal amp = getProbAmp(qubits, i);
        if (amp_search != amp_map.end()) {
            amp_map[masked_state] += amp;
        } else {
            amp_map[masked_state] = amp;
        }

    }

   std::unordered_map<qreal, int> uniq_prob_counts;
   std::unordered_map<int, int> uniq_counts;
   std::map<int, int> exp_counts;
    // All pairs.
    int state_space_size = 0x1 << qubit_subset.size();
    for (const auto& entry : amp_map) {
        auto uniq_search = uniq_prob_counts.find(entry.second);
        if (uniq_search != uniq_prob_counts.end()) {
            uniq_prob_counts[entry.second] += 1;
        } else {
            uniq_prob_counts[entry.second] = 1;
        }
//        std::cout << "Q:" << std::bitset<32>(entry.first).to_string().substr(32 - total_num_qubits) << " "
//                  << entry.second << std::endl;
        int count = (int) ((qreal) state_space_size * entry.second);
        auto uniq_count_search = uniq_counts.find(count);
        if (uniq_count_search != uniq_counts.end()) {
            uniq_counts[count] += 1;
        } else {
            uniq_counts[count] = 1;
        }

        int exp = std::ilogb(entry.second);
        auto exp_search = exp_counts.find(exp);
        if (exp_search != exp_counts.end()) {
            exp_counts[exp] += 1;
        } else {
            exp_counts[exp] = 1;
        }
    }

    for (const auto& entry : exp_counts) {
        std::cerr << "E:" << entry.first << " " << entry.second << std::endl;
    }

    std::cerr << "PO: " << calcProbOfOutcome(qubits, total_num_qubits-1, 1) << std::endl;
    return calcProbOfOutcome(qubits, total_num_qubits-1, 1);
}

int find_max_amplitude_state(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits) {
    int mask = 0x0;
    for (int qubit : qubit_subset) {
        mask |= 0x1 << (qubit);
    }

    std::unordered_map<int, qreal> amp_map;
    int max_amp_state = 0;
    qreal max_amp_value = 0;
    int num_amps = getNumAmps(qubits);
    for (int i = 0; i < num_amps; i++) {
        int masked_state = mask & i;
        auto amp_search = amp_map.find(masked_state);

        qreal amp = getProbAmp(qubits, i);
        if (amp_search != amp_map.end()) {
            amp_map[masked_state] += amp;
        } else {
            amp_map[masked_state] = amp;
        }

        if (amp_map[masked_state] > max_amp_value) {
            max_amp_state = masked_state;
            max_amp_value = amp_map[masked_state];
        }
    }

    return max_amp_state;
}

long sample_amplitude_state(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits) {
    long mask = 0x0;
    for (int qubit : qubit_subset) {
        mask |= 0x1 << (qubit);
    }

    std::unordered_map<long, qreal> amp_map;
    long long int num_amps = getNumAmps(qubits);
    for (long i = 0; i < num_amps; i++) {
        long masked_state = mask & i;
        auto amp_search = amp_map.find(masked_state);

        qreal amp = getProbAmp(qubits, i);
        if (amp_search != amp_map.end()) {
            amp_map[masked_state] += amp;
        } else {
            amp_map[masked_state] = amp;
        }
    }

    qreal acc_val = 0.0;
    qreal rand_val = genrand_real1();
    for (const auto& amp_entry : amp_map) {
        acc_val += amp_entry.second;
        if (acc_val >= rand_val) {
            return amp_entry.first;
        }
    }
    return 0L;
}
