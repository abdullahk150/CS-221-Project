#include <iostream>
#include <string>
#include <cctype>
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

struct UserAuth 
{
    string email;
    string password;
    
    UserAuth(string e, string p) 
    {
        email = e;
        password = p;
    }
};


