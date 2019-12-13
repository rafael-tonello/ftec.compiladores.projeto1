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
string Compiler::getNextToken(bool skeepBlanks)
{
    
    if (this->tokens.size() > 0)
    {
        string ret = this->tokens[0];
        this->tokens.erase(this->tokens.begin());
        while (skeepBlanks && (ret == "\n" || ret == "\t" || ret == " "))
        {
            ret = this->tokens[0];
            this->tokens.erase(this->tokens.begin());
        }

        
        return ret;

    }
    else
        return END;
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
    return this->intermediateCode;
}

//Grammar: <E1>-> <var> | <E2>
bool Compiler::EntryPoint()
{

    bool result = true;
    Result rVar = var();



    if (rVar.wasRecognized && rVar.errors != "")
    {
        //is a valid var declaration, but errors was found
        cout <<  "Some errors was found in var declaration: \r\n" + rVar.errors;

        return false;

    }



    Result result2 = LoopEntryPoint2("");

    cout << "Intermediate code contains " << to_string(this->intermediateCode.size()) << " lines" << endl;
    cout  << " Intermediate code: " << endl;
    for (auto &c: this->intermediateCode)
        cout << "\t" << c << endl;
    cout << endl << endl;

    return result;
    
}

Result Compiler::LoopEntryPoint2(string insertBefore)
{
    
    Result result;
    while (true)
    {
        Result result2 = this->EntryPoint2(insertBefore);
        if (result2.wasRecognized)
        {
            result.wasRecognized = true;
            if (result2.errors != "")
            {
                //is a valid block of code, but errors was found
                return result2;
            }
        }
        else
        {
            return result;;
        }
    }
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
                result.wasRecognized = true;
                if (rBlockOfCode.errors != "")
                    result.errors = "Error in a 'block of code' definition:\r\n"+rBlockOfCode.errors;
            }
            else
            {
                //fourth sequence
                string token = this->getNextToken();

                //%%%%%%%%%%%%%%%%%%%%%%%check if is a valid token name
                if (string("\r\r\t{}()= ").find(token) == string::npos)
                {
                    result.wasRecognized = true;
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
                        string attribCanonical = this->getNextToken();
                        if (attribCanonical == "=")
                        {
                            //call attribDef method and get the variable that must be attributed to current token
                            Result rAttribDef = this->attribDef(insertBefore);
                            if (rAttribDef.wasRecognized)
                            {
                                if (rAttribDef.errors == "")
                                {
                                    //insert the attribuition to current toke to intermediate code
                                    this->insertIntermediateCode(insertBefore, "ATTRIB "+ token +" "+rAttribDef.result, 00);
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
                else
                {
                    this->putBackToken(token);
                }
                
            }
        }
    }

    return result;
}

void Compiler::insertIntermediateCode(string label, string code, int offset)
{
    if (label == "")
        this->intermediateCode.push_back(code);
    else
    {
        //find the label in the code
        int index = this->intermediateCode.size();
        
        auto nPos = this->findInIntermediateCode(":"+label);
        if (nPos != notFound)
            index = nPos + offset;

        if (index >= this->intermediateCode.size())
            this->intermediateCode.push_back(code);        
        else if (index <= 0)
            this->intermediateCode.insert(this->intermediateCode.begin(), code);
        else
            this->intermediateCode.insert(this->intermediateCode.begin() + index, code);

    }

}

string Compiler::getNextTempName()
{
    return string("ID_"+std::to_string(this->globalCount++));
}

int Compiler::findInIntermediateCode(string search)
{

    for (int c = 0; c < this->intermediateCode.size(); c++)
        if (this->intermediateCode[c] == search)
            return c;
    
    return notFound;
}

#pragma endregion

#pragma region attribuition and matematical operations

//grammar-> <attribDef> -> string | <m1>
Result Compiler::attribDef(string insertBefore)
{
    Result result;
    //checks if nextToken is a string
    string nextToken = this->getNextToken();
    if (nextToken.find("\"") != string::npos)
    {
        //return  the string
        result.wasRecognized = true;
        result.result = nextToken;
    }
    else
    {
        this->putBackToken(nextToken);

        Result m1Result = this->m1(insertBefore);
        if (m1Result.wasRecognized)
        {
            result.wasRecognized = true;
            result.result = m1Result.result;
            result.errors = m1Result.errors;
        }
        else
            result.wasRecognized = false;
    }

    return result;
}

/* Grammar: <m1> -> <m2> + ('+' | '-') + <m2> + '^' <m1> | 
                    <m2> + ('+' | '-') <m2> + ('+'|'-')> <m1> |
                    <m2> + ('+'|'-') <m2> |
                    <m2>
*/
Result Compiler::m1(string insertBefore)
{
    Result result;
    Result firstM2Result = this->m2(insertBefore);
    if (firstM2Result.wasRecognized)
    {
        result.wasRecognized = true;
        string nextToken = this->getNextToken();
        if (nextToken == "+" || nextToken == "-")
        {
            string op = nextToken == "+"? "MATH_SUM": "MATH_SUB";
            Result secondM2Result = this->m2(insertBefore);
            if (secondM2Result.wasRecognized)
            {   
                string tempDataResult = this->getNextTempName();
                string nextToken = this->getNextToken();
                if (nextToken == "^")
                {
                    Result thirdResult = this->m1(insertBefore);
                    if (thirdResult.wasRecognized){
                        this->insertIntermediateCode(insertBefore, "MATH_POW "+secondM2Result.result+ " " + thirdResult.result + " " +tempDataResult + " ", 00);
                        this->insertIntermediateCode(insertBefore, op + " "+firstM2Result.result+ " " + tempDataResult + " " +tempDataResult + " ", 00);
                    }
                    else
                    {
                        //the third operand was not recognized
                        result.errors = "A matematical operand was not recognized";
                    }
                    

                }
                else if (nextToken == "+" || nextToken == "-")
                {
                    Result thirdResult = this->m1(insertBefore);
                    if (thirdResult.wasRecognized)
                    {
                        string op2 = nextToken == "+"? "MATH_SUM": "MATH_SUB";
                        this->insertIntermediateCode(insertBefore, op2 + " "+secondM2Result.result+ " " + thirdResult.result + " " +tempDataResult + " ", 00);
                        this->insertIntermediateCode(insertBefore, op + " "+firstM2Result.result+ " " + tempDataResult + " " +tempDataResult + " ", 00);
                    }
                    else
                    {
                        //the third operand was not recognized
                        result.errors = "A matematical operand was not recognized";
                    }
                }
                else
                {   
                    this->putBackToken(nextToken);
                    this->insertIntermediateCode(insertBefore, op + " "+firstM2Result.result+ " " + secondM2Result.result + " " +tempDataResult + " ", 00);
                }
                result.result = tempDataResult;
                
            }
            else
            {
                //the second operand was not recognized
                result.errors = "A matematical operand was not recognized";
            }
        }
        else
        {
            this->putBackToken(nextToken);
            result.errors = firstM2Result.errors;
            result.result = firstM2Result.result;
        }
    }
    else
    {
        result.wasRecognized = false;
        result.errors = firstM2Result.errors;
    }

    return result;
}


/* Grammar: <m2> -> <m3> + ('*'|'/') <m3> + ('*'|'/') <m1> |
                    <m3> + ('*'|'/') <m3> |
                    <m3> */
Result Compiler::m2(string insertBefore)
{
    Result result;
    Result firstResult = this->m3(insertBefore);
    if (firstResult.wasRecognized)
    {
        result.wasRecognized = true;
        string nextToken = this->getNextToken();
        if (nextToken == "*" || nextToken == "/")
        {
            string op = nextToken == "*"? "MATH_MUL": "MATH_DIV";
            Result secondResult = this->m3(insertBefore);
            if (secondResult.wasRecognized)
            {   
                string tempDataResult = this->getNextTempName();
                string nextToken = this->getNextToken();
                if (nextToken == "*" || nextToken == "/")
                {
                    Result thirdResult = this->m1(insertBefore);
                    if (thirdResult.wasRecognized)
                    {
                        string op2 = nextToken == "*"? "MATH_MUL": "MATH_DIV";
                        this->insertIntermediateCode(insertBefore, op2 + " "+secondResult.result+ " " + thirdResult.result + " " +tempDataResult + " ", 00);
                        this->insertIntermediateCode(insertBefore, op + " "+firstResult.result+ " " + tempDataResult + " " +tempDataResult + " ", 00);
                    }
                    else
                    {
                        //the third operand was not recognized
                        result.errors = "A matematical operand was not recognized";
                    }
                }
                else
                {   
                    this->putBackToken(nextToken);
                    this->insertIntermediateCode(insertBefore, op + " "+firstResult.result+ " " + secondResult.result + " " +tempDataResult + " ", 00);
                }
                result.result = tempDataResult;
                
            }
            else
            {
                //the second operand was not recognized
                result.errors = "A matematical operand was not recognized";
            }
        }
        else
        {
            this->putBackToken(nextToken);
            result.errors = firstResult.errors;
            result.result = firstResult.result; 
        }
    }
    else
    {
        result.wasRecognized = false;
        result.errors = firstResult.errors;
    }

    return result;
}


/* Grammar: <m3> -> <m4> + '^' + <m4> |
                    <m4> + '^' + <m4> +'^' + <m1> |
                    <m4>*/

Result Compiler::m3(string insertBefore)
{
    Result result;
    Result firstResult = this->m4(insertBefore);
    if (firstResult.wasRecognized)
    {
        result.wasRecognized = true;
        string nextToken = this->getNextToken();
        if (nextToken == "^")
        {
            string op = "MATH_POW";
            Result secondResult = this->m4(insertBefore);
            if (secondResult.wasRecognized)
            {   
                string tempDataResult = this->getNextTempName();
                string nextToken = this->getNextToken();
                if (nextToken == "^")
                {
                    Result thirdResult = this->m1(insertBefore);
                    if (thirdResult.wasRecognized)
                    {
                        string op2 = "MATH_POW";
                        this->insertIntermediateCode(insertBefore, op2 + " "+secondResult.result+ " " + thirdResult.result + " " +tempDataResult + " ", 00);
                        this->insertIntermediateCode(insertBefore, op + " "+firstResult.result+ " " + tempDataResult + " " +tempDataResult + " ", 00);
                    }
                    else
                    {
                        //the third operand was not recognized
                        result.errors = "A matematical operand was not recognized";
                    }
                }
                else
                {   
                    this->putBackToken(nextToken);
                    this->insertIntermediateCode(insertBefore, op + " "+firstResult.result+ " " + secondResult.result + " " +tempDataResult + " ", 00);
                }
                result.result = tempDataResult;
                
            }
            else
            {
                //the second operand was not recognized
                result.errors = "A matematical operand was not recognized";
            }
        }
        else
        {
            this->putBackToken(nextToken);
            result.errors = firstResult.errors;
            result.result = firstResult.result;
        }
    }
    else
    {
        result.wasRecognized = false;
        result.errors = firstResult.errors;
    }

    return result;
}

//Grammar: <m4> -> "(" + <m1> +")" | <TokenNameOrData>
Result Compiler::m4(string insertBefore)
{
    Result result;
    string nextToken = this->getNextToken();
    if (nextToken == "(")
    {
        result.wasRecognized = true;
        Result resultM1 = this->m1(insertBefore);
        if (resultM1.wasRecognized)
        {
            string canonicalCloseParentesis = this->getNextToken();
            if (canonicalCloseParentesis == ")")
            {
                result.result = resultM1.result;
            }
            else
            {
                result.errors = "Extepected ')' but '"+canonicalCloseParentesis+"' found.";
            }
        }
        else
        {
            result.errors = "Matematical expression or token name or data was not recognized between parentesis";
        }
    }
    else
    {
        this->putBackToken(nextToken);
        result = this->tokenNameOrData(insertBefore);
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
            result.result = nextToken;
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

    return result;
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

    Result result;
    

    //get the next tocken from stack
    string nextToken = this->getNextToken();

    //chekcks if next token represents a valid variable declaration block
    if (nextToken == "var")
    {  
        
        result.wasRecognized = true;
        nextToken = this->getNextToken(false);
        
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
    string nextToken = this->getNextToken(false);
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
    else /*/if (nextToken == "\n")*/
    {
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
        auto tk = this -> getNextToken(false);
        if (tk == "\n")
        {
            rTokenList.result = rTokenList.result + ",";
            //add received names to variables list or intermiate code
            while (rTokenList.result.find(',') != string::npos)
            {
                string nextVarName = rTokenList.result.substr(0, rTokenList.result.find(','));
                rTokenList.result = rTokenList.result.substr(rTokenList.result.find(',')+1);

                //check in the intermediate code if the variable is not defined yet
                bool found = false;
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
    string nextToken = this->getNextToken(false);

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
    string nextToken = this->getNextToken(true);
    
    //checks if is a valid block
    if (nextToken == "while")
    {
        result.wasRecognized = true;

        //insert the retry point of while
        string returnLabelName = this->getNextTempName();
        this->insertIntermediateCode(insertBefore, ":"+returnLabelName, 00);

        Result rBlockOfLogic = this->blockOfLogic(insertBefore);
        //validade the block of logic
        if (rBlockOfLogic.wasRecognized)
        {
            if (rBlockOfLogic.errors == "")
            {
                //the block of lógic returns a <true label>,<false label>,<exit label>
                //get the names of true and false label
                string trueLabel = rBlockOfLogic.result.substr(0, rBlockOfLogic.result.find(','));
                rBlockOfLogic.result = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);
                string falseLabel = rBlockOfLogic.result.substr(0, rBlockOfLogic.result.find(','));
                string exitLabel = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);
                
                string whileExitLabel = this->getNextTempName();
                string whileCodeMilestone = this->getNextTempName();
                this->insertIntermediateCode(trueLabel, ":"+whileCodeMilestone, 1);
                
                /*this->insertIntermediateCode(exitLabel, ":"+whileExitLabel, 1);
                this->insertIntermediateCode(falseLabel, "GOTO "+whileExitLabel, 1);*/

                //validade block of code
                Result rBlockOfCode = this->blockOfCode(whileCodeMilestone);
                if (rBlockOfCode.wasRecognized)
                {
                    if (rBlockOfCode.errors == "")
                    {
                        //add the while loopback to intermediate code (the code must be after the 'exitLabel')
                        //this->insertIntermediateCode(falseLabel, "GOTO "+returnLabelName, 00);
                        this->insertIntermediateCode(whileCodeMilestone, "GOTO "+returnLabelName, 00);

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
        debug("will put back the token "+nextToken + ". (the current next token is "+this->tokens[0]+")");
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

        Result rBlockOfLogic = this->blockOfLogic(insertBefore);
        //validade the block of logic
        if (rBlockOfLogic.wasRecognized)
        {
            if (rBlockOfLogic.errors == "")
            {
                //the block of lógic returns a <true label>,<false label>,<exit label>
                //get the names of true and false label
                string trueLabel = rBlockOfLogic.result.substr(0, rBlockOfLogic.result.find(','));
                rBlockOfLogic.result = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);
                string falseLabel = rBlockOfLogic.result.substr(0, rBlockOfLogic.result.find(','));
                string exitLabel = rBlockOfLogic.result.substr(rBlockOfLogic.result.find(',')+1);

                string ifTrueMalestone = this->getNextTempName();
                this->insertIntermediateCode(trueLabel, ":"+ifTrueMalestone, 1);


                //validade block of code
                Result rBlockOfCode = this->blockOfCode(ifTrueMalestone);
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
    string openKeysToken = this->getNextToken(true);
    if (openKeysToken == "{")
    {
        result.wasRecognized = true;
        Result e2Result = LoopEntryPoint2(insertBefore);
        if (e2Result.wasRecognized)
        {
            if (e2Result.errors == "")
            {
            //read the '}' of block of code
                string canonicalCloseBlock = this->getNextToken();
                if (canonicalCloseBlock != "}")
                {
                    result.errors = "Error in block of code. Expected '}' but '"+canonicalCloseBlock+"' found";    
                }
            }
            else
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
        Result parentesisDResult = this->parentesisD(insertBefore);
        if (parentesisDResult.wasRecognized)
        {
            if (parentesisDResult.errors != "")
            {
                result.errors = "Error in block of logic:\r\n"+parentesisDResult.errors;
            }
            else
            {
                string rCloseKey = this->getNextToken();
                if (rCloseKey == ")")
                {
                    result.result = parentesisDResult.result;

                }
                else
                {
                    result.errors = "Expected ')' but '"+rCloseKey+"' found";
                }
                
                
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


//Grammaer: <attribDef> + <LogicOperator> + <attribDef>
Result Compiler::parentesisD(string insertBefore)
{
    Result result;
    result.wasRecognized = false;
    //first sequence
    Result firstLogicOperator = this->attribDef(insertBefore);
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
                Result secondOperator = this->attribDef(insertBefore);
                if (secondOperator.wasRecognized)
                {

                    if (secondOperator.errors == "")
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

                        this->insertIntermediateCode(insertBefore, operation + " " + firstLogicOperator.result + " " +secondOperator.result + " " + trueLabelName, 00);
                        this->insertIntermediateCode(insertBefore, "JUMP "+falseLabelName, 00);
                        this->insertIntermediateCode(insertBefore, ":"+trueLabelName, 00);
                        this->insertIntermediateCode(insertBefore, "JUMP "+exitLabelName, 00);
                        this->insertIntermediateCode(insertBefore, ":"+falseLabelName, 00);
                        this->insertIntermediateCode(insertBefore, ":"+exitLabelName, 00);

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

    return result;
    
}

#pragma endregion
