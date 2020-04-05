//
// Created by Liam Heeger on 2020-03-03.
//

#ifndef QUEST_PROJECT_UTILS_H
#define QUEST_PROJECT_UTILS_H

#include <vector>
#include "QuEST.h"

qreal measure_subset(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits);

int find_max_amplitude_state(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits);

long sample_amplitude_state(Qureg qubits, const std::vector<int>& qubit_subset, int total_num_qubits);

#endif //QUEST_PROJECT_UTILS_H
