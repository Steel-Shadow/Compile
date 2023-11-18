常量声明、变量声明、读语句、写语句、赋值语句，加减乘除模除等运算语句、函数定义及调用语句

## todo:

检查所有 IR unique_ptr move

完善代码生成的变量类型系统?

局部变量地址：由目标代码生成完成，为每一个 Function 设置活动记录，
allocate 时记录变量相对于当前 activation record 的 offset。

遇到特殊 IR (InStack outStack) 时，将当前的 sp 当前偏移量 curOffset 入栈 stack<int> offsetStack，在结束时出栈恢复相应的
curOffset。

Cond 比较符号 ('<' | '>' | '<=' | '>=')

## 需要检查的内容

中间代码生成的符号表遍历 SymTab::iterIn

局部变量（包括数组）定义、初始化

## 中间表示

输入 AST（或者四元式） ，输出一个 Module（便于继续生成到MIPS）

数据结构：  
Module{Global}{Function}
Function{BasicBlock}  
BasicBlock{Instruction}

## 中间代码生成

如果单独建类 Visitor，在遍历语法树时，
为了区分不同的语法成分，需要多次使用 dynamic_cast(cpp)/instanceof(java) 判断多态性，这么做极其低效。

```java
SplStmt simple = stmt.getSimpleStmt();
if (simple instanceof AssignStmt) {
    analyseAssignStmt((AssignStmt) simple);
} else if (simple instanceof BreakStmt) {
    analyseBreakStmt((BreakStmt) simple);
} else if (simple instanceof ContinueStmt) {
    ...
}
```

下面修改设计：使用多态性，为基类提供纯虚函数，让派生类重载基类的中间代码生成方法。这样显著提高效率，更加优雅。
这么做，就需要将代码生成方法放入到 AST 节点类中，无需单独建立中间代码生成的类。

```c++

```

## 目标代码生成

生成指令用于维护 Activation Record(Stack Frame)

编译器内部存有AR(SF)，变量名->AR地址

## 优化

1. const 全局/局部变量可以消除，代码生成中变为常数，无需访问 $gp $fp
2. IR::Function 初始化时有一个空的 BasicBlock，后续可能无效。可以考虑删除空 BasicBlock
3. Exp 中间代码生成中，常数 Number 生成 IR::Temp，对应 Mips 的 li 指令。立即数装载可以直接在计算中完成，不一定需要分开一步。后端代码优化考虑。
4. BigForStmt 中间代码生成中，iter 结束后可直接判断 cond，减少运行的跳转数（但指令体积增大）
5. 函数传参，使用 register $a0-$a3
6. 函数如果没有调用其他函数，则 $ra 无需入栈