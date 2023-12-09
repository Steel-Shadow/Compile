## 数组开发进度

数组检查维度没有开启错误处理？
中间代码生成 PushParam 没有使用 a0-a3

## 数据类型系统

完善代码生成的变量类型系统：
前端Parser需要将类型录入 SymTab，还需要添加函数参数和函数返回值(变量已经实现)，再将类型录入IR::Module

## 中间表示

输入 AST（或者四元式） ，输出一个 Module（便于继续生成到MIPS）

数据结构：  
Module{Global}{Function}
Function{BasicBlock}  
BasicBlock{Instruction}

将 glob 变量和 const 变量初值存储在符号表中， evaluate 访问符号表获取 const var。
CompUnit::genIR()中，使用符号表的常量信息优化。

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

## 栈内存分配

遇到特殊 IR (InStack outStack) 时，将当前的 sp 当前偏移量 curOffset 入栈 stack<int> offsetStack，在结束时出栈恢复相应的curOffset。

## 临时寄存器分配策略

IR::Temp -> real Register
t0 开始设置临时寄存器，只需修改 MAX_TEMP_REGS 即可调整寄存器数量。
若没有可用临时寄存器，则将临时寄存器存储为局部变量。
所有临时寄存器只生成一次，只使用一次，getReg()使用后马上释放实际寄存器。

## 函数调用

函数调用保存现场时，保存使用到了的临时寄存器，但分配 MAX_TEMP_REGS 的栈内存，这样被调用的子函数在定位栈内存时就无需考虑父函数使用了多少临时寄存器了。

另一种方法是，在 jal 之前将已使用的 tempRegs 数量保存到 realReg $?，
使用 realReg $? 代替 $sp 来定位内存（速度较慢，但栈内存占用较少）。

竞速仅考虑速度，这里不考虑爆栈。

## 优化

1. const 全局/局部变量可以消除，代码生成中变为常数，无需访问 $gp $fp
2. IR::Function 初始化时有一个空的 BasicBlock，后续可能无效。可以考虑删除空 BasicBlock
3. Exp 中间代码生成中，常数 Number 生成 IR::Temp，对应 Mips 的 li 指令。立即数装载可以直接在计算中完成，不一定需要分开一步。后端代码优化考虑。
4. BigForStmt 中间代码生成中，iter 结束后可直接判断 cond，减少运行的跳转数（但指令体积增大）
5. 函数如果没有调用其他函数，则 $ra 无需入栈
6. 地址装载 lw label，我可以自行计算全局数据段地址，可以避免伪指令
7. div 指令选择，伪指令判断除以0了
8. `<= >= !` 中使用了常数1，考虑将1放置在某个固定寄存器(类似$zero)，可以是$fp
9. bif0 bif1 可以与最后一次 Cond 计算结果合并