//
// Created by Liam Heeger on 2020-02-18.
//

#include "methods.h"

#include <iostream>

void basic_logical_oracle(const ClauseList& clause_list, const Qureg& ) {

}

void multi_control_toffoli(Qureg qubits, std::vector<int>& control_qubits,
        const std::vector<int>& anti_control_qubits, const int target_qubit, const ComplexMatrix2& target_matrix) {
    for (int anti_control : anti_control_qubits) {
        pauliX(qubits, anti_control);
    }

    multiControlledUnitary(qubits, control_qubits.data(), static_cast<const int>(control_qubits.size()), target_qubit,
            target_matrix);

    for (int anti_control : anti_control_qubits) {
        pauliX(qubits, anti_control);
    }
}

void compute_clause(Qureg qubits, const Clause& clause, const int target_qubit) {
    std::vector<int> control_qubits;
    std::vector<int> anti_control_qubits;

    for (const Literal& lit : clause) {
        Variable var = lit_to_var(lit);
        control_qubits.push_back(var - 1);
        if (lit < 0) {
            anti_control_qubits.push_back(var - 1);
        }
    }

    ComplexMatrix2 pauli_x = {
            .real={{0.,1.},{1.,0.}},
            .imag={{0.,0.},{0.,0.}}
    };

    multi_control_toffoli(qubits, control_qubits, anti_control_qubits, target_qubit, pauli_x);
}

void grovers_algorithm_init(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env) {
    auto total_qubits_needed = static_cast<uint32_t>(clause_list.num_system_variables() + clause_list.num_clauses() + 1);
    uint32_t sat_qubit = total_qubits_needed - 1;

    // Initialize qubit register.
    initZeroState(qubits);

    // Set up special initial state.
    //pauliX(qubits, sat_qubit);
    hadamard(qubits, sat_qubit);

    for (const Variable& var : clause_list.variables()) {
        hadamard(qubits, (var - 1));
    }
}

void grovers_algorithm_iteration(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env, int iterations) {
    auto total_qubits_needed = static_cast<uint32_t>(clause_list.num_system_variables() + clause_list.num_clauses() + 1);
    auto clause_qubits_offset = static_cast<uint32_t>(clause_list.num_system_variables());
    uint32_t sat_qubit = total_qubits_needed - 1;

    for (int grover_iteration = 0; grover_iteration < iterations; grover_iteration++) {
        hadamard(qubits, sat_qubit);

        // Compute clauses.
        std::vector<int> clause_tie_controls;
        for (uint64_t i = 0; i < clause_list.num_clauses(); i++) {
            //std::cout << "Compute clause:" << i << " " << clause_list.clauses()[i].size() << std::endl;
            int clause_qubit = static_cast<int>(clause_qubits_offset + i);
            compute_clause(qubits, clause_list.clauses()[i], clause_qubit);
            clause_tie_controls.push_back(clause_qubit);
        }

        ComplexMatrix2 pauli_x = {
                .real={{0.,1.},{1.,0.}},
                .imag={{0.,0.},{0.,0.}}
        };

        // Tie clauses.
        multi_control_toffoli(qubits, clause_tie_controls, clause_tie_controls, sat_qubit, pauli_x);

        // Un-compute clauses.
        for (int i = static_cast<int>(clause_list.num_clauses() - 1); i >= 0; i--) {
            //std::cout << "Un-compute clause:" << i << " " << clause_list.clauses()[i].size() << std::endl;

            int clause_qubit = static_cast<int>(clause_qubits_offset + i);
            compute_clause(qubits, clause_list.clauses()[i], clause_qubit);
        }

        hadamard(qubits, sat_qubit);

        // Diffusion operator.
        std::vector<int> diffusion_controls;
        for (const Variable& var : clause_list.variables()) {
            hadamard(qubits, (var - 1));
            pauliX(qubits, (var - 1));
            diffusion_controls.push_back((var - 1));

        }
        int diffusion_target = sat_qubit;

        hadamard(qubits, diffusion_target);
        pauliX(qubits, diffusion_target);
        hadamard(qubits, diffusion_target);

        multi_control_toffoli(qubits, diffusion_controls, {}, diffusion_target, pauli_x);

        hadamard(qubits, diffusion_target);
        pauliX(qubits, diffusion_target);
        hadamard(qubits, diffusion_target);

        for (const Variable& var : clause_list.variables()) {
            pauliX(qubits, (var - 1));
            hadamard(qubits, (var - 1));

        }
    }
}

//void grovers_algorithm(Qureg qubits, const ClauseList& clause_list, QuESTEnv* env, int iterations) {
//    auto total_qubits_needed = static_cast<uint32_t>(clause_list.num_system_variables() + clause_list.num_clauses() + 1);
//    auto clause_qubits_offset = static_cast<uint32_t>(clause_list.num_system_variables());
//    uint32_t sat_qubit = total_qubits_needed - 1;
//
//    // Initialize qubit register.
//    initZeroState(qubits);
//
//    // Set up special initial state.
//    pauliX(qubits, sat_qubit);
//    hadamard(qubits, sat_qubit);
//
//    for (const Variable& var : clause_list.variables()) {
//        hadamard(qubits, (var - 1));
//    }
//
//    for (int grover_iteration = 0; grover_iteration < iterations; grover_iteration++) {
//        // Compute clauses.
//        std::vector<int> clause_tie_controls;
//        for (uint64_t i = 0; i < clause_list.num_clauses(); i++) {
//            //std::cout << "Compute clause:" << i << " " << clause_list.clauses()[i].size() << std::endl;
//            int clause_qubit = static_cast<int>(clause_qubits_offset + i);
//            compute_clause(qubits, clause_list.clauses()[i], clause_qubit);
//            clause_tie_controls.push_back(clause_qubit);
//        }
//
//        ComplexMatrix2 pauli_x = {
//                .real={{0.,1.},{1.,0.}},
//                .imag={{0.,0.},{0.,0.}}
//        };
//
//        // Tie clauses.
//        multi_control_toffoli(qubits, clause_tie_controls, clause_tie_controls, sat_qubit, pauli_x);
//
//        // Un-compute clauses.
//        for (int i = static_cast<int>(clause_list.num_clauses() - 1); i >= 0; i--) {
//            //std::cout << "Un-compute clause:" << i << " " << clause_list.clauses()[i].size() << std::endl;
//
//            int clause_qubit = static_cast<int>(clause_qubits_offset + i);
//            compute_clause(qubits, clause_list.clauses()[i], clause_qubit);
//        }
//
//        // Diffusion operator.
//        std::vector<int> diffusion_controls;
//        for (const Variable& var : clause_list.variables()) {
//            pauliX(qubits, (var - 1));
//            hadamard(qubits, (var - 1));
//            diffusion_controls.push_back((var - 1));
//        }
//        int diffusion_target = diffusion_controls.back();
//        diffusion_controls.pop_back();
//
//        multi_control_toffoli(qubits, diffusion_controls, {} /* diffusion_controls */, diffusion_target, pauli_x);
//
//        for (const Variable& var : clause_list.variables()) {
//            hadamard(qubits, (var - 1));
//            pauliX(qubits, (var - 1));
//        }
//    }
//}
