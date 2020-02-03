#include <iostream>
#include <algorithm>

#include "tree.h"
#include "box.h"

namespace FC { namespace IR {

Stm::Stm(const Kind kind) : kind(kind) {}

SimpleStm::SimpleStm(const std::string &sstm) : Stm(Stm::SIMPLE), sstm(sstm) {}

IfStm::IfStm(const std::string &cond, Stm *const thent) : Stm(Stm::IF), cond(cond), thent(thent), elsee(nullptr) {}

IfStm::IfStm(const std::string &cond, Stm *const thent, Stm *const elsee) : Stm(Stm::IF), cond(cond), thent(thent), elsee(elsee) {}

WhileStm::WhileStm(const std::string &cond, Stm *const body) : Stm(Stm::WHILE), cond(cond), body(body) {}

SeqStm::SeqStm() : Stm(Stm::SEQ) {}

SeqStm::SeqStm(Stm *const stm) : Stm(Stm::SEQ) {
    this->seq.push_back(stm);
}

BE::Box *SeqStm::Build() {
    BE::Box *seqBox = new BE::SeqBox();
    for (Stm *stm : this->seq) {
        ((BE::SeqBox *)seqBox)->seq.push_back(stm->Build());
    }
    return seqBox;
}

BE::Box *SimpleStm::Build() {
    return new BE::SimpleBox(this->sstm);
}

BE::Box *IfStm::Build() {
    if (!this->elsee) {
        return new BE::IfBox(this->cond, this->thent->Build());
    }
    return new BE::IfBox(this->cond, this->thent->Build(), this->elsee->Build());
}

BE::Box *WhileStm::Build() {
    return new BE::WhileBox(this->cond, this->body->Build());
}

void SeqStm::Print(int d) const {
    for (Stm *stm : this->seq) {
        stm->Print(d);
    }
}

static void indentHelper(int num) {
    for (int i = 0; i < num; i++) {
        std::cout << ' ';
    }
}

void SimpleStm::Print(int d) const {
    indentHelper(2 * d);
    std::cout << this->sstm << std::endl;
}

void IfStm::Print(int d) const {
    indentHelper(2 * d);
    std::cout << "if " << this->cond << std::endl;
    this->thent->Print(d + 1);
    if (this->elsee) {
        indentHelper(2 * d);
        std::cout << "else" << std::endl;
        this->elsee->Print(d + 1);
    }
}

void WhileStm::Print(int d) const {
    indentHelper(2 * d);
    std::cout << "while " << this->cond << std::endl;
    this->body->Print(d + 1);
}

}}