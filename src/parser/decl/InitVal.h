//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_INITVAL_H
#define COMPILER_INITVAL_H

#include <memory>
#include <vector>

class Exp;

// ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
// InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
// Both InitVal & ConstInitVal. Distinguish with `bool cons`
class InitVal {
protected:
    bool cons;

public:
    virtual ~InitVal() = default;

    static std::unique_ptr<InitVal> parse(bool cons);

    virtual std::vector<int> evaluate() = 0;
};

class ExpInitVal : public InitVal {
    std::unique_ptr<Exp> exp;

public:
    static std::unique_ptr<ExpInitVal> parse(bool cons);

    std::vector<int> evaluate() override;
};

class ArrayInitVal : public InitVal {
    std::vector<std::unique_ptr<InitVal>> array;

public:
    static std::unique_ptr<ArrayInitVal> parse(bool cons);

    std::vector<int> evaluate() override;
};

#endif //COMPILER_INITVAL_H
