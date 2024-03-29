#ifndef __COMPILER__
#define __COMPILER__

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

const int notFound = -1;
const string END = string("___end___of___program___\0");

class Result{
    public:
        string result = "";
        string errors = "";
        bool wasRecognized = false;
        Result(string r = "", string e = ""): result(r), errors(e){}
};

class Compiler{
    private:
        int globalCount = 0;
        vector<string> tokens;
        vector<string> intermediateCode;
        string getNextToken(bool skeepBlanks = false);
        void putBackToken(string token);
        bool EntryPoint();
        Result LoopEntryPoint2(string insertBefore);
        Result EntryPoint2(string insertBefore);
        void insertIntermediateCode(string label, string code, int offset);
        string getNextTempName();

        Result var();
        Result varDeclaration();
        Result typeDeclaration(string type);
        Result getTokenNameList();

        Result _while(string insertBefore);
        Result _if(string insertBefore);
        Result blockOfCode(string insertBefore);
        Result blockOfLogic(string insertBefore);
        Result parentesisD(string insertBefore);
        Result logicTokenNameOrData(string insertBefore);
        Result logicOperator(string insertBefore);
        Result attribDef(string insertBefore);

        Result m1(string insertBefore);
        Result m2(string insertBefore);
        Result m3(string insertBefore);
        Result m4(string insertBefore);
        Result tokenNameOrData(string insertBefore);
        bool isNumber(string possibleNumber);

        int findInIntermediateCode(string search);
        
    public:
        vector<string> validate(vector<string> tokens);
};

#endif
