//
// Created by Liam Heeger on 2020-02-18.
//

#include "types.h"
#include <algorithm>

bool clause_lit_cmp(const Literal& lit0, const Literal& lit1) {
    Variable var0 = lit_to_var(lit0);
    Variable var1 = lit_to_var(lit1);

    return var0 < var1;
} 


ClauseList::ClauseList()
    : clauses_(), variables_() {}

void ClauseList::add_clause(Clause clause) {
    for (const Literal& lit : clause) {
        Variable var = lit_to_var(lit);
        variables_.insert(var);
    }
    
    std::sort(clause.begin(), clause.end(), clause_lit_cmp);
    clauses_.push_back(clause);
}

bool ClauseList::validate_clause_list() {
    return false;
}

void ClauseList::add_variable(const Variable& variable) {
    variables_.insert(variable);
}

// TODO: Improve performance.
bool ClauseList::check_satisfied(const Assignment& assignment) {
    bool clause_satisfied_flag = true;
    for (const Clause& clause : clauses_) {
        bool satisfied_flag = false;
        for (const Literal& literal : clause) {
            if (assignment.count(literal) == 1) {
                satisfied_flag = true;
                break;
            }
        }

        std::cerr << satisfied_flag << " | ";
        print_clause(clause);
        if (!satisfied_flag) {
            clause_satisfied_flag = false;
        }
    }
    return clause_satisfied_flag;
}

void print_assignment(const Assignment& assignment) {
    for (const Literal& literal : assignment) {
        std::cout << literal << " ";
    }
    std::cout << std::endl;
}

void print_clause(const Clause& clause) {
    for (const Literal& literal : clause) {
        std::cerr << literal << " ";
    }
    std::cerr << std::endl;
}
