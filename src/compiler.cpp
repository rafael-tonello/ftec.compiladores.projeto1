#include "compiler.h"
        
/* The program will come in a symbol vector (where each symbol can be a string,
 * or a number, or a variable name, or a special word, etc.). The compiler will
 * interpret this list of symbols in order from top to bottom. This function is
 * responsible for returning these symbols in the correct order (top to bottom). */
string Compiler::getNextToken()
{
    string ret = this->tokens.front();
    this->tokens.pop();
    return ret;
}
        

/* This is the function that will receive the program (as a symbol vector) and
returns its respective intermediate code (a code that can be converted to
specific assembly for different processors) */
vector<string> Compiler::validadte(vector<string> tokens)
{
    for (auto &c: tokens)
        this->tokens.push(c);
    
}

bool Compiler::EntryPoint()
{

}