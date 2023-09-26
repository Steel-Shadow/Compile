# 重构 LexType
开发 Parser 考虑建造语法树时，如果使用 std::string 作为节点标识符，效率太低。

之前的 Lexer 为了方便输出，直接将 LexType 定义为 std::string。

现将 LexType(添加 COMMENT 为注释，不区分行注释和块注释) 重构为单独枚举类。 SynType 也设计为枚举类。 

```c++
typedef std::string LexType;

enum class LexType {/*...*/}
```

# Lexer 预读优化

实际处理中， Lexer 无需额外扫描。 Lexer 应当领先 Parser 若干个 token(具体领先数量依据设计而定，均存储于 Lexer 内部)。

Lexer 调用 next ，读取 token 替代最新预读词，其它存储的 token 依次更新。

由此优化， Lexer 仅会扫描全文一遍，预读不会影响性能。

# C++ 坑

Singleton 中，单例变量初始化要放在源文件(不可头文件)

[Union 中 gcc clang 编译器实现“错误”](https://stackoverflow.com/questions/70428563/unions-default-constructor-is-implicitly-deleted)
C++标准没说清楚，编译器厂商实现不一样！
