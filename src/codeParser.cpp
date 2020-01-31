#include <iostream>
#include <cstdlib>

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
                // std::cout << "if" << std::endl;
                this->tokenList.push_back(new IfToken());
            }
            else if (str == Token::elsetoken) {
                // std::cout << "else" << std::endl;
                this->tokenList.push_back(new ElseToken());
            }
            else if (str == Token::whiletoken) {
                // std::cout << "while" << std::endl;
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
    }
}

}}