#include "validator.h"
        
string Validator::getNextToken()
{
    string ret = this->tokens.front();
    this->tokens.pop();
    return ret;
}
        
bool Validator::initialToken()
{

}

void Validator::validadte(vector<string> tokens)
{
    for (auto &c: tokens)
        this->tokens.push(c);
    
}
