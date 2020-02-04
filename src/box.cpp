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
    int height = 0;

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

        height += (ainfo.height + 2);
    }

    // if point to 'O' eventually, height would be larger
    for (Box *box : this->seq) {
        if ((box->kind == Box::IF && !((IfBox *)(box))->hasNext && !((IfBox *)(box))->hasElse) || 
         (box->kind == Box::WHILE && !((WhileBox *)(box))->hasNext)) {
            height += 3;
        }
        else if (box->kind == Box::IF && !((IfBox *)(box))->hasNext && ((IfBox *)(box))->hasElse) {
            height += 2;
        }
    }
    return AttachInfo(maxLWidth, maxRWidth, height - 2);
}

AttachInfo SimpleBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    this->lWidth = (this->width - 1) / 2;
    this->rWidth = (this->width - 1) / 2;

    return AttachInfo(this->lWidth, this->rWidth, 3);
}

AttachInfo IfBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    int height;

    if (!this->elsee) {
        // only one side
        this->hasElse = false;
        AttachInfo ainfo = this->thent->Attach();
        
        // if left side is shorter, arrange 'no' branch at left to make flowchart more balanced
        this->nSide = (ainfo.lWidth < ainfo.rWidth);
        this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + (this->nSide ? 4 : 0);
        this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + (this->nSide ? 0 : 4);
        height = ainfo.height;
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
        height = std::max(lainfo.height, rainfo.height);
    }
    return AttachInfo(this->lWidth, this->rWidth, height + 5);
}

AttachInfo WhileBox::Attach() {
    const int length = this->content.size();
    this->width = length + ((length % 2 == 0) ? 9 : 8);
    AttachInfo ainfo = this->body->Attach();
    this->lWidth = std::max((this->width - 1) / 2, ainfo.lWidth) + 4;
    this->rWidth = std::max((this->width - 1) / 2, ainfo.rWidth) + 4;

    return AttachInfo(this->lWidth, this->rWidth, ainfo.height + 5);
}

static void drawArrow(chartT &chart, const posT &from, const posT &to, const int vertical = -1) {
    // no turning point, vertical arrow, used in simple-box
    if (vertical < 0) {
        assert(from.second == to.second);
        for (int i = from.first; i < to.first; i++)
            chart[i][from.second] = '|';
        chart[to.first][from.second] = 'V';
        return;
    }

    // one turning point, used in if-box with else branch
    if (from.second == vertical) {
        // first vertical, then horizontal
        for (int i = from.first; i < to.first; i++)
            chart[i][vertical] = '|';
        for (int i = std::min(vertical, to.second); i <= std::max(vertical, to.second); i++)
            chart[to.first][i] = '-';
        chart[to.first][vertical] = '+';
        chart[to.first][to.second] = (vertical < to.second) ? '>' : '<';
        return;
    }
    if (to.second == vertical) {
        // first horizontal, then vertical
        for (int i = std::min(from.second, vertical); i <= std::max(from.second, vertical); i++)
            chart[from.first][i] = '-';
        chart[from.first][vertical] = '+';
        for (int i = from.first; i < to.first; i++)
            chart[i][vertical] = '|';
        chart[to.first][to.second] = 'V';
        return;
    }

    // two turning points, used in if-box without else branch and while-box
    for (int i = std::min(from.second, vertical); i <= std::max(from.second, vertical); i++)
        chart[from.first][i] = '-';
    for (int i = std::min(from.first, to.first); i <= std::max(from.first, to.first); i++)
        chart[i][vertical] = '|';
    for (int i = std::min(to.second, vertical); i <= std::max(to.second, vertical); i++)
        chart[to.first][i] = '-';
    chart[from.first][vertical] = '+';
    chart[to.first][vertical] = '+';
    chart[to.first][to.second] = (vertical < to.second) ? '>' : '<';
    return;
}

