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
        "S_VAR", "M_BRK",
            "M_TAB", "S_INT", "TN_cont", "M_COMMA", "TN_num", "M_BRK",
            "M_TAB", "S_REAL", "TN_cont2", "M_BRK",
        "TN_num", "M_ASSIGN", "C_0", "M_BRK",
        
        "S_WHILE", "M_PARENTESIS_OPEN", "TN_cont", "M_LESSTHAN", "C_10", "M_PARENTESIS_CLOSE", "M_BLOCK_OPEN", "M_BRK",
            "M_TAB"
    };

}

