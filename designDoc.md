开发 Parser 考虑建造语法树时，如果使用 std::string 作为节点标识符，效率太低。

之前的 Lexer 为了方便输出，直接将 LexType 定义为 std::string。

现将 LexType(添加 COMMENT 为注释，不区分行注释和块注释) 重构为单独枚举类。 SynType 也设计为枚举类。 

```c++
typedef std::string LexType;

enum class LexType {/*...*/}
```