DrawInfo SeqBox::Draw(chartT &chart, const posT &pos) {
    int row = pos.first;
    const int col = pos.second;
    for (int i = 0; i < this->seq.size(); i++)
    {
        DrawInfo dinfo = this->seq[i]->Draw(chart, std::make_pair(row, col));
        row += dinfo.height;

        if (i == this->seq.size() - 1)
            return dinfo;

        // draw arrow
        if (this->seq[i]->kind == Box::SIMPLE) {
            drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 2, col));
        }
        else if (this->seq[i]->kind == Box::IF) {
            IfBox *ifBox = (IfBox *)(this->seq[i]);
            if (!ifBox->hasElse) {
                if (ifBox->hasNext) {
                    drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 2, col));  // vertical arrow
                    int nextHalfWidth = (this->seq[i + 1]->width - 1) / 2;
                    if (ifBox->nSide)
                        drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row, col - nextHalfWidth - 1), col - this->seq[i]->lWidth);
                    else
                        drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row, col + nextHalfWidth + 1), col + this->seq[i]->rWidth);
                }
                else {
                    drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 5, col));  // vertical arrow
                    chart[row - 4][col] = 'O';
                    if (ifBox->nSide)
                        drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row - 4, col - 1), col - this->seq[i]->lWidth);
                    else
                        drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row - 4, col + 1), col + this->seq[i]->rWidth);
                    drawArrow(chart, std::make_pair(row - 3, col), std::make_pair(row - 2, col)); // vertical arrow
                }
            }
            else {
                if (ifBox->hasNext) {
                    int nextHalfWidth = (this->seq[i + 1]->width - 1) / 2;
                    drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row, col - nextHalfWidth - 1), dinfo.arrowAFrom.second);
                    drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row, col + nextHalfWidth + 1), dinfo.arrowBFrom.second);
                }
                else {
                    drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 4, col - 1), dinfo.arrowAFrom.second);
                    drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row - 4, col + 1), dinfo.arrowBFrom.second);
                    chart[row - 4][col] = 'O';
                    drawArrow(chart, std::make_pair(row - 3, col), std::make_pair(row - 2, col)); // vertical arrow
                }
            }
        }
        else if (this->seq[i]->kind == Box::WHILE) {
            WhileBox *whileBox = (WhileBox *)(this->seq[i]);
            if (whileBox->hasNext) {
                drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 2, col)); // vertical arrow
                int nextHalfWidth = (this->seq[i + 1]->width - 1) / 2;
                drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row, col - nextHalfWidth - 1), col - this->seq[i]->lWidth);
            }
            else {
                drawArrow(chart, dinfo.arrowAFrom, std::make_pair(row - 5, col)); // vertical arrow
                chart[row - 4][col] = 'O';
                drawArrow(chart, dinfo.arrowBFrom, std::make_pair(row - 4, col - 1), col - this->seq[i]->lWidth);
                drawArrow(chart, std::make_pair(row - 3, col), std::make_pair(row - 2, col)); // vertical arrow
            }
        }
        else {
            assert(0);
        }
    }
}

DrawInfo SimpleBox::Draw(chartT &chart, const posT &pos) {
    // border
    int halfWidth = (this->width - 1) / 2;
    chart[pos.first - 1][pos.second - halfWidth] = '+';
    chart[pos.first - 1][pos.second + halfWidth] = '+';
    chart[pos.first][pos.second - halfWidth] = '|';
    chart[pos.first][pos.second + halfWidth] = '|';
    chart[pos.first + 1][pos.second - halfWidth] = '+';
    chart[pos.first + 1][pos.second + halfWidth] = '+';
    for (int i = pos.second - halfWidth + 1; i < pos.second + halfWidth; i++) {
        chart[pos.first - 1][i] = '-';
        chart[pos.first + 1][i] = '-';
    }

    // content
    int length = this->content.size();
    int start = pos.second - (length - 1) / 2;
    for (int i = start; i < start + length; i++) {
        chart[pos.first][i] = this->content[i - start];
    }

    return DrawInfo(5, std::make_pair(pos.first + 2, pos.second));
}

DrawInfo IfBox::Draw(chartT &chart, const posT &pos) {

}

DrawInfo WhileBox::Draw(chartT &chart, const posT &pos) {

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


