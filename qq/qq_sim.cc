//
// Created by CantelopePeel on 2020-02-11.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <bitset>
#include "types.h"
#include "methods.h"
#include "utils.h"


std::vector<std::string> split_string(const std::string& str, const char& delimiter) {
    std::vector<std::string> tokens;
    auto tok_start = str.begin();
    auto tok_end = str.begin();

    while (tok_start != str.end()) {
        tok_end = tok_start;
        tok_start = std::find(tok_start, str.end(), delimiter);

        if (tok_start != str.end()) {
            std::string token(tok_end, tok_start);
            if (token.length() > 0) {
                tokens.push_back(token);
            }
            tok_end = ++tok_start;
        }
    }

    std::string final_token(tok_end, tok_start);
    if (final_token.length() > 0) {
        tokens.push_back(final_token);
    }

    return tokens;
}

ClauseList read_dimacs(const std::string& filename) {
//    uint32_t num_clauses = 0, num_variables = 0;
//    uint32_t counted_clauses = 0;
//    bool parsed_program_line = false;

    std::ifstream file_stream;
    file_stream.open(filename);

    ClauseList clause_list;
    for (std::string line; std::getline(file_stream, line);) {
        auto tokens = split_string(line, ' ');
        if (tokens[0] == "p") {
            // parsed_program_line = true;
            uint64_t num_variables = std::stoul(tokens[2]);
            // num_clauses = std::stoul(tokens[3]);
            clause_list.set_num_system_variables(num_variables);
            for (int i = 1; i < (num_variables + 1); i++) {
                clause_list.add_variable(Variable(i));
            }
        } else if (tokens[0] == "c") {
            continue;
        } else { // This is a clause.
            Clause clause;
            for (const auto& token : tokens) {
                Literal lit = std::stoi(token);
                if (lit == 0) {
                    break;
                }
                clause.push_back(lit);
            }
            clause_list.add_clause(clause);
        }
    }

    return clause_list;
}

void usage() {
    std::cout << "qq_sim <cnf-dimacs-file> <sat-method> <oracle-method>" << std::endl;
}

Assignment state_to_assignment(int state, std::unordered_set<Variable> variables) {
    Assignment assignment;
    for (const Variable& var : variables) {
        int var_state_mask = 0x1 << (var - 1);
        if (var_state_mask & state) {
            assignment.insert(Literal(var));
        } else {
            assignment.insert(Literal(-var));
        }
    }
    return assignment;
}

