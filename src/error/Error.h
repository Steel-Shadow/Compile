//
// Created by Steel_Shadow on 2023/9/23.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <exception>

class Error : public std::exception {
public:
    static void raise_error();
};


#endif //COMPILER_ERROR_H
