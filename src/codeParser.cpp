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
            // first erase trailing space
            str.erase(str.find_last_not_of(' ') + 1);
            
            if (str == Token::iftoken)
            {
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
            else if (c == Token::space && !str.empty()) {
                // ignore leading space
                str += c;
            }
        }
    }
    this->tokenList.push_back(new EndToken());
    // this->printTokenList();
}

void CodeParser::printTokenList() {
    for (Token *token : this->tokenList) {
        if (token->kind == Token::Kind::IF)
            std::cout << "if";
        if (token->kind == Token::Kind::ELSE)
            std::cout << "else";
        if (token->kind == Token::Kind::WHILE)
            std::cout << "while";
        if (token->kind == Token::Kind::LBRACE)
            std::cout << "{";
        if (token->kind == Token::Kind::RBRACE)
            std::cout << "}";
        if (token->kind == Token::Kind::STM)
            std::cout << "stm-" << ((StmToken *)token)->sstm;
        if (token->kind == Token::Kind::COND)
            std::cout << "cond-" << ((CondToken *)token)->cond;
        if (token->kind == Token::Kind::END)
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
        if (e.action == parsingTableEntry::Action::SHIFT) {
            this->tokenList[nextToken]->state = e.num;
            parseStack.push_back(this->tokenList[nextToken]);
            if (this->tokenList[nextToken]->kind == Token::Kind::STM) {
                IR::SimpleStm *sstm = new IR::SimpleStm(((StmToken *)(this->tokenList[nextToken]))->sstm);
                stmBuf.push_back(sstm);
            }
            nextToken++;
        }
        else if (e.action == parsingTableEntry::Action::REDUCE) {
            IR::Stm *tmpStm;
            int back = stmBuf.size() - 1;
            if (e.num == 4)
            {
                assert(stmBuf[back]->kind != IR::Stm::Kind::SEQ);
                IR::Stm *newStm = new IR::SeqStm(stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 5) {
                assert(stmBuf[back]->kind == IR::Stm::Kind::SEQ);
                assert(stmBuf[back-1]->kind != IR::Stm::Kind::SEQ);
                IR::Stm *seq = stmBuf[back];
                ((IR::SeqStm *)seq)->seq.insert(((IR::SeqStm *)seq)->seq.begin(), stmBuf[back - 1]);
                stmBuf.pop_back();
                stmBuf.pop_back();
                stmBuf.push_back(seq);
            }
            else if (e.num == 3) {
                assert(stmBuf[back]->kind == IR::Stm::Kind::SEQ);
                assert(parseStack[parseStack.size() - 4]->kind == Token::Kind::COND);
                std::string cond = ((CondToken *)(parseStack[parseStack.size() - 4]))->cond;
                IR::Stm *newStm = new IR::WhileStm(cond, stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 2) {
                assert(stmBuf[back]->kind == IR::Stm::Kind::SEQ);
                assert(stmBuf[back-1]->kind == IR::Stm::Kind::SEQ);
                assert(parseStack[parseStack.size() - 8]->kind == Token::Kind::COND);
                std::string cond = ((CondToken *)(parseStack[parseStack.size() - 8]))->cond;
                IR::Stm *newStm = new IR::IfStm(cond, stmBuf[back - 1], stmBuf[back]);
                stmBuf.pop_back();
                stmBuf.pop_back();
                stmBuf.push_back(newStm);
            }
            else if (e.num == 1) {
                assert(stmBuf[back]->kind == IR::Stm::Kind::SEQ);
                assert(parseStack[parseStack.size() - 4]->kind == Token::Kind::COND);
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
        else if (e.action == parsingTableEntry::Action::ACCEPT) {
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
            return reductionInfo(Token::Kind::STM, 5);
        case 2:
            return reductionInfo(Token::Kind::STM, 9);
        case 3:
            return reductionInfo(Token::Kind::STM, 5);
        case 4:
            return reductionInfo(Token::Kind::SEQ, 1);
        case 5:
            return reductionInfo(Token::Kind::SEQ, 2);
    }
    assert(0);
}

CodeParser::parsingTableEntry CodeParser::lookupParsingTable(int cntState, Token::Kind tokenKind, std::pair<int, int> pos) {
    switch (cntState) {
        case 1: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 4);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 3);
                case Token::Kind::SEQ:
                    return parsingTableEntry(parsingTableEntry::Action::GOTO, 18);
                }
                std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 2: {
            switch (tokenKind) {
                case Token::Kind::COND:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 6);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 3: {
            switch (tokenKind) {
                case Token::Kind::COND:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 14);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 4: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 4);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 3);
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 4);
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 4);
                case Token::Kind::SEQ:
                    return parsingTableEntry(parsingTableEntry::Action::GOTO, 5);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 5: {
            switch (tokenKind) {
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 5);
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 5);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 6: {
            switch (tokenKind) {
                case Token::Kind::LBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 7);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 7: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 4);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 3);
                case Token::Kind::SEQ:
                    return parsingTableEntry(parsingTableEntry::Action::GOTO, 8);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 8: {
            switch (tokenKind) {
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 9);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 9: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 1);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 1);
                case Token::Kind::ELSE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 10);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 1);
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 1);
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 1);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 10: {
            switch (tokenKind) {
                case Token::Kind::LBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 11);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 11: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 4);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 3);
                case Token::Kind::SEQ:
                    return parsingTableEntry(parsingTableEntry::Action::GOTO, 12);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 12: {
            switch (tokenKind) {
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 13);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 13: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 2);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 2);
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 2);
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 2);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 14: {
            switch (tokenKind) {
                case Token::Kind::LBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 15);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 15: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 4);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 2);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 3);
                case Token::Kind::SEQ:
                    return parsingTableEntry(parsingTableEntry::Action::GOTO, 16);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 16: {
            switch (tokenKind) {
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::SHIFT, 17);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 17: {
            switch (tokenKind) {
                case Token::Kind::STM:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 3);
                case Token::Kind::IF:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 3);
                case Token::Kind::WHILE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 3);
                case Token::Kind::RBRACE:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 3);
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::REDUCE, 3);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
        case 18: {
            switch (tokenKind) {
                case Token::Kind::END:
                    return parsingTableEntry(parsingTableEntry::Action::ACCEPT);
            }
            std::cout << "Compile error near (" << pos.first << ", " << pos.second << ")." << std::endl;
                exit(-1);
        }
    }
    assert(0);
}

} // namespace FE
} // namespace FC