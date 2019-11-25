#include "main.h"

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

        "while",
        "S_WHILE", "M_PARENTESIS_OPEN", "TN_cont", "M_LESSTHAN", "C_10", "M_PARENTESIS_CLOSE", "M_BLOCK_OPEN", "M_BRK",
            "M_TAB"
    };

}

