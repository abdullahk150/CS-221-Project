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


struct PasswordManager
{

    PasswordNode* head;
    ActivityQueue activityQueue;    // Queue for activity logging

    PasswordManager() 
    {
        head = nullptr;
    }

    void addPassword() 
    {
        string account, pass;

        cin.ignore(); 
        cout << "\nEnter Account Name (e.g., Gmail, Facebook, Bank): ";
        getline(cin, account);

        cout << "Enter Password for " << account << ": ";
        getline(cin, pass);

        checkAndSuggestStrength(pass);

        PasswordNode* newNode = new PasswordNode(account, pass);

        if (!head) 
        {
            head = newNode;
        } 
        else
        {
            PasswordNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }

        cout << "✅ Password for " << account << " added successfully!\n";
    }
};


int main()
{

}