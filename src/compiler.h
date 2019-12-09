#ifndef __COMPILER__
#define __COMPILER__

#include <iostream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

const int notFound = -1;

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
        string getNextToken();
        void putBackToken(string token);
        bool EntryPoint();
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
        Result attribDef2(string insertBefore);
        Result attribDef3(string insertBefore);
        Result mathLevel1(string insertBefore);
        Result mathLevel2(string insertBefore);
        Result mathLevel3(string insertBefore);
        Result tokenNameOrData(string insertBefore);
        bool isNumber(string possibleNumber);

        int findInIntermediateCode(string search);
        
    public:
        vector<string> validate(vector<string> tokens);
};

#endif
