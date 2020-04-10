//
// Created by Liam Heeger on 2020-02-14.
//

#ifndef QUEST_PROJECT_TYPES_H
#define QUEST_PROJECT_TYPES_H

#include <cstdint>
#include <vector>
#include <unordered_set>
#include <iostream>


using Variable = uint32_t;
using Literal = int32_t;
using Clause = std::vector<Literal>;
using Assignment = std::unordered_set<Literal>;

inline Variable lit_to_var(Literal lit) {
    return Variable(std::abs(lit));
}

class ClauseList {
    public:
        ClauseList();

        void add_clause(Clause clause);
        void add_variable(const Variable& variable);
        bool validate_clause_list();
        bool check_satisfied(const Assignment& assignment);

        const std::vector<Clause>& clauses() const { return clauses_; };
        const std::unordered_set<Variable>& variables() const { return variables_; };

        const uint64_t num_clauses() const { return clauses_.size(); };
        const uint64_t num_variables() const { return variables_.size(); };
        const uint64_t num_system_variables() const { return num_system_variables_; };

        void set_num_system_variables(uint64_t num_system_variables) { num_system_variables_ = num_system_variables; };
    protected:
        uint64_t num_system_variables_ = 0;
        std::vector<Clause> clauses_;
        std::unordered_set<Variable> variables_;

};


void print_assignment(const Assignment& assignment);

void print_clause(const Clause& clause);


#endif //QUEST_PROJECT_TYPES_H
