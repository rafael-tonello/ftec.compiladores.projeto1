#include "main.h"
#include "compiler.h"

/*
    Planning 
        first day
            [ ] language specification (grammar)
            [x] basic structure
            [ ] code writing planning
            
    
*/

using namespace std;

int main()
{
    /*S_ = SPECIAL WORD
    TN_ = TOKEN NAME
    M_ = SYMBOL
    C_ = CONST*/
    vector<string> tokens = {
        "\"var\"", "\n"
            "\t", "\"int\"", "cont", ",", "num", "\n",
            "\t", "\"real\"", "cont2", "\n",
        "num", "=", "0", "\n",

        "while", "(", "cont", "<", "10", ")", "{",
        
    };

    Compiler *cmp = new Compiler();
    cmp->validate(tokens);

    cout << "Hello, can you hear me? Listen well: If you give this man a ride, sweet family will die!" << endl;

}

