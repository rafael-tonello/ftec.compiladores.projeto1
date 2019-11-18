#ifndef __VALIDATOR__
#define __VALIDATOR__

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

class Validator{
    private:
        queue<string> tokens;
        string getNextToken();
        bool initialToken();
        
    public:
        void validadte(vector<string> tokens);
};

#endif
