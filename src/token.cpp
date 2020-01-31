#include "token.h"

namespace FC { namespace FE {

const std::string Token::lparen = "(";
const std::string Token::rparen = ")";
const std::string Token::semicolon = ";";
const std::string Token::lbrace = "{";
const std::string Token::rbrace = "}";
const std::string Token::iftoken = "if";
const std::string Token::elsetoken = "else";
const std::string Token::whiletoken = "while";

StmToken::StmToken(const std::string &sstm) : sstm(sstm) {}

CondToken::CondToken(const std::string &cond) : cond(cond) {}

LBraceToken::LBraceToken() {}

RBraceToken::RBraceToken() {}

IfToken::IfToken() {}

ElseToken::ElseToken() {}

WhileToken::WhileToken() {}

} // namespace FE
} // namespace FC