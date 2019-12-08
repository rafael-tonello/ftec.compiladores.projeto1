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


    if (result.wasRecognized && result.errors != "")
    {
        //is a valid var declaration, but errors was found
        cout << result.errors;

        return;
    }


    Result result2 = EntryPoint2();
    if (result2.wasRecognized && result2.errors != "")
    {
        //is a valid block of code, but errors was found
        cout << result.errors;

        return;
    }
    
}

//Grammar: <var> -> \n + "var" + <var declaration>
Result Compiler::var()
{
    Result result;

    //get the next tocken from stack
    string nextToken = this->getNextToken();

    //chekcks if next token represents a valid variable declaration block
    if (nextToken == "var")
    {  
        nextToken = this->getNextToken();
        if (nextToken == "\n")
        {
            result.wasRecognized = true;
            Result result2 = varDeclaration();

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
        result.errors = "Invalid variable block (expecting \n after 'var' token)";
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

//<var declaration> -> "\t" + <int> | "\t" + <real> | "\n" + <E2>
Result Compiler::varDeclaration()
{
    Result result;

    //get the next tocken from stack
    string nextToken = this->getNextToken();
    result.wasRecognized = true;

    if (nextToken == "\t")
    {
        //Ok, valid 'var' decalaration. Now, try to identiry the type of variables will be declared
        nextToken = this->getNextToken();
        if (nextToken == "int") 
        {
            //a int declaration
            Result result2 = this->intDeclaration();
            if (result2.wasRecognized && result2.errors != "")
                result.errors = "Error in int declaration:\r\n"+result2.errors;
        }
        else if (nextToken == "real")
        {
            //a real declration
            Result result2 = this->realDeclaration();
            if (result2.wasRecognized && result2.errors != "")
                result.errors = "Error in real declaration:\r\n"+result2.errors;
        }
        else
        {
            //invalid type
            result.errors = "Unknown type: "+nextToken;

        }

    }
    else if (nextToken == "\n"){
        //var exit condition
        result.wasRecognized = true;
        result.errors = "";
    }
    else{
        result.errors = "Invalid var declaration (expecting \t or \n)";
    }

    return result;
}

//Grammar: <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
Result Compiler::intDeclaration()
{
    Result result;
    result.wasRecognized = true;

    Result rTokenList = tokenNameList();
    if (!rTokenList.wasRecognized || rTokenList.errors == "")
    {
        auto tk = this -> getNextToken();
        if (tk == "\n")
        {
            Result rNextVarDec = this->varDeclaration();
            if (rNextVarDec.wasRecognized && rNextVarDec.errors != "")
                result.errors = "Invalid variables declaraion:\r\n"+rNextVarDec.errors;
            else
            {
                //add received names to variables list or intermiate code
                while (rNextVarDec.find(',') != string::npos)
                {
                    string nextVarName = rNextVarDec.substr(0, rNextVarDec.find(','));
                    rNextVarDec = rNextVarDec.substr(rNextVarDec.find(','));

                    //check in the intermediate code if the variable is not defined yet
                    double found = false;
                    for (auto &c: this->intermediateCode)
                    {
                        if (c.indexOf("DEFINE") == 0 && c.indexOf(nextVarName) != string::npos)
                        {
                            found = true;
                            break;
                        }
                    }

                    //if variable is already declared, abor the process (define an error and return)
                    if (found)
                    {
                        result.errors  = "Duplicate variable "+nextVarName;
                        return result;
                    }

                    this->intermediateCode.push_back("DEFINE INTEGER "+nextVarName);
                }
            }
        }
        else
        {
            result.errors "Expected a line break after token list name, but '" + tk + "' was found";
        }

    }
    else{
        result.errors = "definition of names of integer variables with problem:\r\n"+rTokenList.errors;
    }

    return result;
}

//Grammar: <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
Result Compiler::realDeclaration()
{
    Result result;
    result.wasRecognized = true;

    Result rTokenList = tokenNameList();
    if (!rTokenList.wasRecognized || rTokenList.errors == "")
    {
        auto tk = this -> getNextToken();
        if (tk == "\n")
        {
            Result rNextVarDec = this->varDeclaration();
            if (rNextVarDec.wasRecognized && rNextVarDec.errors != "")
                result.errors = "Invalid variables declaraion:\r\n"+rNextVarDec.errors;
            else
            {
                //add received names to variables list or intermiate code
                while (rNextVarDec.find(',') != string::npos)
                {
                    string nextVarName = rNextVarDec.substr(0, rNextVarDec.find(','));
                    rNextVarDec = rNextVarDec.substr(rNextVarDec.find(','));

                    //check in the intermediate code if the variable is not defined yet
                    double found = false;
                    for (auto &c: this->intermediateCode)
                    {
                        if (c.indexOf("DEFINE") == 0 && c.indexOf(nextVarName) != string::npos)
                        {
                            found = true;
                            break;
                        }
                    }

                    //if variable is already declared, abor the process (define an error and return)
                    if (found)
                    {
                        result.errors  = "Duplicate variable "+nextVarName;
                        return result;
                    }

                    this->intermediateCode.push_back("DEFINE REAL "+nextVarName);
                }
            }
        }
        else
        {
            result.errors "Expected a line break after token list name, but '" + tk + "' was found";
        }

    }
    else{
        result.errors = "definition of names of integer variables with problem:\r\n"+rTokenList.errors;
    }

    return result;
}