int main (int narg, char *varg[]) {
    if (narg != 6) {
        usage();
        return 1;
    }

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.setf(std::ios::showpoint);
    std::cout.precision(20);

    std::string filename(varg[1]);
    long max_iterations = std::strtol(varg[2], nullptr, 10);
    // long samples = std::strtol(varg[3], nullptr, 10);

    std::string sat_method(varg[4]);
    std::string oracle_method(varg[5]);

    ClauseList clause_list = read_dimacs(filename);
    auto total_qubits_needed = static_cast<uint32_t>(clause_list.num_system_variables() + clause_list.num_clauses() + 1);
    std::cerr << "Num Clauses: " << clause_list.num_clauses() << std::endl;

    // Setup QuEST Environment
    QuESTEnv env = createQuESTEnv();
    Qureg qubits = createQureg(total_qubits_needed, env);

    std::vector<int> var_qubits;
    for (const Variable& var : clause_list.variables()) {
        var_qubits.push_back((var - 1));
    }

    grovers_algorithm_init(qubits, clause_list, &env);
    bool sat_assignment_found = false;
    for (int iterations = 0; iterations < max_iterations; iterations++) {
        std::vector<std::pair<qreal, int>> iter_prob_outcomes;

//        for (int i = 0; i < iterations; i++) {
            grovers_algorithm_iteration(qubits, clause_list, &env, 1);
            //qreal prob_outcome = measure_subset(qubits, var_qubits, total_qubits_needed);
            //iter_prob_outcomes.emplace_back(prob_outcome, iterations);
            int max_amp_state = find_max_amplitude_state(qubits, var_qubits, total_qubits_needed);
//            int long max_amp_state = sample_amplitude_state(qubits, var_qubits, total_qubits_needed);
            Assignment assignment = state_to_assignment(max_amp_state, clause_list.variables());

            std::string assignment_str;
            for (const Literal& lit : assignment) {
                assignment_str += std::to_string(lit) + " ";
            }
            std::cerr << "Iteration: " << iterations << " (" << assignment_str << ")" << std::endl;


        bool satisfied = clause_list.check_satisfied(assignment);
            if (satisfied) {
                std::cerr << "Solution:" << std::endl;
                print_assignment(assignment);
                std::cout << "Iterations: " << iterations + 1 << std::endl;
                sat_assignment_found = true;
                break;
            }
//        }

        //std::sort(iter_prob_outcomes.begin(), iter_prob_outcomes.end());

//        for (int i = 0; i < max_iterations; i++) {
//            std::cerr << iter_prob_outcomes[i].second << " " << iter_prob_outcomes[i].first << std::endl;
//        }
    }
//
//    for (int i = 0; i < total_qubits_needed; i++) {
//        std::cout << "Q:" << i << " " << calcProbOfOutcome(qubits, i, 0) << std::endl;
//    }

//    int num_amps = getNumAmps(qubits);
//    for (int i = 0; i < num_amps; i++) {
//        std::bitset<16> bin_num(i);
//        std::cout << bin_num << " " << getProbAmp(qubits, i) << std::endl;
//    }

    // measure_subset(qubits, var_qubits, total_qubits_needed);

    if (!sat_assignment_found) {
        std::cout << "none" << std::endl;
    }

    // Deallocate system.
    destroyQureg(qubits, env);

    // Deallocate environment.
    destroyQuESTEnv(env);
    return 0;
//    std::printf("-------------------------------------------------------\n");
//    std::printf("Running QuEST QQ Sim \n");
//    std::printf("-------------------------------------------------------\n");
//
//    // Prep system.
//    Qureg qubits = createQureg(3, env);
//    initZeroState(qubits);
//
//
//    // System and environment reporting.
//    std::printf("\nThis is our environment:\n");
//    reportQuregParams(qubits);
//    reportQuESTEnv(env);
//
//    // Apply circuit.
//    hadamard(qubits, 0);
//    controlledNot(qubits, 0, 1);
//    rotateY(qubits, 2, .1);
//
//    int targs[] = {0,1,2};
//    multiControlledPhaseFlip(qubits, targs, 3);
//
//    ComplexMatrix2 u = {
//            .real={{.5,.5},{.5,.5}},
//            .imag={{.5,-.5},{-.5,.5}}
//    };
//    unitary(qubits, 0, u);
//
//    Complex a, b;
//    a.real = .5; a.imag =  .5;
//    b.real = .5; b.imag = -.5;
//    compactUnitary(qubits, 1, a, b);
//
//    Vector v;
//    v.x = 1; v.y = 0; v.z = 0;
//    rotateAroundAxis(qubits, 2, 3.14/2, v);
//
//    controlledCompactUnitary(qubits, 0, 1, a, b);
//
//    int ctrls[] = {0,1};
//    multiControlledUnitary(qubits, ctrls, 2, 2, u);
//
//    ComplexMatrixN toff = createComplexMatrixN(3);
//    toff.real[6][7] = 1;
//    toff.real[7][6] = 1;
//    for (int i=0; i<6; i++)
//        toff.real[i][i] = 1;
//    multiQubitUnitary(qubits, targs, 3, toff);
//
//
//    // Record quantum state.
//    std::printf("\nCircuit output:\n");
//
//    qreal prob;
//    prob = getProbAmp(qubits, 7);
//    std::printf("Probability amplitude of |111>: %f\n", prob);
//
//    prob = calcProbOfOutcome(qubits, 2, 1);
//    std::printf("Probability of qubit 2 being in state 1: %f\n", prob);
//
//    int outcome = measure(qubits, 0);
//    std::printf("Qubit 0 was measured in state %d\n", outcome);
//
//    outcome = measureWithStats(qubits, 2, &prob);
//    std::printf("Qubit 2 collapsed to %d with probability %f\n", outcome, prob);
//
//    // Deallocate system.
//    destroyQureg(qubits, env);
//    destroyComplexMatrixN(toff);
//
//    // Deallocate environment.
//    destroyQuESTEnv(env);
//    return 0;
}
