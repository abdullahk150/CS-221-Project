#include <iostream>
#include <string>
#include <cctype>
#include <ctime>
using namespace std;

struct PasswordNode
{
    string accountName;
    string password;
    PasswordNode* next;

     PasswordNode(string acc, string pass) 
     {
        accountName = acc;
        password = pass;
        next = nullptr;
    }
};

int main()
{

}