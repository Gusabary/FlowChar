#pragma once

#include <string>

namespace FC { namespace IR {

class Tree
{
};

class StmSeq;

class Stm
{
public:
    enum Kind
    {
        SIMPLE,
        IF,
        WHILE
    };

    Kind kind;
};

class SimpleStm : public Stm
{
public:
    std::string sstm;

    SimpleStm(const std::string &sstm);
};

class IfStm : public Stm
{
public:
    std::string cond;
    StmSeq *thent;
    StmSeq *elsee;

    IfStm(const std::string &cond, StmSeq *const thent);
    IfStm(const std::string &cond, StmSeq *const thent, StmSeq *const elsee);
};

class WhileStm : public Stm
{
public:
    std::string cond;
    StmSeq *body;

    WhileStm(const std::string &cond, StmSeq *const body);
};

class StmSeq
{
public:
    Stm *head;
    StmSeq *tail;

    StmSeq(Stm *const head, StmSeq *const tail);
};

} // namespace IR
} // namespace FC