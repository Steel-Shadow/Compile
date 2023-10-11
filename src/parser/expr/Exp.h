//
// Created by Steel_Shadow on 2023/10/11.
//

#ifndef COMPILER_EXP_H
#define COMPILER_EXP_H

class Exp {
    bool isConst;
public:
    explicit Exp(bool isConst);

public:
    ~Exp() = default;
};

#endif //COMPILER_EXP_H
