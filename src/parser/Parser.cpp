#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by Steel_Shadow on 2023/9/25.
//

#include "Parser.h"

Parser *Parser::instance = nullptr;

Parser::Parser(Lexer &lexer) :
        lexer(lexer),
        curLexType(*lexer.getLexTypePtr()) {}

Parser *Parser::getInstance(Lexer &lexer) {
    if (instance == nullptr) {
        instance = new Parser(lexer);
    }

    return instance;
}

void Parser::output(NodeType type) {
#ifdef MY_DEBUG
    std::cout << "<" << typeToStr(type) << ">" << std::endl;
#endif
    lexer.getOutFileStream() << "<" << typeToStr(type) << ">" << std::endl;
}

Node *Parser::CompUnit() {
    auto n = new Node(NodeType::CompUnit);

    while (curLexType == NodeType::CONSTTK || curLexType == NodeType::INTTK) {
        if (lexer.peek(1).first == NodeType::IDENFR && lexer.peek(2).first == NodeType::LPARENT
            || curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->addChild(Decl());
    }

    while (curLexType == NodeType::VOIDTK || curLexType == NodeType::INTTK) {
        if (curLexType == NodeType::INTTK && lexer.peek(1).first == NodeType::MAINTK) {
            break;
        }
        n->addChild(FuncDef());
    }

    if (curLexType == NodeType::INTTK) {
        n->addChild(MainFuncDef());
    } else { Error::raise_error(); }

    output(NodeType::CompUnit);
    return n;
}

Node *Parser::Decl() {
    auto n = new Node(NodeType::Decl);

    if (curLexType == NodeType::CONSTTK) {
        n->addChild(ConstDecl());
    } else if (curLexType == NodeType::INTTK) {
        n->addChild(VarDecl());
    } else { Error::raise_error(); }

//    output(NodeType::Decl);
    return n;
}

Node *Parser::ConstDecl() {
    auto n = new Node(NodeType::ConstDecl);

    n->singleLex(NodeType::CONSTTK);

    n->addChild(BType());
    n->addChild(ConstDef());

    while (curLexType == NodeType::COMMA) {
        n->addChild(new Node(NodeType::COMMA));
        lexer.next();
        n->addChild(ConstDef());
    }

    n->singleLex(NodeType::SEMICN);
    output(NodeType::ConstDecl);
    return n;
}

Node *Parser::ConstDef() {
    auto n = new Node(NodeType::ConstDef);

    n->addChild(Ident());

    while (curLexType == NodeType::LBRACK) {
        n->addChild(new Node(NodeType::LBRACK));
        lexer.next();

        n->addChild(ConstExp());

        n->singleLex(NodeType::RBRACK);
    }

    n->singleLex(NodeType::ASSIGN);

    n->addChild(ConstInitVal());

    output(NodeType::ConstDef);
    return n;
}

Node *Parser::ConstInitVal() {
    auto n = new Node(NodeType::ConstExp);

    if (curLexType == NodeType::LBRACE) {
        n->addChild(new Node(NodeType::LBRACE));
        lexer.next();

        if (curLexType != NodeType::RBRACE) {
            n->addChild(ConstInitVal());

            while (curLexType == NodeType::COMMA) {
                n->addChild(new Node(NodeType::COMMA));
                lexer.next();

                n->addChild(ConstInitVal());
            }
        }

        n->singleLex(NodeType::RBRACE);
    } else {
        n->addChild(ConstExp());
    }

    output(NodeType::ConstInitVal);
    return n;
}

Node *Parser::VarDecl() {
    auto n = new Node(NodeType::VarDecl);

    n->addChild(BType());

    n->addChild(VarDef());

    while (curLexType == NodeType::COMMA) {
        lexer.next();
        n->addChild(VarDef());
    }

    n->singleLex(NodeType::SEMICN);

    output(NodeType::VarDecl);
    return n;
}

Node *Parser::VarDef() {
    auto n = new Node(NodeType::VarDef);

    n->addChild(Ident());

    while (curLexType == NodeType::LBRACK) {
        n->addChild(new Node(NodeType::LBRACK));
        lexer.next();

        n->addChild(ConstExp());

        n->singleLex(NodeType::RBRACK);
    }

    if (curLexType == NodeType::ASSIGN) {
        n->singleLex(NodeType::ASSIGN);
        n->addChild(InitVal());
    }

    output(NodeType::VarDef);
    return n;
}

Node *Parser::InitVal() {
    auto n = new Node(NodeType::InitVal);

    if (curLexType == NodeType::LBRACE) {
        n->addChild(new Node(NodeType::LBRACE));
        lexer.next();

        if (curLexType != NodeType::RBRACE) {
            n->addChild(InitVal());

            while (curLexType == NodeType::COMMA) {
                n->addChild(new Node(NodeType::COMMA));
                lexer.next();
                n->addChild(InitVal());
            }
        }

        n->singleLex(NodeType::RBRACE);

    } else {
        n->addChild(Exp());
    }

    output(NodeType::InitVal);
    return n;
}

Node *Parser::FuncDef() {
    auto n = new Node(NodeType::FuncDef);

    n->addChild(FuncType());
    n->addChild(Ident());

    n->singleLex(NodeType::LPARENT);
    if (curLexType != NodeType::RPARENT) {
        n->addChild(FuncFParams());
    }
    n->singleLex(NodeType::RPARENT);

    n->addChild(Block());

    output(NodeType::FuncDef);
    return n;
}

Node *Parser::MainFuncDef() {
    auto n = new Node(NodeType::MainFuncDef);

    n->singleLex(NodeType::INTTK);
    n->singleLex(NodeType::MAINTK);
    n->singleLex(NodeType::LPARENT);
    n->singleLex(NodeType::RPARENT);

    n->addChild(Block());

    output(NodeType::MainFuncDef);
    return n;
}


Node *Parser::FuncType() {
    auto n = new Node(NodeType::FuncType);

    if (curLexType == NodeType::VOIDTK) {
        n->addChild(new Node(NodeType::VOIDTK));
        lexer.next();
    } else if (curLexType == NodeType::INTTK) {
        n->addChild(new Node(NodeType::INTTK));
        lexer.next();
    } else { Error::raise_error(); }

    output(NodeType::FuncType);
    return n;
}

Node *Parser::FuncFParams() {
    auto n = new Node(NodeType::FuncFParams);

    n->addChild(FuncFParam());

    while (curLexType == NodeType::COMMA) {
        n->addChild(new Node(NodeType::COMMA));
        lexer.next();
        n->addChild(FuncFParam());
    }

    output(NodeType::FuncFParams);
    return n;
}

Node *Parser::FuncFParam() {
    auto n = new Node(NodeType::FuncFParam);

    n->addChild(BType());
    n->addChild(Ident());
//     ['[' ']' { '[' ConstExp ']' }]
    if (curLexType == NodeType::LBRACK) {
        n->addChild(new Node(NodeType::LBRACK));
        lexer.next();

        n->singleLex(NodeType::RBRACK);

        while (curLexType == NodeType::LBRACK) {
            n->addChild(new Node(NodeType::LBRACK));
            lexer.next();

            n->addChild(ConstExp());
            n->singleLex(NodeType::RBRACK);
        }
    }

    output(NodeType::FuncFParam);
    return n;
}

Node *Parser::Block() {
    auto n = new Node(NodeType::Block);

    n->singleLex(NodeType::LBRACE);

    while (curLexType != NodeType::RBRACE) {
        n->addChild(BlockItem());
    }

    n->addChild(new Node(NodeType::RBRACE));
    lexer.next();

    output(NodeType::Block);
    return n;
}

Node *Parser::BlockItem() {
    auto n = new Node(NodeType::BlockItem);

    // Maybe error when neither Decl nor Stmt. But it's too complicated.
    if (curLexType == NodeType::CONSTTK || curLexType == NodeType::INTTK) {
        n->addChild(Decl());
    } else {
        n->addChild(Stmt());
    }

    // output(NodeType::BlockItem);
    return n;
}

Node *Parser::Stmt() {
    auto n = new Node(NodeType::Stmt);

    switch (curLexType) {
        case NodeType::LBRACE:
            n->addChild(Block());
            break;
        case NodeType::IFTK:
            IfStmt(n);
            break;
        case NodeType::BREAKTK:
            n->addChild(new Node(NodeType::BREAKTK));
            lexer.next();
            n->singleLex(NodeType::SEMICN);
            break;
        case NodeType::CONTINUETK:
            n->addChild(new Node(NodeType::CONTINUETK));
            lexer.next();
            n->singleLex(NodeType::SEMICN);
            break;
        case NodeType::FORTK:
            n->addChild(new Node(NodeType::FORTK));
            lexer.next();

            n->singleLex(NodeType::LPARENT);

            if (curLexType != NodeType::SEMICN) {
                n->addChild(ForStmt());
            }
            n->singleLex(NodeType::SEMICN);

            if (curLexType != NodeType::SEMICN) {
                n->addChild(Cond());
            }
            n->singleLex(NodeType::SEMICN);

            if (curLexType != NodeType::RPARENT) {
                n->addChild(ForStmt());
            }

            n->singleLex(NodeType::RPARENT);

            n->addChild(Stmt());
            break;
        case NodeType::RETURNTK:
            ReturnStmt(n);
            break;
        case NodeType::PRINTFTK:
            PrintStmt(n);
            break;
        case NodeType::SEMICN:
            n->addChild(new Node(NodeType::SEMICN));
            lexer.next();
            break;
        case NodeType::IDENFR:
            //find '=' to distinguish LVal and Exp
            if (lexer.findAssignBeforeSemicolon()) {
                n->addChild(LVal());
                n->singleLex(NodeType::ASSIGN);
                if (curLexType == NodeType::GETINTTK) {
                    n->addChild(new Node(NodeType::GETINTTK));
                    lexer.next();

                    n->singleLex(NodeType::LPARENT);
                    n->singleLex(NodeType::RPARENT);
                    n->singleLex(NodeType::SEMICN);
                } else {
                    n->addChild(Exp());
                    n->singleLex(NodeType::SEMICN);
                }
            } else {
                n->addChild(Exp());
                n->singleLex(NodeType::SEMICN);
            }
            break;
        default:
            n->addChild(Exp());
            n->singleLex(NodeType::SEMICN);
    }

    output(NodeType::Stmt);
    return n;
}

void Parser::PrintStmt(Node *n) {
    n->addChild(new Node(NodeType::PRINTFTK));
    lexer.next();

    n->singleLex(NodeType::LPARENT);

    if (lexer.peek().first == NodeType::STRCON) {
        auto str = new Node(NodeType::STRCON);
        str->setValue(lexer.peek().second);
        n->addChild(str);
        lexer.next();
    } else { Error::raise_error(); }

    while (curLexType == NodeType::COMMA) {
        n->addChild(new Node(NodeType::COMMA));
        lexer.next();

        n->addChild(Exp());
    }

    n->singleLex(NodeType::RPARENT);
    n->singleLex(NodeType::SEMICN);
}

void Parser::ReturnStmt(Node *n) {
    n->addChild(new Node(NodeType::RETURNTK));
    lexer.next();

    if (curLexType != NodeType::SEMICN) {
        n->addChild(Exp());
    }

    n->singleLex(NodeType::SEMICN);
}

void Parser::IfStmt(Node *n) {
    n->addChild(new Node(NodeType::IFTK));
    lexer.next();

    n->singleLex(NodeType::LPARENT);
    n->addChild(Cond());
    n->singleLex(NodeType::RPARENT);
    n->addChild(Stmt());

    if (curLexType == NodeType::ELSETK) {
        n->addChild(new Node(NodeType::ELSETK));
        lexer.next();

        n->addChild(Stmt());
    }
}

Node *Parser::ForStmt() {
    auto n = new Node(NodeType::ForStmt);

    n->addChild(LVal());
    n->singleLex(NodeType::ASSIGN);
    n->addChild(Exp());

    output(NodeType::ForStmt);
    return n;
}

Node *Parser::Exp() {
    auto n = new Node(NodeType::Exp);

    n->addChild(AddExp());

    output(NodeType::Exp);
    return n;
}

Node *Parser::Cond() {
    auto n = new Node(NodeType::Cond);

    n->addChild(LOrExp());

    output(NodeType::Cond);
    return n;
}

Node *Parser::LVal() {
    auto n = new Node(NodeType::LVal);

    n->addChild(Ident());

    while (curLexType == NodeType::LBRACK) {
        n->addChild(new Node(NodeType::LBRACK));
        lexer.next();
        n->addChild(Exp());
        n->singleLex(NodeType::RBRACK);
    }

    output(NodeType::LVal);
    return n;
}

Node *Parser::PrimaryExp() {
    auto n = new Node(NodeType::PrimaryExp);

    if (curLexType == NodeType::LPARENT) {
        n->addChild(new Node(NodeType::LPARENT));
        lexer.next();
        n->addChild(Exp());
        n->singleLex(NodeType::RPARENT);
    } else if (curLexType == NodeType::IDENFR) {
        n->addChild(LVal());
    } else if (curLexType == NodeType::INTCON) {
        n->addChild(Number());
    }

    output(NodeType::PrimaryExp);
    return n;
}

// todo: Set int value for Number or INTCON? Set for Number now.
Node *Parser::Number() {
    auto n = new Node(NodeType::Number);

    n->setValue(std::stoi(lexer.peek().second));
    n->singleLex(NodeType::INTCON);

    output(NodeType::Number);
    return n;
}

Node *Parser::UnaryExp() {
    auto n = new Node(NodeType::UnaryExp);

    switch (curLexType) {
        case NodeType::LPARENT:
        case NodeType::INTCON:
            n->addChild(PrimaryExp());
            break;

        case NodeType::IDENFR:
            // LVal → Ident {'[' Exp ']'}
            // PrimaryExp->LVal | Ident '(' [FuncRParams] ')'
            if (lexer.peek(1).first == NodeType::LPARENT) {
                n->addChild(Ident());
                n->singleLex(NodeType::LPARENT);

                if (curLexType != NodeType::RPARENT) {
                    n->addChild(FuncRParams());
                }

                n->singleLex(NodeType::RPARENT);
            } else {
                n->addChild(PrimaryExp());
            }
            break;

        case NodeType::PLUS:
        case NodeType::MINU:
        case NodeType::NOT:
            n->addChild(UnaryOp());
            n->addChild(UnaryExp());
            break;

        default:
            Error::raise_error();
    }

    output(NodeType::UnaryExp);
    return n;
}

Node *Parser::UnaryOp() {
    auto n = new Node(NodeType::UnaryOp);

    if (curLexType == NodeType::PLUS) {
        n->singleLex(NodeType::PLUS);
    } else if (curLexType == NodeType::MINU) {
        n->singleLex(NodeType::MINU);
    } else if (curLexType == NodeType::NOT) {
        n->singleLex(NodeType::NOT);
    } else {
        Error::raise_error();
    }

    output(NodeType::UnaryOp);
    return n;
}

Node *Parser::FuncRParams() {
    auto n = new Node(NodeType::FuncRParams);

    n->addChild(Exp());
    while (curLexType == NodeType::COMMA) {
        n->addChild(new Node(NodeType::COMMA));
        lexer.next();

        n->addChild(Exp());
    }

    output(NodeType::FuncRParams);
    return n;
}

// real struct is flat, but output is tree!
Node *Parser::MulExp() {
    auto n = new Node(NodeType::MulExp);

    n->addChild(UnaryExp());
    output(NodeType::MulExp);

    while (curLexType == NodeType::MULT || curLexType == NodeType::DIV || curLexType == NodeType::MOD) {
        n->singleLex(curLexType);
        n->addChild(UnaryExp());
        output(NodeType::MulExp);
    }

    return n;
}

// real struct is flat, but output is tree!
Node *Parser::AddExp() {
    auto n = new Node(NodeType::AddExp);

    n->addChild(MulExp());
    output(NodeType::AddExp);

    while (curLexType == NodeType::PLUS || curLexType == NodeType::MINU) {
        n->singleLex(curLexType);
        n->addChild(MulExp());
        output(NodeType::AddExp);
    }

    return n;
}

Node *Parser::RelExp() {
    auto n = new Node(NodeType::RelExp);

    n->addChild(AddExp());
    output(NodeType::RelExp);

    while (curLexType == NodeType::LSS || curLexType == NodeType::GRE ||
           curLexType == NodeType::LEQ || curLexType == NodeType::GEQ) {
        n->singleLex(curLexType);
        n->addChild(AddExp());
        output(NodeType::RelExp);
    }

    return n;
}

Node *Parser::EqExp() {
    auto n = new Node(NodeType::EqExp);

    n->addChild(RelExp());
    output(NodeType::EqExp);

    while (curLexType == NodeType::EQL || curLexType == NodeType::NEQ) {
        n->singleLex(curLexType);
        n->addChild(RelExp());
        output(NodeType::EqExp);
    }

    return n;
}

Node *Parser::LAndExp() {
    auto n = new Node(NodeType::LAndExp);

    n->addChild(EqExp());
    output(NodeType::LAndExp);

    while (curLexType == NodeType::AND) {
        n->singleLex(curLexType);
        n->addChild(EqExp());
        output(NodeType::LAndExp);
    }

    return n;
}

Node *Parser::LOrExp() {
    auto n = new Node(NodeType::LOrExp);

    n->addChild(LAndExp());
    output(NodeType::LOrExp);

    while (curLexType == NodeType::OR) {
        n->singleLex(curLexType);
        n->addChild(LAndExp());
        output(NodeType::LOrExp);
    }

    return n;
}

Node *Parser::ConstExp() {
    auto n = new Node(NodeType::ConstExp);

    n->addChild(AddExp());

    output(NodeType::ConstExp);
    return n;
}

Node *Parser::Ident() {
    auto n = new Node(NodeType::IDENFR);

    if (curLexType == NodeType::IDENFR) {
        n->setValue(lexer.peek().second);
        lexer.next();
    } else { Error::raise_error(); }

    return n;
}

Node *Parser::BType() {
    auto n = new Node(NodeType::Btype);

    n->singleLex(NodeType::INTTK);

//    output(NodeType::Btype);
    return n;
}

#pragma clang diagnostic pop