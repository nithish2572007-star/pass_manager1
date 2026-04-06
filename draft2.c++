#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Node 
{
    char Name[50];
    char Password[50];
    int xorKey;
    Node *left, *right;

    Node() 
    {
        right = left = nullptr;
        xorKey = 0;
    } 
};


class PasswordManager 
{
private:
    Node* root;

    void xorP(const char* input, char* output, int key) 
    {
        int i = 0;
        for (i = 0; input[i] != '\0'; i++) 
        {
            output[i] = input[i] ^ key;
        }
        output[i] = '\0';
    }

    Node* insert(Node* node, Node* newNode) 
    {
        if (node == nullptr) 
        {
            return newNode;
        }
        if (strcmp(newNode->Name, node->Name) < 0) 
        {
            node->left = insert(node->left, newNode);
        } 
        else if (strcmp(newNode->Name, node->Name) > 0) 
        {
            node->right = insert(node->right, newNode);
        }
        return node;
    }


    Node* search(Node* node, const char* account) 
    {
        if (node == nullptr || strcmp(node->Name, account) == 0) 
        {
            return node;
        }
        if (strcmp(account, node->Name) < 0) 
        {
            return search(node->left, account);
        }
        return search(node->right, account);
    }



    void inOrder(Node* node) 
    {
        if (node == nullptr) 
        {
            return;
        }
        inOrder(node->left);
        
        cout << "account: " << node->Name << endl;
        
        inOrder(node->right);
    }

    void savePreOrder(Node* node, ofstream& outFile) 
    {
        if (node == nullptr) 
        {
            return;
        }
        outFile.write(reinterpret_cast<const char*>(node), sizeof(Node));
        
        savePreOrder(node->left, outFile);
        
        savePreOrder(node->right, outFile);
    }



    void clearBST(Node* node) 
    {
        if (node == nullptr) 
        {
            return;
        }
        clearBST(node->left);
        
        clearBST(node->right);
        
        delete node;
    }
    
    void updateNode(Node* node, const char* a)
    {
        char oldPassword[50],password[50];
        cout << "Enter Old Password: ";
        cin.getline(oldPassword, 50);
        
        xorP(node->Password, password, node->xorKey);
        if(strcmp(password,oldPassword)==0)
        {
            cout<<"Password Verified, Enter new Passsword to update: ";
            char newPassword[50];
            cin.ignore();
            cin.getline(newPassword, 50);
            
            node->xorKey = (rand() % 32);
            xorP(newPassword, node->Password, node->xorKey);
        }
        else
        {
            cout<<"Password Mismatch.\n";
        }

    }
    
    
    Node* minValueNode(Node* node) 
    {
        Node* current = node;
        while (current && current->left != nullptr) 
        {
            current = current->left;
        }
        return current;
    }

    
    
    Node* deleteNode(Node* root, const char* account) 
    {
        if (root == nullptr) 
        {
            return root;
        }

        if (strcmp(account, root->Name) < 0) 
        {
            root->left = deleteNode(root->left, account);
        }
        else if (strcmp(account, root->Name) > 0) 
        {
            root->right = deleteNode(root->right, account);
        } 
        else 
        {
            if (root->left == nullptr) 
            {
                Node* temp = root->right;
                delete root;
                return temp;
            } 
            else if (root->right == nullptr) 
            {
                Node* temp = root->left;
                delete root;
                return temp;
            }

            
            
            Node* temp = minValueNode(root->right);

            strcpy(root->Name, temp->Name);
            
            strcpy(root->Password, temp->Password);
            
            root->xorKey = temp->xorKey;

            root->right = deleteNode(root->right, temp->Name);
        }
        return root;
    }

public:
    PasswordManager() : root(nullptr) 
    {
        loadFromFile();
    }

    ~PasswordManager() 
    {
        clearBST(root);
    }

    void addPassword() {
        Node* newNode = new Node();
        char rawPassword[50];

        cout << "Enter account Name: ";
        cin.ignore();
        cin.getline(newNode->Name, 50);
        if (search(root, newNode->Name) != nullptr) 
        {
            cout << "Account already exists.\n";
            delete newNode;
            return; 
        }

        cout << "Enter Password: ";
        cin.getline(rawPassword, 50);

        newNode->xorKey = (rand() % 32);

        xorP(rawPassword, newNode->Password, newNode->xorKey);
        root = insert(root, newNode);
        cout << "Password saved successfully!\n";
    }

    void searchPassword() 
    {
        if(root == nullptr)
        {
            return;
        
        }
        char account[50];
        cout << "Enter account Name to retrieve password: ";
        cin.ignore();
        cin.getline(account, 50);

        Node* result = search(root, account);
        if (result) 
        {
            char decrypted[50];
            xorP(result->Password, decrypted, result->xorKey);
            cout << "Account: " << result->Name << "\nPassword: " << decrypted << endl;
        }
        else 
        {
            cout << "Account not found.\n";
        }
    }

    void displayAll() 
    {
        if (root == nullptr) 
        {
            cout << "No accounts available.\n";
            return;
        }
        cout << "--- Managed accounts ---\n";
        inOrder(root);
        cout << "------------------------\n";
    }

    void removePassword() 
    {
        if (root == nullptr)
            return;

        char account[50];
        cout << "Enter account Name to delete: ";
        cin.ignore();
        cin.getline(account, 50);

        if (search(root, account) != nullptr) 
        {
            root = deleteNode(root, account);
            cout << "Account '" << account << "' deleted successfully.\n";
        }
        else 
        {
            cout << "Account not found.\n";
        }
    }
    
    void updatePassword()
    {
        if (root == nullptr)
            return;
        
        char account[50];
        cout << "Enter account Name to update: ";
        cin.ignore();
        cin.getline(account, 50);
        
        if (search(root, account) != nullptr)
        {
            updateNode(root, account);
            cout << "Password for account '" << account << "' updated successfully.\n";
        }
        else
        {
            cout << "Account not found.\n";
        }


    }

    void saveToFile() 
    {
        ofstream outFile("vault1.dat", ios::binary | ios::trunc);
        if (!outFile) 
        {
            cout << "Error opening file for saving.\n" << endl;
            return;
        }
        savePreOrder(root, outFile);
        outFile.close();
    }    

    void loadFromFile() 
    {
        ifstream inFile("vault1.dat", ios::binary);
        if (!inFile) 
        {
            return;
        }

        Node tempNode;
        while (inFile.read(reinterpret_cast<char*>(&tempNode), sizeof(Node))) 
        {
            Node* newNode = new Node();
        
            strcpy(newNode->Name, tempNode.Name);
        
            strcpy(newNode->Password, tempNode.Password);
        
            newNode->xorKey = tempNode.xorKey;
        
            newNode->left = newNode->right = nullptr;
        
            root = insert(root, newNode);
        }
        inFile.close();
    }
};

int main() 
{
    PasswordManager manager;
    int choice;

    do 
    {
        cout << "\n--- Password Manager ---\n";
        cout << "1. Add Password\n";
        cout << "2. Retrive account and Password\n";
        cout << "3. Display All accounts\n";
        cout << "4. Delete Password\n";
        cout << "5. Update Password\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) 
        {
            case 1:
                manager.addPassword();
                break;
            case 2:
                manager.displayAll();
                manager.searchPassword();
                break;
            case 3:
                manager.displayAll();
                break;
            case 4:
                manager.displayAll();
                manager.removePassword();
                break;
            case 5:
                manager.displayAll();
                manager.updatePassword();
                break;
            case 6:
                cout << "Saving data and exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
        manager.saveToFile();
    } while (choice != 6);

    return 0;
}
