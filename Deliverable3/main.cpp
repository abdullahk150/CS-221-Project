#include <iostream>
#include <string>
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

// Action represents a single change (for undo/redo)
struct Action 
{
    string actionType;      // "ADD", "EDIT", "DELETE"
    string accountName;     
    string oldPassword;     // For EDIT and DELETE
    string newPassword;     // For ADD and EDIT
};

// Stack for Undo/Redo operations (array-based)
struct ActionStack 
{
    Action actions[50];   
    int top;
    
    void init() 
    {
        top = -1;
    }
    
    void push(Action action) 
    {
        if (top < 49) 
        {
            actions[++top] = action;
        } else 
        {
            cout << "⚠️ Undo history full!\n";
        }
    }
    
    Action pop() 
    {
        if (top >= 0) 
        {
            return actions[top--];
        }

        Action empty;
        empty.actionType = "NONE";
        return empty;
    }
    
    bool isEmpty() 
    {
        return top == -1;
    }
    
    void clear() 
    {
        top = -1;
    }
};

struct ViewAttempt
{
    bool success;
};

const size_t MAX_VIEW_HISTORY = 10;

struct ViewAttemptQueue
{
    ViewAttempt buffer[MAX_VIEW_HISTORY];
    size_t count;
    size_t startIndex;

    ViewAttemptQueue()
    {
        count = 0;
        startIndex = 0;
    }

    void push(ViewAttempt attempt)
    {
        if (count < MAX_VIEW_HISTORY)
        {
            size_t insertIndex = (startIndex + count) % MAX_VIEW_HISTORY;
            buffer[insertIndex] = attempt;
            ++count;
        }
        else
        {
            buffer[startIndex] = attempt;
            startIndex = (startIndex + 1) % MAX_VIEW_HISTORY;
        }
    }

    bool getFromEnd(size_t reverseIndex, ViewAttempt& out) const
    {
        if (reverseIndex >= count) return false;
        size_t lastIndex = (startIndex + count - 1) % MAX_VIEW_HISTORY;
        size_t idx = (lastIndex + MAX_VIEW_HISTORY - (reverseIndex % MAX_VIEW_HISTORY)) % MAX_VIEW_HISTORY;
        out = buffer[idx];
        return true;
    }

    size_t size() const 
    { 
        return count;
    }
};

// BST Node that stores a pointer to PasswordNode
struct BSTNode
{
    PasswordNode* passwordNodePtr;  // Pointer to the actual PasswordNode in linked list
    BSTNode* left;
    BSTNode* right;

