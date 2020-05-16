#pragma once

#include <string>
#include <vector>

namespace FC { 

namespace BE {

class Box;

} // namespace BE

namespace IR {

class Stm
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
    Stm(const Kind kind);
    virtual void Print(int d) const = 0;
    virtual BE::Box *Build() = 0;
};

class SeqStm : public Stm
{
public:
    std::vector<Stm *> seq;

    SeqStm();
    SeqStm(Stm *const stm);

    void Print(int d) const override;
    BE::Box *Build() override;

};

class SimpleStm : public Stm
{
public:
    std::string sstm;

    SimpleStm(const std::string &sstm);

    void Print(int d) const override;
    BE::Box *Build() override;

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
    BE::Box *Build() override;

};

class WhileStm : public Stm
{
public:
    std::string cond;
    Stm *body;

    WhileStm(const std::string &cond, Stm *const body);

    void Print(int d) const override;
    BE::Box *Build() override;

};

} // namespace IR
} // namespace FC