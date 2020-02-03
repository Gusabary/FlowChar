#include "box.h"

namespace FC { namespace BE {

Box::Box(const Kind kind) : kind(kind) {}

SeqBox::SeqBox() : Box(Box::SEQ) {}

SimpleBox::SimpleBox(const std::string &content) : Box(Box::SIMPLE), content(content) {}

IfBox::IfBox(const std::string &content, Box *const thent) : Box(Box::IF), thent(thent) {}

IfBox::IfBox(const std::string &content, Box *const thent, Box *const elsee) : Box(Box::IF), thent(thent), elsee(elsee) {}

WhileBox::WhileBox(const std::string &content, Box *const body) : Box(Box::WHILE), content(content), body(body) {}

AttachInfo SeqBox::Attach() {
    int maxLWidth = 0;
    int maxRWidth = 0;
    for (Box *box : this->seq) {
        AttachInfo ainfo = box->Attach();
        maxLWidth = std::max(maxLWidth, ainfo.lWidth);
        maxRWidth = std::max(maxRWidth, ainfo.rWidth);
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

AttachInfo WhileBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    AttachInfo ainfo = this->body->Attach();
    this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + 4;
    this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + 4;

    return AttachInfo(this->lWidth, this->rWidth);
}

}}


