#include "compiler.h"

void debug(string msg)
{
    cout << "DEBUG: " << msg << endl;   
}
        
/* The program will come in a symbol vector (where each symbol can be a string,
 * or a number, or a variable name, or a special word, etc.). The compiler will
 * interpret this list of symbols in order from top to bottom. This function is
 * responsible for returning these symbols in the correct order (top to bottom). */
string Compiler::getNextToken()
{
    string ret = this->tokens[0];
    this->tokens.erase(this->tokens.begin());
    return ret;
}

void Compiler::putBackToken(string token)
{
    this->tokens.insert(this->tokens.begin(), token);
}
        

/* This is the function that will receive the program (as a symbol vector) and
returns its respective intermediate code (a code that can be converted to
specific assembly for different processors) */
vector<string> Compiler::validate(vector<string> tokens)
{
    for (auto &c: tokens)
        this->tokens.push_back(c);

    if (this->EntryPoint())
    {
        cout << "Apers it work!" << endl;
            
    }
    else
    {
        cout << "Ohhh no! It not work!" << endl;
    }
}

//Grammar: <E1>-> <var> | <E2>
Result Compiler::EntryPoint()
{
    Result result;
    Result rVar = var();


    if (rVar.wasRecognized && result.errors != "")
    {
        result.wasRecognized = true;
        //is a valid var declaration, but errors was found
        result.errors =  "Some errors was found in var declaration: \r\n" <<  result.errors;

    }

    cout << "Parece que foi" << endl;


   /* Result result2 = EntryPoint2();
    if (result2.wasRecognized && result2.errors != "")
    {
        //is a valid block of code, but errors was found
        cout << result.errors;

        return;
    }*/

    cout << "Returned errors: "<< result.errors << endl << endl << endl;
    cout << endl << endl << " Intermediate code: " << endl;
    for (auto &c: this->intermediateCode)
        cout << "\t" << c << endl;
    cout << endl << endl;

    return result;
    
}

//grammar: <E2> -> <while> | <if> | <blockOfCode> | TokenName + "=" +<attribDef> | {exit}
Result Compiler::EntryPoint2()
{
    Result result;
    //first sequence
    Result rWhile = this->_while();
    if (rWhile.wasRecognized)
    {
        result.wasRecognized = true;

        if (rWhile.errors != "")
            result.errors = "Error in a 'while' definition:\r\n"+rWhile.errors;

    }
    else{
        //second sequence
        Result rIf = this->_if();
        if (rIf.wasRecognized)
        {
            result.wasRecognized = true;
            if (rIf.errors != "")
                result.errors = "Error in a 'if' definition:\r\n"+rIf.errors;
        }
        else {
            //third sequence
            Result rBlockOfCode = this->blockOfCode();
            if (rBlockOfCode.wasRecognized)
            {
                if (rBlockOfCode.errors != "")
                    result.errors = "Error in a 'block of code' definition:\r\n"+rBlockOfCode.errors;
            }
            else
            {
                //fourth sequence
                string token = this->getNextToken();

                if (string("\r\r\t ").find(token) == string::npos)
                {
                    //checks if exista a variable declaration with this token
                    bool found = false;
                    for (auto &c: this->intermediateCode)
                    {

                        if (c.find("DEFINE") == 0 && c.find(token) == c.size() - token.size())
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        string attribCanical = this->getNextToken();
                        if (attribCanical == "=")
                        {
                            //call attribDef method and get the variable that must be attributed to current token
                            Result rAttribDef = this->attribDef();
                            if (rAttribDef.wasRecognized)
                            {
                                if (rAttribDef.errors == "")
                                {
                                    //insert the attribuition to current toke to intermediate code
                                    this->intermediateCode.push_back("ATTRIB "+ token +" "+rAttribDef.result);

                                }
                                else
                                {
                                    result.errors = "Error defining attribuition of "+token+": \r\n" + rAttribDef.errors;
                                }
                            }
                            else
                            {
                                result.errors = "Attribuition of "+token+" was not recognized by the compiler";
                            }
                        }
                        else
                        {
                            result.errors = "Missing '=' after token name (after "+token+")";
                        }
                    }
                    else
                    {
                        result.errors = "Unknown token '"+token+"'. Didn't you forget to declare it?";
                    }
                }
            }
        }
    }
}

