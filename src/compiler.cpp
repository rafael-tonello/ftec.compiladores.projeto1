#include "compiler.h"

#pragma region entrypoints and auxiliar methods
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
Result Compiler::EntryPoint2(string insertBefore)
{
    Result result;
    //first sequence
    Result rWhile = this->_while(insertBefore);
    if (rWhile.wasRecognized)
    {
        result.wasRecognized = true;

        if (rWhile.errors != "")
            result.errors = "Error in a 'while' definition:\r\n"+rWhile.errors;

    }
    else{
        //second sequence
        Result rIf = this->_if(insertBefore);
        if (rIf.wasRecognized)
        {
            result.wasRecognized = true;
            if (rIf.errors != "")
                result.errors = "Error in a 'if' definition:\r\n"+rIf.errors;
        }
        else {
            //third sequence
            Result rBlockOfCode = this->blockOfCode(insertBefore);
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
                            Result rAttribDef = this->attribDef(insertBefore);
                            if (rAttribDef.wasRecognized)
                            {
                                if (rAttribDef.errors == "")
                                {
                                    //insert the attribuition to current toke to intermediate code
                                    this->insertIntermediateCode(insertBefore, "ATTRIB "+ token +" "+rAttribDef.result, -1);

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

void Compiler::insertIntermediateCode(string label, string code, int offset)
{
    if (label == "")
        this->intermediateCode.push_back(code);
    else
    {
        //find the label in the code
        int index = this->intermediateCode.size();
        auto nPos = this->intermediateCode.find(":"+label);
        if (nPos != this->intermediateCode.end())
            index = nPos + offset;

        if (index >= this->intermediateCode.size())
            index = this->intermediateCode.size() -1;
        
        if (index < 0)
            index = 0;

        this->intermediateCode.insert(this->intermediateCode.begin() + index, code);

    }

}

string Compiler::getNextTempName()
{
    return string("ID_"+std::to_string(this->globalCount++));
}

#pragma endregion

#pragma region attribuition and matematical operations

//Grammar: <attribDef2> | <attribDef2> + <mathLevel1> + <TokenNameOrData>
Result Compiler::attribDef(string insertBefore)
{
    Result result;
    Result attribDef2Result = this->attribDef2(insertBefore);
    if (attribDef2Result.wasRecognized)
    {
        result.wasRecognized = true;
        if (attribDef2Result.errors == "")
        {
            //checks by mathLevel1
            Result mathLevel1Result = this->mathLevel1();
            if (mathLevel1Result.wasRecognized)
            {
                Result tokenNameOrDataResult = this->tokenNameOrData(insertBefore);
                if (tokenNameOrDataResult.wasRecognized)
                {
                    if (tokenNameOrDataResult.errors == "")
                    {
                        string resultVariable = this->getNextTempName();
                        //define matematical operator name
                        string mathOperator = mathLevel1Result.result;
                        if (mathOperator == "+")
                            mathOperator = "SUM";
                        else if (mathOperator == "-")
                            mathOperator = "SUB";

                        this->insertIntermediateCode(insertBefore, "MATH_"+mathOperator + " " + attribDef2Result.result + " " + tokenNameOrDataResult.result + " " + resultVariable, -1);
                        result.result = resultVariable;

                    }
                    else
                        result.errors = "Error in second operand of an attribuition with a matematical operation:\r\n"+tokenNameOrDataResult.errors;
                }
                else
                {
                    result.errors = "second operand of an attribuition with a matematical operation was not recognized";
                }
            }
            else
            {
                //a simple attribuition
                result.result = attribDef2Result.result;
            }
        }
        else
        {
            result.errors = "attribDef error:\r\n"+attribDef2Result.errors;
        }

    }
    else
    {
        result.wasRecognized = false;
    }
}

//Grammar: <attribDef2> | <attribDef2> + <mathLevel1> + <TokenNameOrData>
Result Compiler::attribDef2(string insertBefore)
{
    Result result;
    Result attribDef3Result = this->attribDef3(insertBefore);
    if (attribDef3Result.wasRecognized)
    {
        result.wasRecognized = true;
        if (attribDef3Result.errors == "")
        {
            //checks by mathLevel1
            Result mathLevel2Result = this->mathLevel2();
            if (mathLevel2Result.wasRecognized)
            {
                Result tokenNameOrDataResult = this->tokenNameOrData(insertBefore);
                if (tokenNameOrDataResult.wasRecognized)
                {
                    if (tokenNameOrDataResult.errors == "")
                    {
                        string resultVariable = this->getNextTempName();
                        //define matematical operator name
                        string mathOperator = mathLevel2Result.result;
                        if (mathOperator == "*")
                            mathOperator = "MUL";
                        else if (mathOperator == "/")
                            mathOperator = "DIV";

                        this->insertIntermediateCode(insertBefore, "MATH_"+mathOperator + " " + attribDef3Result.result + " " + tokenNameOrDataResult.result + " " + resultVariable, -1);
                        result.result = resultVariable;

                    }
                    else
                        result.errors = "Error in second operand of an attribuition with a matematical operation:\r\n"+tokenNameOrDataResult.errors;
                }
                else
                {
                    result.errors = "second operand of an attribuition with a matematical operation was not recognized";
                }
            }
            else
            {
                //a simple attribuition
                result.result = attribDef3Result.result;
            }
        }
        else
        {
            result.errors = "attribDef2 error:\r\n"+attribDef3Result.errors;
        }

    }
    else
    {
        result.wasRecognized = false;
    }
}

//Grammar: <attribDef3>-> <TokenNameOrData> | <attribDef3> + <mathLevel3> + <attribDef2>
Result Compiler::attribDef3(string insertBefore)
{
    Result result;
    Result tokenNameOrDataResult = this->tokenNameOrData(insertBefore);
    if (tokenNameOrDataResult.wasRecognized)
    {
        // a simple declaration
        result.wasRecognized = true;
        result.result = tokenNameOrDataResult.result;
    }
    else
    {
        Result attribDef3Result = this->attribDef3(insertBefore);
        if (attribDef3Result.wasRecognized)
        {
            result.wasRecognized = true;
            if (attribDef3Result.errors == "")
            {
                //checks by mathLevel1
                Result mathLevel2Result = this->mathLevel3();
                if (mathLevel2Result.wasRecognized)
                {
                    Result tokenNameOrDataResult = this->tokenNameOrData(insertBefore);
                    if (tokenNameOrDataResult.wasRecognized)
                    {
                        if (tokenNameOrDataResult.errors == "")
                        {
                            string resultVariable = this->getNextTempName();
                            //define matematical operator name
                            string mathOperator = mathLevel2Result.result;
                            if (mathOperator == "^")
                                mathOperator = "POW";

                            this->insertIntermediateCode(insertBefore, "MATH_"+mathOperator + " " + attribDef3Result.result + " " + tokenNameOrDataResult.result + " " + resultVariable, -1);
                            result.result = resultVariable;

                        }
                        else
                            result.errors = "Error in second operand of an attribuition with a matematical operation:\r\n"+tokenNameOrDataResult.errors;
                    }
                    else
                    {
                        result.errors = "second operand of an attribuition with a matematical operation was not recognized";
                    }
                }
                else
                {
                    //a simple attribuition
                    result.result = attribDef3Result.result;
                }
            }
            else
            {
                result.errors = "attribDef3 error:\r\n"+attribDef3Result.errors;
            }

        }
        else
        {
            result.wasRecognized = false;
        }
    }
}

//Grammar: <mathLevel1> -> + | - 
Result Compiler::mathLevel1(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken == "+" || nextToken == "-")
    {
        result.wasRecognized = true;
        result.result = nextToken;
    }
    else
    {
        result.wasRecognized = false;
    }

    return result;
}

//Grammar: <mathLevel1> -> * | /
Result Compiler::mathLevel2(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken == "*" || nextToken == "/")
    {
        result.wasRecognized = true;
        result.result = nextToken;
    }
    else
    {
        result.wasRecognized = false;
    }

    return result;
}

//Grammar: <mathLevel1> -> ^
Result Compiler::mathLevel2(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken == "^")
    {
        result.wasRecognized = true;
        result.result = nextToken;
    }
    else
    {
        result.wasRecognized = false;
    }

    return result;
}

//Grammar: <TokenNameOrData> -> TokenName | number | string
Result Compiler::tokenNameOrData(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken.size() > 0)
    {
        result.wasRecognized = true;
        if (nextToken[0] == '"' || nextToken[0] == '\'' || this->isNumber(nextToken))
        {
            //numbers and strings
            result.result = nextToken
        }
        else
        {
            //looks for variable definition in the intermediate code
            bool found = false;
            for (auto &c: this->intermediateCode)
            {

                if (c.find("DEFINE") == 0 && c.find(nextToken) == c.size() - nextToken.size())
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                result.result = nextToken;
            }
            else
            {
                result.errors = "Unknwon token '"+nextToken+"'";
            }

        }
    }
    else
    {
        result.wasRecognized = false;
        this->putBackToken(nextToken);
    }
}

bool Compiler::isNumber(string possibleNumber)
{
    bool dot = false;
    string  numbers = "0123456789";
    
    for (unsigned c = 0; c < possibleNumber.size(); c++)
    {
        if (numbers.find(possibleNumber[c]) == string::npos)
        {
            if (c == 0 && possibleNumber[c] != '-' && possibleNumber[c] != '+')
                return false;
            else if (possibleNumber[c] == '.')
            {
                if (dot)
                    return false;
                else
                    dot = true;
            }
        }
    }

    return true;
}
#pragma endregion

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

#pragma region blocks of code (while, if, logic and unamed block)
//Grammar: <while>-> "while" + <blockOfLogic> + <blockOfCode>
Result Compiler::_while(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    //checks if is a valid block
    if (nextToken == "while")
    {
        result.wasRecognized = true;

        //insert the retry point of while
        string returnLabelName = this->getNextTempName();
        this->insertIntermediateCode(insertBefore, ":"+returnLabelName, -1);

        Result rBlockOfLogic = this->blockOfLogic(insertBefore);
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
                Result rBlockOfCode = this->blockOfCode(falseLabel);
                if (rBlockOfCode.wasRecognized)
                {
                    if (rBlockOfCode.errors == "")
                    {
                        //add the while loopback to intermediate code (the code must be after the 'exitLabel')
                        //this->insertIntermediateCode(falseLabel, "GOTO "+returnLabelName, -1);
                        this->insertIntermediateCode(exitLabel, "GOTO "+returnLabelName, 1);

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
Result Compiler::_if(string insertBefore)
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
                Result rBlockOfCode = this->blockOfCode(falseLabel);
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
Result Compiler::blockOfCode(string insertBefore)
{
    Result result;
    string openKeysToken = this->getNextToken();
    if (openKeysToken == "{")
    {
        result.wasRecognized = true;
        Result e2Result = EntryPoint2(insertBefore);
        if (e2Result.wasRecognized)
        {
            if (e2Result.errors != "")
            {
                result.errors = "Error in block of code:\r\n"+e2Result.errors;
            }
        }
        else{
            result.errors = "Error in block of code: code was not recognized";
        }
    }
    else
    {
        result.wasRecognized = false;
        this->putBackToken(openKeysToken);
    }

    return result;

}

//<blockOfLogic> -> "(" + <parentesisD> + ")"
Result Compiler::blockOfLogic(string insertBefore)
{
    Result result;
    string openParentesisToken = this->getNextToken();
    if (openParentesisToken == "(")
    {
        result.wasRecognized = true;
        Result parantesisDResult = this->parentesisD(insertBefore);
        if (parantesisDResult.wasRecognized)
        {
            if (parantesisDResult.errors != "")
            {
                result.errors = "Error in block of logic:\r\n"+parantesisDResult.errors;
            }
        }
        else
        {
            result.errors = "Error in block of logic: parentesis structure was not reconized";
        }
    }
    else
    {
        result.wasRecognized = false;
        this->putBackToken(openParentesisToken);
    }

    return result;
}


//Grammaer: <parentesisD> -> <blockOfLogic> | <logicTokenNameOrData> + <LogicOperator> + <logicTokenNameOrData> 
Result Compiler::parentesisD(string insertBefore)
{
    Result result;
    result.wasRecognized = false;
    //first sequence
    Result parentesisDResult = this->parentesisD(insertBefore);
    if (parentesisDResult.wasRecognized)
    {
        result.wasRecognized = true;
        if (parentesisDResult.errors == "")
        {
            Result logicBlockResult = this->blockOfLogic(insertBefore);
            if (logicBlockResult.wasRecognized)
            {
                if (logicBlockResult.errors == "")
                {
                    //just return
                }
                else
                {
                    result.errors = "Invalid block of logic:\r\n"+logicBlockResult.errors;
                }
            }
            else
            {
                result.errors = "Unrecognized block of logic";
            }

        }
        else
        {
            result.errors = "Error in nesting of parentesis:\r\n"+parentesisDResult.errors;
        }
        
    }
    else
    {
        Result firstLogicOperator = this->logicTokenNameOrData(insertBefore);
        if (firstLogicOperator.wasRecognized)
        {
            result.wasRecognized = true;
            if (firstLogicOperator.errors == "")
            {
                //get the logic operator
                Result logicOperator = this->logicOperator(insertBefore);
                if (logicOperator.wasRecognized)
                {

                    //get the second operator
                    Result secondOperator = this->logicTokenNameOrData(insertBefore);
                    if (secondOperator.wasRecognized)
                    {

                        if (secondOperator.errors != "")
                        {
                            //add the operation to the intermediate code
                            //define the logic operation name
                            string operation = "NOP";
                            if (logicOperator.result == "==")
                                operation = "CHECK_EQUALS";
                            else if (logicOperator.result == ">")
                                operation = "CHECK_BIGGERTHAN";
                            else if (logicOperator.result == ">=")
                                operation = "CHECK_BIGGEOREQUALS";
                            else if (logicOperator.result == "<")
                                operation = "CHECK_LESSTHAN";
                            else if (logicOperator.result == "<=")
                                operation = "CHECK_LESSOREQUALS";
                            else if (logicOperator.result == "!=")
                                operation = "CHECK_DIFFERENT";

                            string trueLabelName = this->getNextTempName();
                            string falseLabelName = this->getNextTempName();
                            string exitLabelName = this->getNextTempName();

                            this->insertIntermediateCode(insertBefore, operation + " " + firstLogicOperator.result + " " +secondOperator.result + " " + trueLabelName, -1);
                            this->insertIntermediateCode(insertBefore, "JUMP "+falseLabelName, -1);
                            this->insertIntermediateCode(insertBefore, ": "+trueLabelName, -1);
                            this->insertIntermediateCode(insertBefore, "JUMP "+exitLabelName, -1);
                            this->insertIntermediateCode(insertBefore, ": "+falseLabelName, -1);
                            this->insertIntermediateCode(insertBefore, ": "+exitLabelName, -1);

                            result.result = trueLabelName + "," + falseLabelName+","+exitLabelName;
                        }
                        else
                        {
                            result.errors = "Error in the second logic operand of a block of logic:\r\n"+secondOperator.errors;
                        }
                    }
                    else
                    {
                        result.errors = "The second logic operand of a block of logic was not recognized";
                    }

                }
                else
                {
                    result.errors = "logic comparer of a block of logic was not recognized";

                }
            }
            else
            {
                result.errors = "Error in the first logic operand of a block of logic:\r\n"+firstLogicOperator.errors;

            }
        }
        else
        {
            //not recognized sequence, just return
        }

    }

    return result;
}

//Grammar:<logicTokenNameOrData> -> attribDef | blockOfLogic
Result Compiler::logicTokenNameOrData(string insertBefore)
{
    Result result;
    Result attribDefResult = this->attribDef(insertBefore);
    if (attribDefResult.wasRecognized)
    {
        result.wasRecognized = true;
        if (attribDefResult.errors == "")
        {
            result.result = attribDefResult.result;
        }
        else
        {
            result.errors = "Error getting logic, token name or const data:\r\n"+attribDefResult.errors;
        }
    }
    else
    {
        Result blockOfLogicResult = this->blockOfLogic(insertBefore);
        if (blockOfLogicResult.wasRecognized)
        {
            result.wasRecognized = true;
            if (blockOfLogicResult.errors == "")
            {
                result.result = blockOfLogicResult.result;
            }
            else
            {
                result.errors = "Error getting logic, token name or const data:\r\n"+blockOfLogicResult.errors;
            }
        }
        else
        {
            result.wasRecognized = false;
        }
    }

    return result;
}

//Grammar: <logicOperator> -> > | < | >= | <= | == | !=
Result Compiler::logicOperator(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken == ">" || nextToken == "<" || nextToken == ">=" || nextToken == "<=" || nextToken == "==" || nextToken == "!=")
    {
        result.wasRecognized = true;
        result.result = nextToken;
    }
    else
    {
        result.wasRecognized = false;
        this->putBackToken(nextToken);
    }
    
}

#pragma endregion
