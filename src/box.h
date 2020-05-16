#pragma once

#include <memory>

#include "tree.h"

namespace FC { namespace BE {

struct AttachInfo {
    int lWidth;
    int rWidth;
    int height;

    AttachInfo(int lWidth, int rWidth, int height) : lWidth(lWidth), rWidth(rWidth), height(height) {}
};

using posT = std::pair<int, int>;
using chartT = std::vector<std::vector<char>>;

struct DrawInfo {
    int height;
    posT arrowAFrom;  // make sense only to if-box without else branch and while-box
    posT arrowBFrom;  // make sense only to if-box with else branch

    DrawInfo() {}
    DrawInfo(int height) : height(height) {}
    DrawInfo(int height, posT arrowAFrom) : height(height), arrowAFrom(arrowAFrom) {}
    DrawInfo(int height, posT arrowAFrom, posT arrowBFrom) : height(height), arrowAFrom(arrowAFrom), arrowBFrom(arrowBFrom) {}
};

class Box
{
public:
    enum class Kind : int
    {
        SIMPLE,
        IF,
        WHILE,
        SEQ
    };

    Kind kind;
    int lWidth;  // width of left side of axis
    int rWidth;  // width of right side of axis
    int width;  // width of the stm box, must be odd
    int height;

    Box(const Kind kind);
    virtual AttachInfo Attach() = 0;
    virtual DrawInfo Draw(chartT &chart, const posT &pos) = 0;
    virtual void Print(int d) const = 0;
};

class SeqBox : public Box
{
public:
    std::vector<Box *> seq;

    SeqBox();
    
    AttachInfo Attach() override;
    DrawInfo Draw(chartT &chart, const posT &pos) override;
    void Print(int d) const override;
};

class SimpleBox : public Box
{
public:
    std::string content;

    SimpleBox(const std::string &content);
  
    AttachInfo Attach() override;
    DrawInfo Draw(chartT &chart, const posT &pos) override;
    void Print(int d) const override;
};

class IfBox : public Box
{
public:
    std::string content;
    Box *thent;
    Box *elsee;

    bool hasElse;
    bool nSide;  // if true, 'no' branch is at left side. make sense without else branch
    int axisDistance;  // distance between side axis and center one. make sense with else branch
    bool hasNext;  // if true, can point to a box eventually, otherwise point to 'O'

    IfBox(const std::string &content, Box *const thent);
    IfBox(const std::string &content, Box *const thent, Box *const elsee);
  
    AttachInfo Attach() override;
    DrawInfo Draw(chartT &chart, const posT &pos) override;
    void Print(int d) const override;
};

class WhileBox : public Box
{
public:
    std::string content;
    Box *body;

    bool hasNext;  // if true, can point to a box eventually, otherwise point to 'O'
    bool needExtraO;  // if true, backward arrow should start from an extra 'O'

    WhileBox(const std::string &content, Box *const body);
  
    AttachInfo Attach() override;
    DrawInfo Draw(chartT &chart, const posT &pos) override;
    void Print(int d) const override;
};

} // namespace BE
} // namespace FC