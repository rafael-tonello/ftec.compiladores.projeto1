#ifndef __COMPILER__
#define __COMPILER__

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

class Result{
    public:
        string result = "";
        string errors = "";
        bool wasRecognized = false;
        Result(string r = "", string e = ""): result(r), errors(e){}
};

class Compiler{
    private:
        queue<string> tokens;
        queue<string> intermediateCode;
        string getNextToken();
        void putBackToken(string token);
        bool EntryPoint();

        Result var();
        Result varDeclaration();
        Result intDeclaration();
        Result realDeclaration();
        
    public:
        vector<string> validadte(vector<string> tokens);
};

#endif