void Compiler::insertIntermediateCodeAfterLabel(string label, string code, int offset = 0)
{

}

string Compiler::getNextTempName()
{

}


#pragma region variable declaration
//Grammar: <var> -> \n + "var" + <var declaration>
Result Compiler::var()
{
    cout << "DEBUG: enterred on var" << endl;
    Result result;
    

    //get the next tocken from stack
    string nextToken = this->getNextToken();

    //chekcks if next token represents a valid variable declaration block
    if (nextToken == "var")
    {  
        result.wasRecognized = true;
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
                debug ("Invalid variable declaration");
                result.errors = "Invalid variable declaration";
            }
        }
        else
        {
            debug("Invalid variable block (expecting a line break after 'var' token but '"+nextToken+"' was found)");
            result.errors = "Invalid variable block (expecting a line break after 'var' token but '"+nextToken+"' was found)";
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
            Result result2 = this->typeDeclaration("INT");
            if (result2.wasRecognized && result2.errors != "")
                result.errors = "Error in int declaration:\r\n"+result2.errors;
        }
        else if (nextToken == "real")
        {
            //a real declration
            Result result2 = this->typeDeclaration("REAL");
            if (result2.wasRecognized && result2.errors != "")
                result.errors = "Error in real declaration:\r\n"+result2.errors;
        }
        else
        {
            //invalid type
            result.errors = "Unknown type: "+nextToken;

        }

    }
    else /*/if (nextToken == "\n")*/{
        //var exit condition
        result.wasRecognized = false;
        result.errors = "";
    /*}
    else{
        debug(
            "Invalid var declaration (expecting '\\t' or '\\n'. '"+
            nextToken+
            "', which size is "+
            std::to_string(nextToken.size())+
            ", and first character is '"+
            std::to_string((int)nextToken[0])+
            "', was found)')");
        result.errors = "Invalid var declaration (expecting '\\t' or '\\n'. '"+nextToken+"' was found)";
    }*/

        this->putBackToken(nextToken);
    }

    return result;
}

