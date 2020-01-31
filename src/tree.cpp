#include "tree.h"

namespace FC { namespace IR {

SimpleStm::SimpleStm(const std::string &sstm) : sstm(sstm) {}

IfStm::IfStm(const std::string &cond, StmSeq *const thent) : cond(cond), thent(thent), elsee(nullptr) {}

IfStm::IfStm(const std::string &cond, StmSeq *const thent, StmSeq *const elsee) : cond(cond), thent(thent), elsee(elsee) {}

WhileStm::WhileStm(const std::string &cond, StmSeq *const body) : cond(cond), body(body) {}

StmSeq::StmSeq(Stm *const head, StmSeq *const tail) : head(head), tail(tail) {}

}}