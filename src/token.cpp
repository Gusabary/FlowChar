#include "token.h"

namespace FC { namespace FE {

const std::string LBraceToken::lbrace = "{";
const std::string RBraceToken::rbrace = "}";
const std::string IfToken::iftoken = "if";
const std::string ElseToken::elsetoken = "else";
const std::string WhileToken::whiletoken = "while";

StmToken::StmToken(const std::string &sstm) : sstm(sstm) {}

CondToken::CondToken(const std::string &cond) : cond(cond) {}

LBraceToken::LBraceToken() {}

RBraceToken::RBraceToken() {}

IfToken::IfToken() {}

ElseToken::ElseToken() {}

WhileToken::WhileToken() {}

} // namespace FE
} // namespace FC