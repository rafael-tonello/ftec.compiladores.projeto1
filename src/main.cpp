#include "main.h"
#include "compiler.h"

/*
    Planning "A"
        first day
            [ ] language specification (grammar)
            [x] basic structure
            [ ] code writing planning

    day 11, the plannin was not well executed... a new planning was make, but it was not documented (I'm afraid of Murphy spirt. The planning is just in my mind)
    
*/

using namespace std;

int main()
{
    vector<string> tokens = {
        "var", "\n",
            "\t", "int", "cont", ",", "num", "\n",
            "\t", "real", "cont2", "\n",
            
        "num", "=", "5", "+", "2" , /*"/" , "3",*/ "\n",

        "while", "(", "cont", "<", "10", ")", "{",

            /*"cont2", "=", "3.1415", "*", "cont", "^", "2", 
            "if", "(", "cont", "<", "5", ")","{",
                "num", "=", "num", "+", "cont2",
            "}",
            "else","{",
                "cont", "=", "0",
            "}",
            
            "cont", "=", "cont", "+", "1",*/

            "num", "=", "10",
        "}"
        
    };

    Compiler *cmp = new Compiler();
    cmp->validate(tokens);

    cout << "Hello, can you hear me? Listen well: If you give this man a ride, sweet family will die!" << endl;

}

