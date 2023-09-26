编译单元CompUnit→{Decl}{FuncDef}MainFuncDef//1.是否存在Decl2.是否存在FuncDef 

声明Decl→ConstDecl|VarDecl//覆盖两种声明

常量声明ConstDecl→'const'BTypeConstDef{','ConstDef}';'//1.花括号内重复0 次2.花括号内重复多次

基本类型BType→'int'//存在即可

常数定义ConstDef→Ident{'['ConstExp']'}'='ConstInitVal//包含普通变量、一维数组、二维数组共三种情况

常量初值ConstInitVal→ConstExp |'{'[ConstInitVal{','ConstInitVal}]'}'//1.常表达式初值2.一维数组初值3.二维数组初值

变量声明VarDecl→BTypeVarDef{','VarDef}';'//1.花括号内重复0次2.花括号内重复多次

变量定义VarDef→Ident{'['ConstExp']'}//包含普通变量、一维数组、二维数组定义
|Ident{'['ConstExp']'}'='InitVal

变量初值InitVal→Exp|'{'[InitVal{','InitVal}]'}'//1.表达式初值2.一维数组初值3.二维数组初值

函数定义FuncDef→FuncTypeIdent'('[FuncFParams]')'Block//1.无形参2.有形参

主函数定义MainFuncDef→'int''main''('')'Block//存在main函数

函数类型FuncType→'void'|'int'//覆盖两种类型的函数

函数形参表FuncFParams→FuncFParam{','FuncFParam}//1.花括号内重复0次2.花括号内重复多次

函数形参FuncFParam→BTypeIdent['['']'{'['ConstExp']'}]//1.普通变量2.一维数组变量3.二维数组变量

语句块Block→'{'{BlockItem}'}'//1.花括号内重复0次2.花括号内重复多次

语句块项BlockItem→Decl|Stmt//覆盖两种语句块项

语句Stmt→LVal'='Exp';'//每种类型的语句都要覆盖

|[Exp]';'//有无Exp两种情况
|Block 
|'if''('Cond')'Stmt['else'Stmt]//1.有else2.无else 
|'for''('[ForStmt]';'[Cond]';'[ForStmt]')'Stmt//1.无缺省2.缺省第一个ForStmt3.缺省Cond4.缺省第二个ForStmt

|'break'';'|'continue'';'
|'return'[Exp]';'//1.有Exp2.无Exp 
|LVal'=''getint''('')'';'
|'printf''('FormatString{','Exp}')'';'//1.有Exp2.无Exp

语句ForStmt→LVal'='Exp//存在即可

表达式Exp→AddExp注：SysY表达式是int型表达式//存在即可

条件表达式Cond→LOrExp//存在即可

左值表达式LVal→Ident{'['Exp']'}//1.普通变量2.一维数组3.二维数组
基本表达式PrimaryExp→'('Exp')'|LVal|Number//三种情况均需覆盖

数值Number→IntConst//存在即可

一元表达式UnaryExp→PrimaryExp|Ident'('[FuncRParams]')'//3种情况均需覆盖,函数调用也需要覆盖FuncRParams的不同情况

|UnaryOpUnaryExp//存在即可

单目运算符UnaryOp→'+'|'−'|'!'注：'!'仅出现在条件表达式中//三种均需覆盖

函数实参表FuncRParams→Exp{','Exp}//1.花括号内重复0次2.花括号内重复多次3.Exp需要覆盖数组传参和部分数组传参

乘除模表达式MulExp→UnaryExp|MulExp('*'|'/'|'%')UnaryExp//1.UnaryExp
2.*3./4.%均需覆盖

加减表达式AddExp→MulExp|AddExp('+'|'−')MulExp//1.MulExp2.+需覆盖3.-需覆盖

关系表达式RelExp→AddExp|RelExp('<'|'>'|'<='|'>=')AddExp//1.AddExp
2.<3.>4.<=5.>=均需覆盖

相等性表达式EqExp→RelExp|EqExp('=='|'!=')RelExp//1.RelExp2.==3.!=均需覆盖

逻辑与表达式LAndExp→EqExp|LAndExp'&&'EqExp//1.EqExp2.&&均需覆盖

逻辑或表达式LOrExp→LAndExp|LOrExp'||'LAndExp//1.LAndExp2.||均需覆盖

常量表达式ConstExp→AddExp注：使用的Ident必须是常量//存在即可
