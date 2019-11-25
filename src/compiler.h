#ifndef __COMPILER__
#define __COMPILER__

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

class Compiler{
    private:
        queue<string> tokens;
        string getNextToken();
        bool EntryPoint();
        
    public:
        vector<string> validadte(vector<string> tokens);
};

#endif
