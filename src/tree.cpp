#include <iostream>
#include <algorithm>

#include "tree.h"

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

AttachInfo SeqStm::Attach() {
    int maxLWidth = 0;
    int maxRWidth = 0;
    for (Stm *stm : this->seq) {
        AttachInfo ainfo = stm->Attach();
        maxLWidth = std::max(maxLWidth, ainfo.lWidth);
        maxRWidth = std::max(maxRWidth, ainfo.rWidth);
    }
    return AttachInfo(maxLWidth, maxRWidth);
}

AttachInfo SimpleStm::Attach() {
    const int length = this->sstm.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    this->lWidth = (this->width - 1) / 2;
    this->rWidth = (this->width - 1) / 2;

    return AttachInfo(this->lWidth, this->rWidth);
}

AttachInfo IfStm::Attach() {
    const int length = this->cond.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);

    if (!this->elsee) {
        // only one side
        this->hasElse = false;
        AttachInfo ainfo = this->thent->Attach();
        
        // if left side is shorter, arrange 'no' branch at left to make flowchart more balanced
        this->nSide = (ainfo.lWidth < ainfo.rWidth);
        this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + (this->nSide ? 4 : 0);
        this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + (this->nSide ? 0 : 4);
    }
    else {
        // both sides
        this->hasElse = true;
        
        // 'yes' branch is at left side and 'no' branch is at right one
        AttachInfo lainfo = this->thent->Attach();
        AttachInfo rainfo = this->elsee->Attach();

        // fixed width of branch shoulder
        int minAxisDistance = 4 + (this->width - 1) / 2;
        // fixed distance between both branches
        int actualAxisDistance = (lainfo.rWidth + rainfo.lWidth + 3) / 2;
        this->axisDistance = std::max(actualAxisDistance, minAxisDistance);

        this->lWidth = this->axisDistance + lainfo.lWidth;
        this->rWidth = this->axisDistance + rainfo.rWidth;
    }
    return AttachInfo(this->lWidth, this->rWidth);
}

AttachInfo WhileStm::Attach() {
    const int length = this->cond.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    AttachInfo ainfo = this->body->Attach();
    this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + 4;
    this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + 4;

    return AttachInfo(this->lWidth, this->rWidth);
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