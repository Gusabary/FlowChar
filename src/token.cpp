#include "token.h"

namespace FC { namespace FE {

const char Token::lparen = '(';
const char Token::rparen = ')';
const char Token::semicolon = ';';
const char Token::lbrace = '{';
const char Token::rbrace = '}';
const std::string Token::iftoken = "if";
const std::string Token::elsetoken = "else";
const std::string Token::whiletoken = "while";
const std::set<char> Token::delimiters = {';', '(', ')', '{', '}'};
const std::set<char> Token::whitespaces = {' ', '\n', '\t'};

Token::Token(const Kind kind) : kind(kind) {}

StmToken::StmToken(const std::string &sstm) : Token(Token::STM), sstm(sstm) {}

CondToken::CondToken(const std::string &cond) : Token(Token::COND), cond(cond) {}

LBraceToken::LBraceToken() : Token(Token::LBRACE) {}

RBraceToken::RBraceToken() : Token(Token::RBRACE) {}

IfToken::IfToken() : Token(Token::IF) {}

ElseToken::ElseToken() : Token(Token::ELSE) {}

WhileToken::WhileToken() : Token(Token::WHILE) {}

} // namespace FE
} // namespace FC