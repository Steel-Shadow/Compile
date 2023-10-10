//
// Created by Steel_Shadow on 2023/9/25.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "Node.h"
#include "lexer/Lexer.h"

class Parser {
private:
    static Parser *instance;

    explicit Parser(Lexer &lexer);

    Lexer &lexer;
    NodeType &curLexType;

public:
    // Singleton
    static Parser *getInstance(Lexer &lexer);

    Parser(Parser const &) = delete;

    void operator=(Parser const &) = delete;
    // Singleton

    // don't use/print in <BlockItem>, <Decl>, <BType>
    void output(NodeType type);

    // CompUnit→{Decl}{FuncDef}MainFuncDef
    Node *CompUnit();

    // Decl→ConstDecl|VarDecl
    Node *Decl();

    // ConstDecl→'const' BType ConstDef {','ConstDef}';'
    Node *ConstDecl();

    // ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
    Node *ConstDef();

    // ConstInitVal → ConstExp
    //| '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    Node *ConstInitVal();

    // VarDecl → BType VarDef { ',' VarDef } ';'
    Node *VarDecl();

    // VarDef → Ident { '[' ConstExp ']' }
    // | Ident { '[' ConstExp ']' } '=' InitVal
    Node *VarDef();

    // InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
    Node *InitVal();

    //FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
    Node *FuncDef();

    // MainFuncDef→'int''main''('')'Block
    Node *MainFuncDef();

    // FuncType → 'void' | 'int'
    Node *FuncType();

    // FuncFParams → FuncFParam { ',' FuncFParam }
    Node *FuncFParams();

    // FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
    Node *FuncFParam();

    // Block → '{' { BlockItem } '}'
    Node *Block();

    // BlockItem → Decl | Stmt
    Node *BlockItem();

    // Stmt → LVal '=' Exp ';'
    //| [Exp] ';'
    //| Block
    //| 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
    //| 'break' ';' | 'continue' ';'
    //| 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
    //| 'return' [Exp] ';'
    //| LVal '=' 'getint''('')'';'
    //| 'printf''('FormatString{','Exp}')'';'
    Node *Stmt();

    // ForStmt → LVal '=' Exp
    Node *ForStmt();

    // Exp → AddExp
    Node *Exp();

    // Cond → LOrExp
    Node *Cond();

    // LVal → Ident {'[' Exp ']'}
    Node *LVal();

    // PrimaryExp → '(' Exp ')' | LVal | Number
    Node *PrimaryExp();

    // Number → IntConst
    Node *Number();

    // UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
    Node *UnaryExp();

    // UnaryOp → '+' | '−' | '!'
    Node *UnaryOp();

    // FuncRParams → Exp { ',' Exp }
    Node *FuncRParams();

    // MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
    Node *MulExp();

    // AddExp → MulExp | AddExp ('+' | '−') MulExp
    Node *AddExp();

    // RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
    Node *RelExp();

    // EqExp → RelExp | EqExp ('==' | '!=') RelExp
    Node *EqExp();

    // LAndExp → EqExp | LAndExp '&&' EqExp
    Node *LAndExp();

    // LOrExp → LAndExp | LOrExp '||' LAndExp
    Node *LOrExp();

    // ConstExp → AddExp
    Node *ConstExp();

    // invoke lexer
    Node *Ident();

    // BType → 'int'
    Node *BType();

    void IfStmt(Node *n);

    void ReturnStmt(Node *n);

    void PrintStmt(Node *n);

};


#endif //COMPILER_PARSER_H
