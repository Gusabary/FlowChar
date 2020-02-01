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

Token::Token(const Kind kind, std::pair<int, int> pos) : kind(kind), pos(pos) {}

Token::Token(const Kind kind, const int state, std::pair<int, int> pos) : kind(kind), state(state), pos(pos) {}

StmToken::StmToken(const std::string &sstm, std::pair<int, int> pos) : Token(Token::STM, pos), sstm(sstm) {}

CondToken::CondToken(const std::string &cond, std::pair<int, int> pos) : Token(Token::COND, pos), cond(cond) {}

LBraceToken::LBraceToken(std::pair<int, int> pos) : Token(Token::LBRACE, pos) {}

RBraceToken::RBraceToken(std::pair<int, int> pos) : Token(Token::RBRACE, pos) {}

IfToken::IfToken(std::pair<int, int> pos) : Token(Token::IF, pos) {}

ElseToken::ElseToken(std::pair<int, int> pos) : Token(Token::ELSE, pos) {}

WhileToken::WhileToken(std::pair<int, int> pos) : Token(Token::WHILE, pos) {}

EndToken::EndToken() : Token(Token::END) {}

SeqToken::SeqToken() : Token(Token::SEQ) {}

BeginToken::BeginToken() : Token(Token::BEGIN, 1, std::make_pair<int, int>(0, 0)) {}


} // namespace FE
} // namespace FC