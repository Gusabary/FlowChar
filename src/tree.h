#pragma once

#include <string>
#include <vector>

namespace FC { namespace IR {

// class SeqStm;

// class Tree
// {
// public:
//     SeqStm *SeqStm;
// };

class Stm
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
    Stm(const Kind kind);
    virtual void Print(int d) const = 0;
};

class SeqStm : public Stm
{
public:
    std::vector<Stm *> seq;

    SeqStm();
    SeqStm(Stm *const stm);

    void Print(int d) const override;

};

class SimpleStm : public Stm
{
public:
    std::string sstm;

    SimpleStm(const std::string &sstm);

    void Print(int d) const override;

};

class IfStm : public Stm
{
public:
    std::string cond;
    Stm *thent;
    Stm *elsee;

    IfStm(const std::string &cond, Stm *const thent);
    IfStm(const std::string &cond, Stm *const thent, Stm *const elsee);

    void Print(int d) const override;

};

class WhileStm : public Stm
{
public:
    std::string cond;
    Stm *body;

    WhileStm(const std::string &cond, Stm *const body);

    void Print(int d) const override;

};


} // namespace IR
} // namespace FC