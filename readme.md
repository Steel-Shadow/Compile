# Lexer

## 预读优化

实际处理中， Lexer 无需额外扫描。 Lexer 应当领先 Parser 若干个 token(具体领先数量依据设计而定，均存储于 Lexer 内部)。

Lexer 调用 next ，读取 token 替代最新预读词，其它存储的 token 依次更新。

由此优化， Lexer 仅会扫描全文一遍，预读不会影响性能。

我的设计

## 重构 LexType

之前的 Lexer 为了方便输出，直接将 LexType 定义为 std::string。

开发 Parser 考虑建造语法树时，如果使用 std::string 作为节点标识符，效率太低。

现将 LexType 和 SynType 重构为枚举类，两者都是 NodeType (语法树节点类型)。

```c++
// old
typedef std::string LexType;
// new
enum class NodeType {/*...*/}
```

# Parser

## 重构语法树 AST

原设计：AST 的节点均为一个类，该类成员属性有`本节点类型（枚举类，同时包含终结符词法类型和非终结符语法类型）`

应当为每个非终结符设置一个类，因为：不同非终结符的属性不同，不应当使用单个树节点类构建AST。

多个节点类使用继承降低复杂度、复用代码。

解析方法分散在各个节点类中，作为 static 方法。

# C++ 坑

## Singleton

Singleton 中，单例变量初始化要放在源文件(不可头文件)

[Union 中 gcc clang 编译器实现“错误”](https://stackoverflow.com/questions/70428563/unions-default-constructor-is-implicitly-deleted)
union 的 C++标准没说清楚，编译器厂商实现不一样！
c++ 17 应该使用 variant 替代 union。

## include 原则

头文件应当尽可能少地 include，以减少编译时间。

## Cmake

学习 Cmake 构建 cpp 工程。makefile 过于原始，不够现代。
