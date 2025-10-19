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

bool verifyPassword() 
{
    string inputPassword;
    cout << "\nEnter your password to view passwords: ";
    cin >> inputPassword;
    
    if (currentUser && inputPassword == currentUser->password) {
        cout << "✅ Password verified!" << endl;
        return true;
    } else {
        cout << "❌ Incorrect password! Access denied." << endl;
        return false;
    }
}

bool checkAndSuggestStrength(const string& password) 
{
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (char c : password) 
    {
        if (!hasUpper && isupper(static_cast<unsigned char>(c))) hasUpper = true;
        if (!hasDigit && isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        if (!hasSymbol && !isalnum(static_cast<unsigned char>(c))) hasSymbol = true;
        if (hasUpper && hasDigit && hasSymbol) break;
    }

    if (hasUpper && hasDigit && hasSymbol) 
    {
        cout << "Password looks strong." << '\n';
        return true;
    }

    cout << "Password could be stronger. Consider adding:" << '\n';
    if (!hasUpper) 
    {
        cout << "- Uppercase letters (A-Z)" << '\n';
    }
    if (!hasDigit) 
    {
        cout << "- Numbers (0-9)" << '\n';
    }
    if (!hasSymbol) 
    {
        cout << "- Symbols (!@#$%^&* etc.)" << '\n';
    }
    return false;
}

struct PasswordManager 
{
    PasswordNode* head;

    PasswordManager() 
    {
        head = nullptr;
    }

    void addPassword() 
    {
        string account, pass;

        cin.ignore(); // clear leftover newline
        cout << "\nEnter Account Name (e.g., Gmail, Facebook, Bank): ";
        getline(cin, account);

        cout << "Enter Password for " << account << ": ";
        getline(cin, pass);

        // Check strength and suggest improvements (does not block saving)
        checkAndSuggestStrength(pass);

        PasswordNode* newNode = new PasswordNode(account, pass);

        if (!head) 
        {
            head = newNode;
        } else 
        {
            PasswordNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }

        cout << "✅ Password for " << account << " added successfully!\n";
    }
};

void viewPasswords() 
    {
        // First verify user's password before showing passwords
        if (!verifyPassword()) 
        {
            return; // Exit if password verification fails
        }
        
        if (!head) 
        {
            cout << "\nNo passwords saved yet.\n";
            return;
        }

        PasswordNode* temp = head;
        cout << "\n---- Your Stored Passwords ----\n";
        int count = 1;
        while (temp) 
        {
            cout << count << ". Account: " << temp->accountName << endl;
            cout << "   Password: " << temp->password << endl;
            cout << "   ---------------------------\n";
            temp = temp->next;
            count++;
        }
    }
    
    // Method to clear all passwords (for logout)
    void clearAllPasswords() 
    {
        PasswordNode* temp = head;
        while (temp) 
        {
            PasswordNode* toDelete = temp;
            temp = temp->next;
            delete toDelete;
        }
        head = nullptr;
    }


// Function to logout and allow new user login
bool logout() 
{
    cout << "\n========== LOGOUT ==========" << endl;
    cout << "Are you sure you want to logout? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') 
    {
        cout << "✅ Logged out successfully!" << endl;
        return true;
    }
    return false;
}



