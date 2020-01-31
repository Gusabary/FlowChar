#pragma once

#include <string>
#include <set>

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
        WHILE,
        END,
        SEQ,
        BEGIN
    };

    Kind kind;
    int state;
    std::pair<int, int> pos;

    static const char lparen;
    static const char rparen;
    static const char semicolon;
    static const char lbrace;
    static const char rbrace;
    static const std::string iftoken;
    static const std::string elsetoken;
    static const std::string whiletoken;
    static const std::set<char> delimiters;
    static const std::set<char> whitespaces;

    Token(const Kind kind);
    Token(const Kind kind, const int state);
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

class EndToken : public Token
{
public:
    EndToken();
};

class SeqToken : public Token
{
public:
    SeqToken();
};

class BeginToken : public Token
{
public:
    BeginToken();
};

} // namespace FE
} // namespace FC