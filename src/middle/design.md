常量声明、变量声明、读语句、写语句、赋值语句，加减乘除模除等运算语句、函数定义及调用语句

输入 AST（或者四元式） ，输出一个 Module（便于继续生成到MIPS）
## 中间表示
数据结构：  
Module{Global}{Function}  
Function{BasicBlock}  
BasicBlock{Instruction}  

## Instruction
四元式，非 SSA

## 代码生成
生成指令用于维护 Activation Record(Stack Frame)

编译器内部存有AR(SF)，变量名->AR地址 