//Grammar: <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
Result Compiler::typeDeclaration(string type)
{
    Result result;
    result.wasRecognized = true;

    Result rTokenList = this->getTokenNameList();
    if (!rTokenList.wasRecognized || rTokenList.errors == "")
    {
        auto tk = this -> getNextToken();
        if (tk == "\n")
        {
            
            
            debug ("Received the var list: "+rTokenList.result);
            rTokenList.result = rTokenList.result + ",";
            //add received names to variables list or intermiate code
            while (rTokenList.result.find(',') != string::npos)
            {
                string nextVarName = rTokenList.result.substr(0, rTokenList.result.find(','));
                rTokenList.result = rTokenList.result.substr(rTokenList.result.find(',')+1);

                //check in the intermediate code if the variable is not defined yet
                bool found = false;
                debug("Parsing the varname " + nextVarName);
                for (auto &c: this->intermediateCode)
                {

                    if (c.find("DEFINE") == 0 && c.find(nextVarName) == c.size() - nextVarName.size())
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

                debug ("Insert define of "+nextVarName+":"+type+" in the intermediate code");

                this->intermediateCode.push_back("DEFINE "+type + " "+nextVarName);
            }
            Result rNextVarDec = this->varDeclaration();
            if (rNextVarDec.wasRecognized && rNextVarDec.errors != "")
                result.errors = "Invalid variables declaraion:\r\n"+rNextVarDec.errors;
        }
        else
        {
            result.errors = "Expected a line break after token names list, but '" + tk + "' was found";
        }

    }
    else
    {
        result.errors = "definition of names of integer variables with problem:\r\n"+rTokenList.errors;
    }

    return result;
}

//Grammar' <TokenNamesList> -> TokenName + "\n" | Tokename + "," + <TokenNamesList>
Result Compiler::getTokenNameList()
{
    Result result;
    result.wasRecognized = true;
    string  nextName = this->getNextToken();
    //validate the token name"

    result.result = nextName;
    //check if token list is done
    string nextToken = this->getNextToken();

    if (nextToken == ",")
    {
        Result tmp = this->getTokenNameList();
        if (tmp.wasRecognized)
        {
            if (tmp.errors == "")
            {
                result.result += "," + tmp.result;
            }
            else
            {
                result.errors = "Error defining a list of names:\r\n" + tmp.errors;
            }
        }
    }
    else if(nextToken == "\n")
    {
        //put back the "\n" in the token list (typeDeclaraion need them)
        this->putBackToken("\n");
    }
    else
    {
        //unespected token after a token name
        result.errors = "Unexpected token '"+nextToken+"' after token name '"+nextName+"'";
    }

    return result;

}
#pragma endregion

//Grammar: <while>-> "while" + <blockOfLogic> + <blockOfCode>
Result Compiler::_while()
{
    Result result;
    string nextToken = this->getNextToken();
    //checks if is a valid block
    if (nextToken == "while")
    {
        result.wasRecognized = true;

        //insert the retry point of while
        string returnLabelName = this->getNextTempName();
        intermediateCode.push_back(":"+returnLabelName);

        Result rBlockOfLogic = this->blockOfLogic();
        //validade the block of logic
        if (rBlockOfLogic.wasRecognized)
        {
            if (rBlockOfLogic.errors == "")
            {
                //the block of lógic returns a <true label>,<false label>,<exit label>
                //get the names of true and false label
                string trueLabel = rBlockOfLogic.result.substr(rBlockOfLogic.begin(), rBlockOfLogic.result.find(','));
                rBlockOfLogic.result = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);
                string falseLabel = rBlockOfLogic.result.substr(rBlockOfLogic.begin(), rBlockOfLogic.result.find(','));
                string exitLabel = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);


                //validade block of code
                Result rBlockOfCode = this->blockOfCode();
                if (rBlockOfCode.wasRecognized)
                {
                    if (rBlockOfCode.errors == "")
                    {
                        //add the while loopback to intermediate code (the code must be after the 'exitLabel')
                        //this->insertIntermediateCodeAfterLabel(falseLabel, "GOTO "+returnLabelName, -1);
                        this->insertIntermediateCodeAfterLabel(exitLabel, "GOTO "+returnLabelName);

                    }
                    else
                        result.errors = "Error in 'block of code' of a 'while' structure:\r\n" + rBlockOfLogic.errors;
                }
                else
                    result.errors = "Missing or invalid 'block of code' of a 'while' structure";
            }
            else
            {
                result.errors = "Error in 'logic' of a 'while' structure:\r\n" + rBlockOfLogic.errors;
            }
        }
        else
            result.errors = "Missing or invalid 'logic' of a 'while' structure";

    }
    else{
        result.wasRecognized = false;
        this->putBackToken(nextToken);
    }

    return result;
}

//Grammar: <if>-> <blockOfLogic> + <blockOfCode>
Result Compiler::_if()
{
    Result result;
    string nextToken = this->getNextToken();
    //checks if is a valid block
    if (nextToken == "if")
    {
        result.wasRecognized = true;

        Result rBlockOfLogic = this->blockOfLogic();
        //validade the block of logic
        if (rBlockOfLogic.wasRecognized)
        {
            if (rBlockOfLogic.errors == "")
            {
                //the block of lógic returns a <true label>,<false label>,<exit label>
                //get the names of true and false label
                string trueLabel = rBlockOfLogic.result.substr(rBlockOfLogic.begin(), rBlockOfLogic.result.find(','));
                rBlockOfLogic.result = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);
                string falseLabel = rBlockOfLogic.result.substr(rBlockOfLogic.begin(), rBlockOfLogic.result.find(','));
                string exitLabel = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);


                //validade block of code
                Result rBlockOfCode = this->blockOfCode();
                if (rBlockOfCode.wasRecognized)
                {
                    if (rBlockOfCode.errors == "")
                    {
                        //just return
                    }
                    else
                        result.errors = "Error in 'block of code' of a 'if' structure:\r\n" + rBlockOfLogic.errors;
                }
                else
                    result.errors = "Missing or invalid 'block of code' of a 'if' structure";
            }
            else
            {
                result.errors = "Error in 'logic' of a 'if' structure:\r\n" + rBlockOfLogic.errors;
            }
        }
        else
            result.errors = "Missing or invalid 'logic' of a 'if' structure";

    }
    else{
        result.wasRecognized = false;
        this->putBackToken(nextToken);
    }

    return result;

}

//Grammar: <blockOfCode> -> "{" +<E2> + "}"
Result Compiler::blockOfCode()
{

}

Result Compiler::blockOfLogic()
{

}
