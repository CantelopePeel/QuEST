//
// Created by Liam Heeger on 2020-02-14.
//

#ifndef QUEST_PROJECT_ORACLE_METHODS_H
#define QUEST_PROJECT_ORACLE_METHODS_H

#include "types.h"
#include "QuEST.h"

void basic_logical_oracle(const ClauseList& clause_list, QuESTEnv env);

void grovers_algorithm(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env, int iterations);
void grovers_algorithm_iteration(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env, int iterations = 1);
void grovers_algorithm_init(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env);


#endif //QUEST_PROJECT_ORACLE_METHODS_H
