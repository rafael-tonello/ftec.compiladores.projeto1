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

void Compiler::putBackToken(string token)
{
    this->tokens.push(token);

}
        

/* This is the function that will receive the program (as a symbol vector) and
returns its respective intermediate code (a code that can be converted to
specific assembly for different processors) */
vector<string> Compiler::validadte(vector<string> tokens)
{
    for (auto &c: tokens)
        this->tokens.push(c);
    
}

//Grammar: <E1>-> <var> | <E2>
bool Compiler::EntryPoint()
{

    Result result = var();


    if (result.wasRecognized && result.errors == "")
    {
        //is a valid var declaration
    }
    else{
        Result result2 = EntryPoint2();
        if (result2.wasRecognized && result2.errors == "")
        {

        }
        else{
            //try identify more correspondent error
            if (result.wasRecognized)
            {
                //a var declaration result

            }
            else if (result2.wasRecognized)
            {
                
            }
            else
            {
                /* unknown code */
            }
            
            
        }
    }
}

//Grammar: <var> -> "var" + <var declaration>
Result Compiler::var()
{
    Result result;

    //get the next tocken from stack
    string nextToken = this->getNextToken();

    //chekcks if next token represents a valid variable declaration block
    if (nextToken == "var")
    {  
        result.wasRecognized = true;
        auto result2 = varDeclaration();

        //if a valid variable declarations is detectected, the variable declarations must
        //be corret. Checks if the nexts tokens represents a valid '<var declaration>' described
        //by language grammar (take a look in the README.md)
        if (result2.wasRecognized)
        {
            //returns back the same errors detect by 'varDeclaration' method. If no errors are
            //returned, the declaration is correct.
            result.errors = result2.errors;

            //if a valid block of variable declaration was found, the function "var" not need to 
            //return anything, just a Result with no erros, no 'results' properties setted and 
            //the property 'wasRecognized' setted as true.
        }
        else
        {
            result.errors = "Invalid variable declaration";
        }
    }
    else
    {
        //if the token not represents a valid variable declaration block, just abort operation and
        //put back the token that was taken from the stack
        result.wasRecognized = false;
        this->putBackToken(nextToken);
    }

    return result;
}