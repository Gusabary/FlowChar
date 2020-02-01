#include <iostream>
#include <cstdlib>
#include <cassert>

#include "codeParser.h"

namespace FC { namespace FE {

CodeParser::CodeParser(const std::string &path, std::shared_ptr<IR::Stm> tree) : tree(tree) {
    this->fs = std::fstream();
    this->fs.open(path.c_str(), std::fstream::in);
    if (!this->fs.is_open()) {
        std::cout << "[Error] code file open failed." << std::endl;
        exit(-1);
    }
}

CodeParser::~CodeParser() {
    this->fs.close();
}

void CodeParser::scan() {
    char c;
    std::string str;
    while (this->fs.get(c))
    {
        if (Token::delimiters.find(c) != Token::delimiters.end()) {
            // c is a delimiter
            if (str == Token::iftoken) {
                this->tokenList.push_back(new IfToken());
            }
            else if (str == Token::elsetoken) {
                this->tokenList.push_back(new ElseToken());
            }
            else if (str == Token::whiletoken) {
                this->tokenList.push_back(new WhileToken());
            }
            else if (!str.empty()) {
                // cond or simple stm
                if (c == Token::rparen) {
                    // cond
                    this->tokenList.push_back(new CondToken(str));
                }
                else {
                    // simple stm
                    this->tokenList.push_back(new StmToken(str));
                }
            }

            if (c == Token::lbrace) {
                this->tokenList.push_back(new LBraceToken());
            }
            else if (c == Token::rbrace) {
                this->tokenList.push_back(new RBraceToken());
            }
            str.clear();
        }
        else {
            if (Token::whitespaces.find(c) == Token::whitespaces.end()) {
                // c is not a whitespace
                str += c;
            }
        }
    }
    this->tokenList.push_back(new EndToken());
    this->printTokenList();
}

void CodeParser::printTokenList() {
    for (Token *token : this->tokenList) {
        if (token->kind == Token::IF)
            std::cout << "if ";
        if (token->kind == Token::ELSE)
            std::cout << "else ";
        if (token->kind == Token::WHILE)
            std::cout << "while ";
        if (token->kind == Token::LBRACE)
            std::cout << "{" << std::endl;
        if (token->kind == Token::RBRACE)
            std::cout << "}" << std::endl;
        if (token->kind == Token::STM)
            std::cout << "stm-" << ((StmToken *)token)->sstm << std::endl;
        if (token->kind == Token::COND)
            std::cout << "cond-" << ((CondToken *)token)->cond << " ";
        if (token->kind == Token::END)
            std::cout << "$" << std::endl;
    }
}

void CodeParser::parse() {
    this->parseStack.push_back(new BeginToken());
    int nextToken = 0;

    while (true) {
        parsingTableEntry e = this->lookupParsingTable(this->parseStack.back()->state, this->tokenList[nextToken]->kind);
        if (e.action == parsingTableEntry::SHIFT) {
            this->tokenList[nextToken]->state = e.num;
            this->parseStack.push_back(this->tokenList[nextToken]);
            if (this->tokenList[nextToken]->kind == Token::STM) {
                IR::SimpleStm *sstm = new IR::SimpleStm(((StmToken *)(this->tokenList[nextToken]))->sstm);
                this->stmBuf.push_back(sstm);
            }
            nextToken++;
        }
        else if (e.action == parsingTableEntry::REDUCE) {
            IR::Stm *tmpStm;
            int back = this->stmBuf.size() - 1;
            if (e.num == 4)
            {
                assert(this->stmBuf[back]->kind != IR::Stm::SEQ);
                IR::Stm *newStm = new IR::SeqStm(this->stmBuf[back]);
                this->stmBuf.pop_back();
                this->stmBuf.push_back(newStm);
            }
            else if (e.num == 5) {
                assert(this->stmBuf[back]->kind == IR::Stm::SEQ);
                assert(this->stmBuf[back-1]->kind != IR::Stm::SEQ);
                IR::Stm *seq = this->stmBuf[back];
                ((IR::SeqStm *)seq)->seq.insert(((IR::SeqStm *)seq)->seq.begin(), this->stmBuf[back - 1]);
                this->stmBuf.pop_back();
                this->stmBuf.pop_back();
                this->stmBuf.push_back(seq);
            }
            else if (e.num == 3) {
                assert(this->stmBuf[back]->kind == IR::Stm::SEQ);
                assert(this->parseStack[this->parseStack.size() - 4]->kind == Token::COND);
                std::string cond = ((CondToken *)(this->parseStack[this->parseStack.size() - 4]))->cond;
                IR::Stm *newStm = new IR::WhileStm(cond, this->stmBuf[back]);
                this->stmBuf.pop_back();
                this->stmBuf.push_back(newStm);
            }
            else if (e.num == 2) {
                assert(this->stmBuf[back]->kind == IR::Stm::SEQ);
                assert(this->stmBuf[back-1]->kind == IR::Stm::SEQ);
                assert(this->parseStack[this->parseStack.size() - 8]->kind == Token::COND);
                std::string cond = ((CondToken *)(this->parseStack[this->parseStack.size() - 8]))->cond;
                IR::Stm *newStm = new IR::IfStm(cond, this->stmBuf[back - 1], this->stmBuf[back]);
                this->stmBuf.pop_back();
                this->stmBuf.pop_back();
                this->stmBuf.push_back(newStm);
            }
            else if (e.num == 1) {
                assert(this->stmBuf[back]->kind == IR::Stm::SEQ);
                assert(this->parseStack[this->parseStack.size() - 4]->kind == Token::COND);
                std::string cond = ((CondToken *)(this->parseStack[this->parseStack.size() - 4]))->cond;
                IR::Stm *newStm = new IR::IfStm(cond, this->stmBuf[back]);
                this->stmBuf.pop_back();
                this->stmBuf.push_back(newStm);
            }
            else {
                assert(0);
            }

            reductionInfo rinfo = this->getReductionInfo(e.num);
            for (int i = 0; i < rinfo.popNum; i++) {
                this->parseStack.pop_back();
            }
            int newState = this->lookupParsingTable(this->parseStack.back()->state, rinfo.newTokenKind).num;
            this->parseStack.push_back(new Token(rinfo.newTokenKind, newState));
        }
        else if (e.action == parsingTableEntry::ACCEPT) {
            assert(this->stmBuf.size() == 1);
            this->tree.reset(this->stmBuf[0]);
            break;
        }
        else {
            assert(0);
        }
    }
    this->tree->Print(0);
}

CodeParser::reductionInfo CodeParser::getReductionInfo(int productionNum) {
    switch (productionNum) {
        case 1:
            return reductionInfo(Token::STM, 5);
        case 2:
            return reductionInfo(Token::STM, 9);
        case 3:
            return reductionInfo(Token::STM, 5);
        case 4:
            return reductionInfo(Token::SEQ, 1);
        case 5:
            return reductionInfo(Token::SEQ, 2);
    }
    assert(0);
}

CodeParser::parsingTableEntry CodeParser::lookupParsingTable(int cntState, Token::Kind tokenKind) {
    switch (cntState) {
        case 1: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 4);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 3);
                case Token::SEQ:
                    return parsingTableEntry(parsingTableEntry::GOTO, 18);
                }
                assert(0);
        }
        case 2: {
            switch (tokenKind) {
                case Token::COND:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 6);
            }
            assert(0);
        }
        case 3: {
            switch (tokenKind) {
                case Token::COND:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 14);
            }
            assert(0);
        }
        case 4: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 4);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 3);
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 4);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 4);
                case Token::SEQ:
                    return parsingTableEntry(parsingTableEntry::GOTO, 5);
            }
            assert(0);
        }
        case 5: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 5);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 5);
            }
            assert(0);
        }
        case 6: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 7);
            }
            assert(0);
        }
        case 7: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 4);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 3);
                case Token::SEQ:
                    return parsingTableEntry(parsingTableEntry::GOTO, 8);
            }
            assert(0);
        }
        case 8: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 9);
            }
            assert(0);
        }
        case 9: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 1);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 1);
                case Token::ELSE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 10);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 1);
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 1);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 1);
            }
            assert(0);
        }
        case 10: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 11);
            }
            assert(0);
        }
        case 11: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 4);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 3);
                case Token::SEQ:
                    return parsingTableEntry(parsingTableEntry::GOTO, 12);
            }
            assert(0);
        }
        case 12: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 13);
            }
            assert(0);
        }
        case 13: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 2);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 2);
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 2);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 2);
            }
            assert(0);
        }
        case 14: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 15);
            }
            assert(0);
        }
        case 15: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 4);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 2);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 3);
                case Token::SEQ:
                    return parsingTableEntry(parsingTableEntry::GOTO, 16);
            }
            assert(0);
        }
        case 16: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 17);
            }
            assert(0);
        }
        case 17: {
            switch (tokenKind) {
                case Token::STM:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 3);
                case Token::IF:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 3);
                case Token::WHILE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 3);
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 3);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 3);
            }
            assert(0);
        }
        case 18: {
            switch (tokenKind) {
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::ACCEPT);
            }
            assert(0);
        }
    }
    assert(0);
}
}}