#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// 定义令牌类型的枚举
enum TokenType { KEYWORD, IDENTIFIER, CONSTANT, OPERATOR };

// 令牌结构
struct Token {
    enum TokenType type;
    char value[100];  // 假设标识符和常量的最大长度为100
};

// 函数原型
bool isKeyword(char* str);
bool isOperator(char c);
bool isDigit(char c);
bool isLetter(char c);
void tokenize(char* input);

int main() {
    char input[1000];  // 假设输入的源代码不超过1000个字符

    printf("请输入源代码：\n");
    fgets(input, sizeof(input), stdin);

    tokenize(input);

    return 0;
}

// 判断字符串是否为关键字
bool isKeyword(char* str) {
    char* keywords[] = {"int", "if", "else", "while", "for"};
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true;
        }
    }

    return false;
}

// 判断字符是否为运算符
bool isOperator(char c) {
    char operators[] = "+-*/=";
    int numOperators = strlen(operators);

    for (int i = 0; i < numOperators; i++) {
        if (c == operators[i]) {
            return true;
        }
    }

    return false;
}

// 判断字符是否为数字
bool isDigit(char c) {
    return isdigit(c);
}

// 判断字符是否为字母
bool isLetter(char c) {
    return isalpha(c);
}

// 将源代码分割成令牌
void tokenize(char* input) {
    char tokenValue[100];
    int tokenIndex = 0;
    int inputIndex = 0;

    while (input[inputIndex] != '\0') {
        char currentChar = input[inputIndex];

        // 忽略空白字符
        if (isspace(currentChar)) {
            inputIndex++;
            continue;
        }

        // 处理运算符
        if (isOperator(currentChar)) {
            tokenValue[tokenIndex++] = currentChar;
            tokenValue[tokenIndex] = '\0';
            printf("令牌：(OPERATOR, %s)\n", tokenValue);
            tokenIndex = 0;
            inputIndex++;
            continue;
        }

        // 处理数字
        if (isDigit(currentChar)) {
            while (isDigit(currentChar)) {
                tokenValue[tokenIndex++] = currentChar;
                inputIndex++;
                currentChar = input[inputIndex];
            }
            tokenValue[tokenIndex] = '\0';
            printf("令牌：(CONSTANT, %s)\n", tokenValue);
            tokenIndex = 0;
            continue;
        }

        // 处理字母（可能是关键字或标识符）
        if (isLetter(currentChar)) {
            while (isLetter(currentChar) || isDigit(currentChar)) {
                tokenValue[tokenIndex++] = currentChar;
                inputIndex++;
                currentChar = input[inputIndex];
            }
            tokenValue[tokenIndex] = '\0';

            if (isKeyword(tokenValue)) {
                printf("令牌：(KEYWORD, %s)\n", tokenValue);
            } else {
                printf("令牌：(IDENTIFIER, %s)\n", tokenValue);
            }

            tokenIndex = 0;
            continue;
        }

        // 其他字符
        printf("无法识别的字符：%c\n", currentChar);
        inputIndex++;
    }
}