    BSTNode(PasswordNode* ptr)
    {
        passwordNodePtr = ptr;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree structure for account names (stores pointers to PasswordNode)
struct AccountBST
{
    BSTNode* root;

    // Constructor
    AccountBST()
    {
        root = nullptr;
    }

    BSTNode* insertHelper(BSTNode* node, PasswordNode* passwordNode)
    {
        // If tree is empty, create new node
        if (node == nullptr)
        {
            return new BSTNode(passwordNode);
        }

        // Compare based on account name from the PasswordNode
        string accountName = passwordNode->accountName;
        string nodeAccountName = node->passwordNodePtr->accountName;

        if (accountName < nodeAccountName)
        {
            // Insert in left subtree
            node->left = insertHelper(node->left, passwordNode);
        }
        else if (accountName > nodeAccountName)
        {
            // Insert in right subtree
            node->right = insertHelper(node->right, passwordNode);
        }
        // If accountName == nodeAccountName, do nothing (duplicate)

        return node;
    }

    // Insert pointer to PasswordNode into BST
    void insert(PasswordNode* passwordNode)
    {
        root = insertHelper(root, passwordNode);
    }

    // Search for a PasswordNode by account name
    PasswordNode* search(const string& accountName)
    {
        BSTNode* current = root;
        while (current != nullptr)
        {
            string currentAccountName = current->passwordNodePtr->accountName;
            if (accountName == currentAccountName)
            {
                return current->passwordNodePtr;
            }
            else if (accountName < currentAccountName)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }
        return nullptr;
    }

    // Helper function to find and delete a node from BST
    BSTNode* deleteHelper(BSTNode* node, PasswordNode* passwordNodeToDelete)
    {
        if (node == nullptr) 
        
        
        {
            return nullptr;
        }

        string accountName = passwordNodeToDelete->accountName;
        string nodeAccountName = node->passwordNodePtr->accountName;

        if (accountName < nodeAccountName)
        {
            node->left = deleteHelper(node->left, passwordNodeToDelete);
        }
        else if (accountName > nodeAccountName)
        {
            node->right = deleteHelper(node->right, passwordNodeToDelete);
        }
        else
        {
            // Found the node to delete
            // Check if it's the exact same PasswordNode pointer
            if (node->passwordNodePtr == passwordNodeToDelete)
            {
                // Case 1: No children
                if (node->right == nullptr && node->left == nullptr)
                {
                    delete node;
                    return nullptr;
                }
                // Case 2: One child - only left child exists
                else if (node->right == nullptr)
                {
                    BSTNode* temp = node->left;
                    delete node;
                    return temp;
                }
                // Case 3: One child - only right child exists
                else if (node->left == nullptr)
                {
                    BSTNode* temp = node->right;
                    delete node;
                    return temp;
                }
                // Case 4: Two children - get inorder successor (smallest in right subtree)
                else
                {
                    BSTNode* temp = node->right;
                    while (temp->left != nullptr)
                    {
                        temp = temp->left;
                    }

                    // Copy the pointer (not the data)
                    node->passwordNodePtr = temp->passwordNodePtr;
                    
                    // Delete the inorder successor
                    node->right = deleteHelper(node->right, temp->passwordNodePtr);
                }
            }
        }
        return node;
    }

    // Delete a PasswordNode pointer from BST
    void remove(PasswordNode* passwordNode)
    {
        root = deleteHelper(root, passwordNode);
    }

    // Helper function for in-order traversal to collect pointers
    void inOrderCollect(BSTNode* node, PasswordNode* nodes[], int& index, int maxSize)
    {
        if (node != nullptr && index < maxSize)
        {
            inOrderCollect(node->left, nodes, index, maxSize);
            if (index < maxSize)
            {
                nodes[index++] = node->passwordNodePtr;
            }
            inOrderCollect(node->right, nodes, index, maxSize);
        }
    }

    // Get all PasswordNode pointers in sorted order (returns count)
    int getAllNodes(PasswordNode* nodes[], int maxSize)
    {
        int count = 0;
        inOrderCollect(root, nodes, count, maxSize);
        return count;
    }

    // Check if BST is empty
    bool isEmpty()
    {
        return root == nullptr;
    }
};

// ==================== GLOBAL VARIABLES ====================

UserAuth* currentUser = nullptr;
ViewAttemptQueue viewAttempts;
ActionStack undoStack;
ActionStack redoStack;

// ==================== HELPER FUNCTIONS ====================

void recordViewAttempt(bool success)
{
    viewAttempts.push({ success });
}

bool lastKViewAttemptsFailed(size_t k)
{
    if (viewAttempts.size() < k) return false;
    int failed = 0;
    for (size_t i = 0; i < k; ++i)
    {
        ViewAttempt a;
        if (viewAttempts.getFromEnd(i, a))
        {
            if (!a.success) ++failed;
        }
    }
    return failed == k;
}

string xorCipher(const string& data, const string& key)
{
    if (key.empty()) 
    {
        return data;
    }

    string out = data;
    for (int i = 0; i < data.size(); ++i)
    {
        out[i] = static_cast<char>(data[i] ^ key[i % key.size()]);
    }
    return out;
}

string encryptPassword(const string& password)
{
    const string key = "X0rKey!2025";
    return xorCipher(password, key);
}

string decryptPassword(const string& encryptedPassword)
{
    const string key = "X0rKey!2025";
    return xorCipher(encryptedPassword, key);
}

bool isValidEmail(const string& email) 
{
    if (email.empty()) 
    {
        return false;
    }
    
    // Check if email contains @ symbol
    int atPos = email.find('@');
    if (atPos == string::npos || atPos == 0) 
    {
        return false;
    }
    
    // Check if there's text before @
    if (atPos == 0) 
    {
        return false;
    }
    
    // Extract domain part (after @)
    string domain = email.substr(atPos + 1);
    if (domain.empty()) 
    {
        return false;
    }
    
    // Check if domain contains a dot (for extension like .com, .org, etc.)
    int dotPos = domain.find('.');
    if (dotPos == string::npos || dotPos == 0) 
    {
        return false;
    }
    
    return true;
}

bool checkAndSuggestStrength(const string& password) 
{
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (char c : password) 
    {
        bool isUppercase = (c >= 'A' && c <= 'Z');
        bool isLowercase = (c >= 'a' && c <= 'z');
        bool isNumeric   = (c >= '0' && c <= '9');
        bool isAlnum     = isUppercase || isLowercase || isNumeric;

        if (!hasUpper && isUppercase) hasUpper = true;
        if (!hasDigit && isNumeric) hasDigit = true;
        if (!hasSymbol && !isAlnum) hasSymbol = true;

        if (hasUpper && hasDigit && hasSymbol) break;
    }

    if (hasUpper && hasDigit && hasSymbol) 
    {
        cout << "✅ Password looks strong.\n";
        return true;
    }

    cout << "⚠️ Password could be stronger. Consider adding:\n";
    if (!hasUpper) 
    {
        cout << "   - Uppercase letters (A-Z)\n";
    }

    if (!hasDigit) 
    {
        cout << "   - Numbers (0-9)\n";
    }

    if (!hasSymbol) 
    {
        cout << "   - Symbols (!@#$%^&* etc.)\n";
    }

    return false;
}

bool authenticateUser() 
{
    string email, password;
    
    cout << "\n========== LOGIN REQUIRED ==========" << endl;
    
    // Loop until valid email is entered
    while (true)
    {
        cout << "Enter your email: ";
        cin >> email;
        
        // Validate email format
        if (isValidEmail(email))
        {
            break; 
        }
        else
        {
            cout << "❌ Invalid email format. Please try again.\n";
        }
    }
    
    // Loop until non-empty password is entered
    while (true)
    {
        cout << "Enter your password: ";
        cin >> password;
        
        if (!password.empty())
        {
            break; // Valid password, exit loop
        }
        else
        {
            cout << "❌ Password cannot be empty. Please try again.\n";
        }
    }
    
    currentUser = new UserAuth(email, password);
    cout << "✅ Login successful! Welcome to Password Manager.\n";
    return true;
}

bool verifyPassword() 
{
    string inputPassword;
    cout << "\nEnter your password to view passwords: ";
    cin >> inputPassword;
    
    if (currentUser && inputPassword == currentUser->password) 
    {
        cout << "✅ Password verified!\n";
        return true;
    } 
    else 
    {
        cout << "❌ Incorrect password! Access denied.\n";
        return false;
    }
}

bool logout() 
{
    cout << "\n========== LOGOUT ==========" << endl;
    cout << "Are you sure you want to logout? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') 
    {
        cout << "✅ Logged out successfully!\n";
        return true;
    }
    return false;
}


struct PasswordManager 
{
    PasswordNode* head = nullptr;
    AccountBST bst;  // BST stores pointers to PasswordNodes for fast searching

    PasswordManager() 
    {
        head = nullptr;
    }

    // Destructor to clean up memory
    ~PasswordManager()
    {
        clearAllPasswords();
    }

    // Find node by account name
    PasswordNode* findNodeByAccount(const string& account)
    {
        PasswordNode* temp = head;
        while (temp)
        {
            if (temp->accountName == account) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    // Check if password is already used by another account
    string findAccountWithPassword(const string& encryptedPassword, const string& excludeAccount)
    {
        PasswordNode* temp = head;
        while (temp)
        {
            if (temp->password == encryptedPassword && temp->accountName != excludeAccount)
            {
                return temp->accountName;
            }
            temp = temp->next;
        }
        return "";
    }

    // Add a new password
    void addPassword() 
    {   
        cin.ignore();
        string account, pass;
        
        // Loop until non-empty account name is entered
        while (true)
        {
            cout << "\nEnter Account Name (e.g., Gmail, Facebook, Bank): ";
            getline(cin, account);
            
            if (!account.empty())
            {
                break; // Valid account name, exit loop
            }
            else
            {
                cout << "❌ Account name cannot be empty. Please try again.\n";
            }
        }

        cout << "Enter Password for " << account << ": ";
        getline(cin, pass);

        checkAndSuggestStrength(pass);

        string encrypted = encryptPassword(pass);
        
        // Check if this password is already used by another account
        string existingAccount = findAccountWithPassword(encrypted, account);
        if (!existingAccount.empty())
        {
            cout << "⚠️ Warning: This password is already used for account \"" << existingAccount << "\". Try a new password for better security.\n";
        }
        
        PasswordNode* newNode = new PasswordNode(account, encrypted);

        if (head == nullptr) 
        {
            head = newNode;
        } 
        else 
        {
            PasswordNode* temp = head;
            while (temp->next != nullptr) 
            {
               temp = temp->next;
            }
            
            temp->next = newNode;
        }

        // Insert pointer to PasswordNode into BST (for fast searching and sorted display)
        bst.insert(newNode);

        // Record action for undo
        Action action;
        action.actionType = "ADD";
        action.accountName = account;
        action.newPassword = encrypted;
        undoStack.push(action);
        redoStack.clear(); // Clear redo stack after new action

        cout << "✅ Password for " << account << " added successfully!\n";
    }

    // View all passwords (using BST for sorted display)
    void viewPasswords() 
    {
        if (!verifyPassword()) 
        {
            recordViewAttempt(false);
            if (lastKViewAttemptsFailed(3))
            {
                cout << "⚠️ Multiple failed view attempts detected (last 3).\n";
            }
            return;
        }
        recordViewAttempt(true);
        
        if (!head || bst.isEmpty()) 
        {
            cout << "\nNo passwords saved yet.\n";
            return;
        }

        // Get all PasswordNode pointers from BST in sorted order
        const int MAX_ACCOUNTS = 1000;
        PasswordNode* sortedNodes[MAX_ACCOUNTS];
        int nodeCount = bst.getAllNodes(sortedNodes, MAX_ACCOUNTS);

        cout << "\n========== Your Stored Passwords (Sorted by Account Name) ==========\n";
        for (int i = 0; i < nodeCount; i++)
        {
            PasswordNode* node = sortedNodes[i];
            cout << (i + 1) << ". Account: " << node->accountName << endl;
            cout << "   Password: " << decryptPassword(node->password) << endl;
            cout << "   ------------------------------------------\n";
        }
    }

    // Edit existing password (uses BST for fast searching)
    void editPassword()
    {
        if (!head || bst.isEmpty())
        {
            cout << "\nNo passwords to edit.\n";
            return;
        }

        cin.ignore();
        string account;
        cout << "\nEnter Account Name to edit: ";
        getline(cin, account);

        // Use BST to quickly find the PasswordNode
        PasswordNode* node = bst.search(account);
        if (!node)
        {
            cout << "❌ Account not found.\n";
            return;
        }

        cout << "Current Password: " << decryptPassword(node->password) << endl;
        string newPass;
        cout << "Enter New Password: ";
        getline(cin, newPass);

        checkAndSuggestStrength(newPass);

        string encryptedNewPass = encryptPassword(newPass);
        
        // Check if this password is already used by another account
        string existingAccount = findAccountWithPassword(encryptedNewPass, account);
        if (!existingAccount.empty())
        {
            cout << "⚠️ Warning: This password is already used for account \"" << existingAccount << "\". Try a new password for better security.\n";
        }

        // Record action for undo
        Action action;
        action.actionType = "EDIT";
        action.accountName = account;
        action.oldPassword = node->password; // Store encrypted old password
        action.newPassword = encryptedNewPass;
        undoStack.push(action);
        redoStack.clear(); // Clear redo stack after new action

        node->password = action.newPassword;

        cout << "✅ Password updated for " << account << "!\n";
    }

    // Delete a password (removes from both BST and linked list)
    void deletePassword()
    {
        if (!head || bst.isEmpty())
        {
            cout << "\nNo passwords to delete.\n";
            return;
        }

        cin.ignore();
        string account;
        cout << "\nEnter Account Name to delete: ";
        getline(cin, account);

        // Use BST to quickly find the PasswordNode pointer
        PasswordNode* nodeToDelete = bst.search(account);
        
        if (!nodeToDelete)
        {
            cout << "❌ Account not found.\n";
            return;
        }

        // Record action for undo
        Action action;
        action.actionType = "DELETE";
        action.accountName = account;
        action.oldPassword = nodeToDelete->password; // Store encrypted password
        undoStack.push(action);
        redoStack.clear(); // Clear redo stack after new action

        // Remove from BST first (just removes the BST node, not the PasswordNode)
        bst.remove(nodeToDelete);

        // Now remove from linked list (this deletes the actual PasswordNode)
        PasswordNode* temp = head;
        PasswordNode* prev = nullptr;

        while (temp && temp != nodeToDelete)
        {
            prev = temp;
            temp = temp->next;
        }

        if (temp == nodeToDelete)
        {
            if (prev == nullptr) 
            {
                head = temp->next;
            } 
            else 
            {
                prev->next = temp->next;
            }

            delete temp;  // Delete the actual PasswordNode (account name and password)
        }

        cout << "✅ Password for " << account << " deleted successfully!\n";
    }

    // Undo last action
    void undo()
    {
        if (undoStack.isEmpty())
        {
            cout << "\n❌ Nothing to undo!\n";
            return;
        }

        Action action = undoStack.pop();
        redoStack.push(action); // Move to redo stack

        if (action.actionType == "ADD")
        {
            // Find the node with matching account name AND password
            PasswordNode* temp = head;
            PasswordNode* prev = nullptr;

            while (temp && (temp->accountName != action.accountName || temp->password != action.newPassword))
            {
                prev = temp;
                temp = temp->next;
            }

            if (temp)
            {
                // Remove from BST first
                bst.remove(temp);

                // Then remove from linked list
                if (prev == nullptr)
                    head = temp->next;
                else
                    prev->next = temp->next;

                delete temp;  // Delete the actual PasswordNode
                cout << "✅ Undo: Removed password for " << action.accountName << "\n";
            }
            else
            {
                cout << "⚠️ Undo: Could not find password for " << action.accountName << " to remove.\n";
            }
        }
        else if (action.actionType == "EDIT")
        {
            // Restore old password (use BST for fast search)
            PasswordNode* node = bst.search(action.accountName);
            if (node)
            {
                // Verify the current password matches what we expect (the newPassword from the action)
                if (node->password == action.newPassword)
                {
                    node->password = action.oldPassword;
                    cout << "✅ Undo: Restored old password for " << action.accountName << "\n";
                }
                else
                {
                    cout << "⚠️ Undo: Password for " << action.accountName << " has been modified. Cannot undo.\n";
                }
            }
            else
            {
                cout << "⚠️ Undo: Account " << action.accountName << " not found. Cannot undo edit.\n";
            }
        }
        else if (action.actionType == "DELETE")
        {
            // Re-add the deleted password
            PasswordNode* newNode = new PasswordNode(action.accountName, action.oldPassword);

            if (!head)
            {
                head = newNode;
            } else 
            {
                PasswordNode* temp = head;
                while (temp->next) temp = temp->next;
                temp->next = newNode;
            }

            // Re-insert into BST
            bst.insert(newNode);

            cout << "✅ Undo: Restored password for " << action.accountName << "\n";
        }
    }

    // Redo last undone action
    void redo()
    {
        if (redoStack.isEmpty())
        {
            cout << "\n❌ Nothing to redo!\n";
            return;
        }

        Action action = redoStack.pop();
        bool success = false;

        if (action.actionType == "ADD")
        {
            // Check if account already exists (use BST for fast search)
            PasswordNode* existing = bst.search(action.accountName);
            if (existing)
            {
                // Check if it's the same password (from a previous redo)
                if (existing->password == action.newPassword)
                {
                    cout << "⚠️ Redo: Password for " << action.accountName << " already exists.\n";
                    redoStack.push(action); // Put it back
                    return;
                }
                else
                {
                    cout << "⚠️ Redo: Account " << action.accountName << " already exists with different password. Cannot redo.\n";
                    redoStack.push(action); // Put it back
                    return;
                }
            }

            // Re-add the password
            PasswordNode* newNode = new PasswordNode(action.accountName, action.newPassword);

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

            // Re-insert into BST
            bst.insert(newNode);

            cout << "✅ Redo: Re-added password for " << action.accountName << "\n";
            success = true;
        }
        else if (action.actionType == "EDIT")
        {
            // Reapply new password (use BST for fast search)
            PasswordNode* node = bst.search(action.accountName);
            if (node)
            {
                // Verify the current password matches the oldPassword (what we expect after undo)
                if (node->password == action.oldPassword)
                {
                    node->password = action.newPassword;
                    cout << "✅ Redo: Reapplied new password for " << action.accountName << "\n";
                    success = true;
                }
                else
                {
                    cout << "⚠️ Redo: Password for " << action.accountName << " has been modified. Cannot redo.\n";
                    redoStack.push(action); // Put it back
                    return;
                }
            }
            else
            {
                cout << "⚠️ Redo: Account " << action.accountName << " not found. Cannot redo edit.\n";
                redoStack.push(action); // Put it back
                return;
            }
        }
        else if (action.actionType == "DELETE")
        {
            // Use BST to find the node quickly
            PasswordNode* nodeToDelete = bst.search(action.accountName);
            
            if (!nodeToDelete || nodeToDelete->password != action.oldPassword)
            {
                cout << "⚠️ Redo: Could not find password for " << action.accountName << " to delete.\n";
                redoStack.push(action); // Put it back
                return;
            }

            // Remove from BST first
            bst.remove(nodeToDelete);

            // Then remove from linked list
            PasswordNode* temp = head;
            PasswordNode* prev = nullptr;

            while (temp && temp != nodeToDelete)
            {
                prev = temp;
                temp = temp->next;
            }

            if (temp == nodeToDelete)
            {
                if (prev == nullptr)
                    head = temp->next;
                else
                    prev->next = temp->next;

                delete temp;  // Delete the actual PasswordNode
                cout << "✅ Redo: Deleted password for " << action.accountName << "\n";
                success = true;
            }
            else
            {
                cout << "⚠️ Redo: Could not find password for " << action.accountName << " to delete.\n";
                redoStack.push(action); // Put it back
                return;
            }
        }

        // Only move to undo stack if redo was successful
        if (success)
        {
            undoStack.push(action);
        }
    }

    
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
};


int main() 
{
    PasswordManager pm;
    int choice;
    bool loggedIn = false;

    // Initialize stacks
    undoStack.init();
    redoStack.init();

    do {
        if (!loggedIn) 
        {
            if (!authenticateUser()) 
            {
                cout << "Program terminated due to failed authentication.\n";
                return 0;
            }
            loggedIn = true;
        }
        
        cout << "\n====== PASSWORD MANAGER ======" << endl;
        cout << "Logged in as: " << currentUser->email << endl;
        cout << "1. Add Password" << endl;
        cout << "2. View All Passwords" << endl;
        cout << "3. Edit Password" << endl;
        cout << "4. Delete Password" << endl;
        cout << "5. Undo Last Action" << endl;
        cout << "6. Redo Last Undo" << endl;
        cout << "7. Logout" << endl;
        cout << "8. Exit" << endl;
        
        
        while (true)
        {
            cout << "Enter your choice: ";
            if (cin >> choice)
            {
                
                if (choice >= 1 && choice <= 8)
                {
                    break;
                }
                else
                {
                    cout << "❌ Invalid choice! Please enter a number between 1 and 8.\n";
                }
            }
            else
            {
                // Invalid input (non-numeric)
                cout << "❌ Invalid input! Please enter a number between 1 and 8.\n";
                cin.clear(); // Clear error flags
                cin.ignore(10000, '\n');
            }
        }

        switch (choice) 
        {
            case 1:
                pm.addPassword();
                break;
            case 2:
                pm.viewPasswords();
                break;
            case 3:
                pm.editPassword();
                break;
            case 4:
                pm.deletePassword();
                break;
            case 5:
                pm.undo();
                break;
            case 6:
                pm.redo();
                break;
            case 7:
                if (logout()) 
                {
                    pm.clearAllPasswords(); 
                    delete currentUser;
                    currentUser = nullptr;
                    loggedIn = false;
                    undoStack.clear();
                    redoStack.clear();
                }
                break;
            case 8:
                cout << "Exiting...\n";
                pm.clearAllPasswords();
                delete currentUser;
                break;
            default:
                cout << "❌ Invalid choice!\n";
                break;
        }

    } while (choice != 8);

    return 0;
}