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
    static const std::string lbrace;

    LBraceToken();
};

class RBraceToken : public Token
{
public:
    static const std::string rbrace;

    RBraceToken();
};

class IfToken : public Token
{
public:
    static const std::string iftoken;

    IfToken();
};

class ElseToken : public Token
{
public:
    static const std::string elsetoken;

    ElseToken();
};

class WhileToken : public Token
{
public:
    static const std::string whiletoken;

    WhileToken();
};

} // namespace FE
} // namespace FC