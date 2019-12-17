#include "main.h"
#include "compiler.h"
#include "tokenExtractor.h"

/*
    Planning "A"
        first day
            [ ] language specification (grammar)
            [x] basic structure
            [ ] code writing planning

    day 11, the plannin was not well executed... a new planning was make, but it was not documented (I'm afraid of Murphy spirt. The planning is just in my mind)
    
*/

using namespace std;

int main(int argc, char** argv)
{
    TokenExtractor te;
    string t2 = string(argv[1]);
    string fContent = readFile(t2);
    auto tokens2 = te.getTokens(fContent);

    Compiler *cmp = new Compiler();
    auto intermediateCode = cmp->validate(tokens2);
    auto c = intermediateCode.size();

   /* vector<string> tokens = {
        "var", "\n",
            "\t", "int", "cont", ",", "num", "\n",
            "\t", "real", "cont2", "\n",
            
        "num", "=", "5", "+", "2" , "/" , "3", "+", "10", "^", "3", "*", "2", "+", "(", "2", "+", "5", ")", "^", "num", "-", "cont", "*", "cont2", "\n",

        "while", "(", "cont", "<", "10", ")", "{", "\n",
            "if", "(", "num", "==", "3",")", "{",
                "num", "=", "5",
            "}",
            
            "if",  "(", "(", "15", "+", "5", "-", "25", "*", "18", "^", "2", ")", "<=",  "4", "*", "5", "+", "6", ")", 
            "{", 
                "num", "=", "10", "*", "2",
            "}", 
        "}"
        
    };

    Compiler *cmp2 = new Compiler();
    cmp2->validate(tokens);*/

    cout << "Hello, can you hear me? Listen well: If you give this man a ride, sweet family will die!" << endl;

}

string readFile(string fName)
{
    std::ifstream t(fName);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}