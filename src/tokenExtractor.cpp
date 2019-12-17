#include "tokenExtractor.h"

vector<string> TokenExtractor::getTokens(string programText)
{
    vector<string> tokens;
    string chars = "abcdefghijklmnopqrstuvxywzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    string numbers = "0123456789";
    string continuosSimbols = "<>=+-*/^";
    string singleSimbols = "\n\t,{}()";
    string state="findStart";
    string c = "";
    string temp = "";
    for (int count = 0; count < programText.size(); count++)
    {
        c = string(1, programText.at(count));
        if (state == "findStart")
        {
            if (chars.find(c) != string::npos)
                state = "readingWord";
            else if (numbers.find(c) != string::npos)
                state = "readingNumber";
            else if (continuosSimbols.find(c) != string::npos)
                state = "readingContinuousSimbols";
            else if (singleSimbols.find(c) != string::npos)
                state = "readingSingleSimbols";
            else if (c == "\"" || c == "'")
                state = "readingString";
            else
                count++;

            count --;
        }
        else if (state == "readingWord")
        {
            if ((chars.find(c) != string::npos) || (numbers.find(c) != string::npos))
                temp += c;
            else
            {
                tokens.push_back(temp);
                temp = "";
                state = "findStart";
                count--;
            }
        }
        else if (state == "readingNumber")
        {
            if (numbers.find(c) != string::npos)
                temp += c;
            else
            {
                tokens.push_back(temp);
                temp = "";
                state = "findStart";
                count--;
            }
        }
        else if (state == "readingContinuousSimbols")
        {
            if (continuosSimbols.find(c) != string::npos)
                temp += c;
            else
            {
                tokens.push_back(temp);
                temp = "";
                state = "findStart";
                count--;
            }
        }
        else if (state == "readingSingleSimbols")
        {
            tokens.push_back(c);
            state = "findStart";
        }
    }

    if (temp != "")
        tokens.push_back(temp);

    return tokens;
}

