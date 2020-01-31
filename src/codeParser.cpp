#include <iostream>
#include <cstdlib>
#include <cassert>

#include "codeParser.h"

namespace FC { namespace FE {

CodeParser::CodeParser(const std::string &path, std::shared_ptr<IR::Tree> tree) {
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
                case Token::RBRACE:
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