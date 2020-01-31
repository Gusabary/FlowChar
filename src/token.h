#pragma once

#include <string>

namespace FC { namespace FE {

class Token
{  
public:
    enum Kind
    {
        STM,
        COND,
        LBRACE,
        RBRACE,
        IF,
        ELSE,
        WHILE
    };

    Kind kind;
    int state;
    std::pair<int, int> pos;

    static const std::string lparen;
    static const std::string rparen;
    static const std::string semicolon;
    static const std::string lbrace;
    static const std::string rbrace;
    static const std::string iftoken;
    static const std::string elsetoken;
    static const std::string whiletoken;
};

class StmToken : public Token
{
public:
    std::string sstm;

    StmToken(const std::string &sstm);
};

class CondToken : public Token
{
public:
    std::string cond;

    CondToken(const std::string &cond);
};

class LBraceToken : public Token
{
public:
    LBraceToken();
};

class RBraceToken : public Token
{
public:
    RBraceToken();
};

class IfToken : public Token
{
public:
    IfToken();
};

class ElseToken : public Token
{
public:
    ElseToken();
};

class WhileToken : public Token
{
public:
    WhileToken();
};

} // namespace FE
} // namespace FC