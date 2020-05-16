#include "token.h"

namespace FC { namespace FE {

const char Token::lparen = '(';
const char Token::rparen = ')';
const char Token::semicolon = ';';
const char Token::lbrace = '{';
const char Token::rbrace = '}';
const char Token::space = ' ';
const std::string Token::iftoken = "if";
const std::string Token::elsetoken = "else";
const std::string Token::whiletoken = "while";
const std::set<char> Token::delimiters = {';', '(', ')', '{', '}'};
const std::set<char> Token::whitespaces = {' ', '\n', '\t'};

Token::Token(const Kind kind) : kind(kind) {}

Token::Token(const Kind kind, std::pair<int, int> pos) : kind(kind), pos(pos) {}

Token::Token(const Kind kind, const int state, std::pair<int, int> pos) : kind(kind), state(state), pos(pos) {}

StmToken::StmToken(const std::string &sstm, std::pair<int, int> pos) : Token(Kind::STM, pos), sstm(sstm) {}

CondToken::CondToken(const std::string &cond, std::pair<int, int> pos) : Token(Kind::COND, pos), cond(cond) {}

LBraceToken::LBraceToken(std::pair<int, int> pos) : Token(Kind::LBRACE, pos) {}

RBraceToken::RBraceToken(std::pair<int, int> pos) : Token(Kind::RBRACE, pos) {}

IfToken::IfToken(std::pair<int, int> pos) : Token(Kind::IF, pos) {}

ElseToken::ElseToken(std::pair<int, int> pos) : Token(Kind::ELSE, pos) {}

WhileToken::WhileToken(std::pair<int, int> pos) : Token(Kind::WHILE, pos) {}

EndToken::EndToken() : Token(Kind::END) {}

SeqToken::SeqToken() : Token(Kind::SEQ) {}

BeginToken::BeginToken() : Token(Kind::BEGIN, 1, std::make_pair<int, int>(0, 0)) {}

} // namespace FE
} // namespace FC