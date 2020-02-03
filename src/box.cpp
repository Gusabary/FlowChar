#include <iostream>
#include <math.h>
#include <cassert>

#include "box.h"

namespace FC { namespace BE {

Box::Box(const Kind kind) : kind(kind) {}

SeqBox::SeqBox() : Box(Box::SEQ) {}

SimpleBox::SimpleBox(const std::string &content) : Box(Box::SIMPLE), content(content) {}

IfBox::IfBox(const std::string &content, Box *const thent) : Box(Box::IF), content(content), thent(thent), hasNext(false) {}

IfBox::IfBox(const std::string &content, Box *const thent, Box *const elsee) : Box(Box::IF), content(content), thent(thent), elsee(elsee), hasNext(false) {}

WhileBox::WhileBox(const std::string &content, Box *const body) : Box(Box::WHILE), content(content), body(body), hasNext(false) {}

AttachInfo SeqBox::Attach() {
    int maxLWidth = 0;
    int maxRWidth = 0;
    // for (Box *box : this->seq) {
    for (int i = 0; i < this->seq.size(); i++) {
        AttachInfo ainfo = this->seq[i]->Attach();
        maxLWidth = std::max(maxLWidth, ainfo.lWidth);
        maxRWidth = std::max(maxRWidth, ainfo.rWidth);

        // determine whether the if box points to a simple box or 'O' eventually
        if (i > 0 && this->seq[i-1]->kind == Box::IF && this->seq[i]->kind == Box::SIMPLE) {
            IfBox *ifBox = ((IfBox *)(this->seq[i - 1]));
            SimpleBox *simpleBox = ((SimpleBox *)(this->seq[i]));
            ifBox->hasNext = true;

            // update width info if necessary
            if (!ifBox->hasElse) {
                if (ifBox->nSide)
                    ifBox->lWidth = std::max(ifBox->lWidth - 4, simpleBox->lWidth) + 4;
                else
                    ifBox->rWidth = std::max(ifBox->rWidth - 4, simpleBox->rWidth) + 4;
            }
            else {
                assert(simpleBox->lWidth == simpleBox->rWidth);
                int oldAxisDtstance = ifBox->axisDistance;
                ifBox->axisDistance = std::max(oldAxisDtstance, simpleBox->lWidth + 3);
                ifBox->lWidth += (ifBox->axisDistance - oldAxisDtstance);
                ifBox->rWidth += (ifBox->axisDistance - oldAxisDtstance);
            }
        }
        // determine whether the while box points to a simple box or 'O' eventually
        else if (i > 0 && this->seq[i-1]->kind == Box::WHILE && this->seq[i]->kind == Box::SIMPLE) {
            WhileBox *whileBox = ((WhileBox *)(this->seq[i - 1]));
            SimpleBox *simpleBox = ((SimpleBox *)(this->seq[i]));
            whileBox->hasNext = true;
            
            // update width info if necessary
            whileBox->lWidth = std::max(whileBox->lWidth - 4, simpleBox->lWidth) + 4;
        }
    }
    return AttachInfo(maxLWidth, maxRWidth);
}

AttachInfo SimpleBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    this->lWidth = (this->width - 1) / 2;
    this->rWidth = (this->width - 1) / 2;

    return AttachInfo(this->lWidth, this->rWidth);
}

AttachInfo IfBox::Attach() {
    const int length = this->content.size();
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

        // fixed width of branch shoulder, must be int
        int minAxisDistance = (3 + this->width + 3 - 1) / 2;
        // fixed distance between both branches, may be float
        int actualAxisDistance = ceil((lainfo.rWidth + 3 + rainfo.lWidth - 1) / 2.0);
        this->axisDistance = std::max(actualAxisDistance, minAxisDistance);

        this->lWidth = this->axisDistance + lainfo.lWidth + 1;
        this->rWidth = this->axisDistance + rainfo.rWidth + 1;
    }
    return AttachInfo(this->lWidth, this->rWidth);
}

AttachInfo WhileBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    AttachInfo ainfo = this->body->Attach();
    this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + 4;
    this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + 4;

    return AttachInfo(this->lWidth, this->rWidth);
}


void SeqBox::Print(int d) const {
    for (Box *box : this->seq) {
        box->Print(d);
    }
}

static void indentHelper(int num) {
    for (int i = 0; i < num; i++) {
        std::cout << ' ';
    }
}

void SimpleBox::Print(int d) const {
    indentHelper(2 * d);
    std::cout << this->content << "  (W: " << this->width
     << ", L: " << this->lWidth
     << ", R: " << this->rWidth << ")" << std::endl;
}

void IfBox::Print(int d) const {
    indentHelper(2 * d);
    std::cout << "if " << this->content << "  (W: " << this->width
     << ", L: " << this->lWidth
     << ", R: " << this->rWidth 
     << ", AD: " << this->axisDistance << ")" << std::endl;
    this->thent->Print(d + 1);
    if (this->elsee) {
        indentHelper(2 * d);
        std::cout << "else" << std::endl;
        this->elsee->Print(d + 1);
    }
}

void WhileBox::Print(int d) const {
    indentHelper(2 * d);
    std::cout << "while " << this->content << "  (W: " << this->width
     << ", L: " << this->lWidth
     << ", R: " << this->rWidth << ")" << std::endl;
    this->body->Print(d + 1);
}

}}


