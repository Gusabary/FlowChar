#include <iostream>
#include <cstdlib>
#include <cassert>

#include "codeParser.h"

namespace FC { namespace FE {

CodeParser::CodeParser(const std::string &path) {
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
    std::pair<int, int> cntPos = std::make_pair<int, int>(1, 0);
    while (this->fs.get(c))
    {
        // update current position
        if (c == '\n') {
            cntPos.first++;
            cntPos.second = 0;
        }
        else {
            cntPos.second++;
        }

        if (Token::delimiters.find(c) != Token::delimiters.end()) {
            // c is a delimiter
            if (str == Token::iftoken) {
                this->tokenList.push_back(new IfToken(cntPos));
            }
            else if (str == Token::elsetoken) {
                this->tokenList.push_back(new ElseToken(cntPos));
            }
            else if (str == Token::whiletoken) {
                this->tokenList.push_back(new WhileToken(cntPos));
            }
            else if (!str.empty()) {
                // cond or simple stm
                if (c == Token::rparen) {
                    // cond
                    this->tokenList.push_back(new CondToken(str, cntPos));
                }
                else {
                    // simple stm
                    this->tokenList.push_back(new StmToken(str, cntPos));
                }
            }

            if (c == Token::lbrace) {
                this->tokenList.push_back(new LBraceToken(cntPos));
            }
            else if (c == Token::rbrace) {
                this->tokenList.push_back(new RBraceToken(cntPos));
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
    // this->printTokenList();
}

void CodeParser::printTokenList() {
    for (Token *token : this->tokenList) {
        if (token->kind == Token::IF)
            std::cout << "if";
        if (token->kind == Token::ELSE)
            std::cout << "else";
        if (token->kind == Token::WHILE)
            std::cout << "while";
        if (token->kind == Token::LBRACE)
            std::cout << "{";
        if (token->kind == Token::RBRACE)
            std::cout << "}";
        if (token->kind == Token::STM)
            std::cout << "stm-" << ((StmToken *)token)->sstm;
        if (token->kind == Token::COND)
            std::cout << "cond-" << ((CondToken *)token)->cond;
        if (token->kind == Token::END)
            std::cout << "$" << std::endl;
        std::cout << "   (" << token->pos.first << ", " << token->pos.second << ")" << std::endl;
    }
}

std::shared_ptr<IR::Stm> CodeParser::parse() {
    std::vector<Token *> parseStack;
    std::vector<IR::Stm *> stmBuf;
    std::shared_ptr<IR::Stm> tree;

    parseStack.push_back(new BeginToken());
    int nextToken = 0;

    while (true) {
        parsingTableEntry e = this->lookupParsingTable(parseStack.back()->state, this->tokenList[nextToken]->kind, parseStack.back()->pos);
        if (e.action == parsingTableEntry::SHIFT) {
            this->tokenList[nextToken]->state = e.num;
            parseStack.push_back(this->tokenList[nextToken]);
            if (this->tokenList[nextToken]->kind == Token::STM) {
                IR::SimpleStm *sstm = new IR::SimpleStm(((StmToken *)(this->tokenList[nextToken]))->sstm);
                stmBuf.push_back(sstm);
            }
            nextToken++;
        }
        else if (e.action == parsingTableEntry::REDUCE) {
            IR::Stm *tmpStm;
            int back = stmBuf.size() - 1;
            if (e.num == 4)
            {
                assert(stmBuf[back]->kind != IR::Stm::SEQ);
                IR::Stm *newStm = new IR::SeqStm(stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 5) {
                assert(stmBuf[back]->kind == IR::Stm::SEQ);
                assert(stmBuf[back-1]->kind != IR::Stm::SEQ);
                IR::Stm *seq = stmBuf[back];
                ((IR::SeqStm *)seq)->seq.insert(((IR::SeqStm *)seq)->seq.begin(), stmBuf[back - 1]);
                stmBuf.pop_back();
                stmBuf.pop_back();
                stmBuf.push_back(seq);
            }
            else if (e.num == 3) {
                assert(stmBuf[back]->kind == IR::Stm::SEQ);
                assert(parseStack[parseStack.size() - 4]->kind == Token::COND);
                std::string cond = ((CondToken *)(parseStack[parseStack.size() - 4]))->cond;
                IR::Stm *newStm = new IR::WhileStm(cond, stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 2) {
                assert(stmBuf[back]->kind == IR::Stm::SEQ);
                assert(stmBuf[back-1]->kind == IR::Stm::SEQ);
                assert(parseStack[parseStack.size() - 8]->kind == Token::COND);
                std::string cond = ((CondToken *)(parseStack[parseStack.size() - 8]))->cond;
                IR::Stm *newStm = new IR::IfStm(cond, stmBuf[back - 1], stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 1) {
                assert(stmBuf[back]->kind == IR::Stm::SEQ);
                assert(parseStack[parseStack.size() - 4]->kind == Token::COND);
                std::string cond = ((CondToken *)(parseStack[parseStack.size() - 4]))->cond;
                IR::Stm *newStm = new IR::IfStm(cond, stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else {
                assert(0);
            }

            reductionInfo rinfo = this->getReductionInfo(e.num);
            std::pair<int, int> newPos = parseStack.back()->pos;
            for (int i = 0; i < rinfo.popNum; i++)
            {
                parseStack.pop_back();
            }
            int newState = this->lookupParsingTable(parseStack.back()->state, rinfo.newTokenKind, parseStack.back()->pos).num;
            parseStack.push_back(new Token(rinfo.newTokenKind, newState, newPos));
        }
        else if (e.action == parsingTableEntry::ACCEPT) {
            assert(stmBuf.size() == 1);
            tree.reset(stmBuf[0]);
            break;
        }
        else {
            assert(0);
        }
    }
    // tree->Print(0);
    return tree;
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

CodeParser::parsingTableEntry CodeParser::lookupParsingTable(int cntState, Token::Kind tokenKind, std::pair<int, int> pos) {
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
                std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 2: {
            switch (tokenKind) {
                case Token::COND:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 6);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 3: {
            switch (tokenKind) {
                case Token::COND:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 14);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 5: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 5);
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::REDUCE, 5);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 6: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 7);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 8: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 9);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 10: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 11);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 12: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 13);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 14: {
            switch (tokenKind) {
                case Token::LBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 15);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 16: {
            switch (tokenKind) {
                case Token::RBRACE:
                    return parsingTableEntry(parsingTableEntry::SHIFT, 17);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
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
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 18: {
            switch (tokenKind) {
                case Token::END:
                    return parsingTableEntry(parsingTableEntry::ACCEPT);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
    }
    assert(0);
}
}}