#pragma once

#include <memory>

#include "tree.h"

namespace FC { namespace BE {

struct AttachInfo {
    int lWidth;
    int rWidth;

    AttachInfo(int lWidth, int rWidth) : lWidth(lWidth), rWidth(rWidth) {}
};

class Box
{
public:
    enum Kind
    {
        SIMPLE,
        IF,
        WHILE,
        SEQ
    };

    Kind kind;
    Box(const Kind kind);
    virtual AttachInfo Attach() = 0;
    virtual void Print(int d) const = 0;
};

class SeqBox : public Box
{
public:
    std::vector<Box *> seq;

    SeqBox();
    
    AttachInfo Attach() override;
    void Print(int d) const override;
};

class SimpleBox : public Box
{
public:
    std::string content;

    int lWidth;  // width of left side of axis
    int rWidth;  // width of right side of axis
    std::pair<int, int> pos;
    int width;  // width of the stm box, must be odd

    SimpleBox(const std::string &content);
  
    AttachInfo Attach() override;
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
    int lWidth;  // width of left side of axis
    int rWidth;  // width of right side of axis
    std::pair<int, int> pos;
    int width;  // width of the cond box, must be odd
    int axisDistance;  // distance between side axis and center one. make sense with else branch
    bool hasNext;  // if true, can point to a box eventually, otherwise point to 'O'

    IfBox(const std::string &content, Box *const thent);
    IfBox(const std::string &content, Box *const thent, Box *const elsee);
  
    AttachInfo Attach() override;
    void Print(int d) const override;
};

class WhileBox : public Box
{
public:
    std::string content;
    Box *body;

    int lWidth;  // width of left side of axis
    int rWidth;  // width of right side of axis
    std::pair<int, int> pos;
    int width;  // width of the cond box, must be odd
    bool hasNext;  // if true, can point to a box eventually, otherwise point to 'O'

    WhileBox(const std::string &content, Box *const body);
  
    AttachInfo Attach() override;
    void Print(int d) const override;
};

} // namespace BE
} // namespace FC