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

UserAuth* currentUser = nullptr;


bool authenticateUser() 
{
    string email, password;
    
    cout << "\n========== LOGIN REQUIRED ==========" << endl;
    cout << "Enter your email: ";
    cin >> email;
    
    cout << "Enter your password: ";
    cin >> password;
    
    // Accept any non-empty credentials
    if (!email.empty() && !password.empty()) 
    {
        
        currentUser = new UserAuth(email, password);
        cout << "✅ Login successful! Welcome to Password Manager." << endl;
        return true;
    } 
    else 
    {   
        cout << "❌ Email and password cannot be empty! Access denied." << endl;
        return false;
    }
}


