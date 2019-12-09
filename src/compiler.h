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
        vector<string> tokens;
        vector<string> intermediateCode;
        string getNextToken();
        void putBackToken(string token);
        Result EntryPoint();
        Result EntryPoint2();

        Result var();
        Result varDeclaration();
        Result typeDeclaration(string type);
        Result getTokenNameList();

        Result _while();
        Result _if();
        Result blockOfCode();
        Result blockOfLogic();
        
    public:
        vector<string> validate(vector<string> tokens);
};

#endif
