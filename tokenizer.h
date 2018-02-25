//
// Created by Nina Lei on 2/19/18.
//

#ifndef OSLAB2_TOKENIZER_H
#define OSLAB2_TOKENIZER_H

#endif //OSLAB2_TOKENIZER_H

#include <iostream>
#include <string.h>
using namespace std;


class tokenizer {
public:
    struct Token {
        string tokenName;
        size_t line, offset;
        Token(string tokenName, size_t line, size_t offset)
                :tokenName(tokenName), line(line), offset(offset) {}
    };

    int getToken();
    bool nextToken();
    tokenizer(string file);
    void close();

protected:
    ifstream file;
    size_t lineNum, offset;
    string line;
};