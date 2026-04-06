#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>

using namespace std;

struct PasswordNode 
{
    char serviceName[50];
    char encryptedPassword[50];
    int xorKey;
    PasswordNode *left, *right;

    PasswordNode() 
    {
        right = left = nullptr;
        xorKey = 0;
    } 
};


class PasswordManager 
{
private:
    PasswordNode* root;

    void xorProcess(const char* input, char* output, int key) 
    {
        int i = 0;
        for (i = 0; input[i] != '\0'; i++) 
        {
            output[i] = input[i] ^ key;
        }
        output[i] = '\0';
    }

    PasswordNode* insert(PasswordNode* node, PasswordNode* newNode) 
    {
        if (node == nullptr) 
        {
            return newNode;
        }
        if (strcmp(newNode->serviceName, node->serviceName) < 0) 
        {
            node->left = insert(node->left, newNode);
        } 
        else if (strcmp(newNode->serviceName, node->serviceName) > 0) 
        {
            node->right = insert(node->right, newNode);
        }
        return node;
    }


    PasswordNode* search(PasswordNode* node, const char* service) 
    {
        if (node == nullptr || strcmp(node->serviceName, service) == 0) 
        {
            return node;
        }
        if (strcmp(service, node->serviceName) < 0) 
        {
            return search(node->left, service);
        }
        return search(node->right, service);
    }



    void inOrder(PasswordNode* node) 
    {
        if (node == nullptr) 
        {
            return;
        }
        inOrder(node->left);
        
        cout << "Service: " << node->serviceName << endl;
        
        inOrder(node->right);
    }

    void savePreOrder(PasswordNode* node, ofstream& outFile) 
    {
        if (node == nullptr) 
        {
            return;
        }
        outFile.write(reinterpret_cast<const char*>(node), sizeof(PasswordNode));
        
        savePreOrder(node->left, outFile);
        
        savePreOrder(node->right, outFile);
    }



    void clearBST(PasswordNode* node) 
    {
        if (node == nullptr) 
        {
            return;
        }
        clearBST(node->left);
        
        clearBST(node->right);
        
        delete node;
    }

    
    
    PasswordNode* minValueNode(PasswordNode* node) 
    {
        PasswordNode* current = node;
        while (current && current->left != nullptr) 
        {
            current = current->left;
        }
        return current;
    }

    
    
    PasswordNode* deleteNode(PasswordNode* root, const char* service) 
    {
        if (root == nullptr) 
        {
            return root;
        }

        if (strcmp(service, root->serviceName) < 0) 
        {
            root->left = deleteNode(root->left, service);
        }
        else if (strcmp(service, root->serviceName) > 0) 
        {
            root->right = deleteNode(root->right, service);
        } 
        else 
        {
            if (root->left == nullptr) 
            {
                PasswordNode* temp = root->right;
                delete root;
                return temp;
            } 
            else if (root->right == nullptr) 
            {
                PasswordNode* temp = root->left;
                delete root;
                return temp;
            }

            
            
            PasswordNode* temp = minValueNode(root->right);

            strcpy(root->serviceName, temp->serviceName);
            
            strcpy(root->encryptedPassword, temp->encryptedPassword);
            
            root->xorKey = temp->xorKey;

            root->right = deleteNode(root->right, temp->serviceName);
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
        PasswordNode* newNode = new PasswordNode();
        char rawPassword[50];

        cout << "Enter Service Name: ";
        cin.ignore();
        cin.getline(newNode->serviceName, 50);

        cout << "Enter Password: ";
        cin.getline(rawPassword, 50);

        newNode->xorKey = (rand() % 32);

        xorProcess(rawPassword, newNode->encryptedPassword, newNode->xorKey);
        root = insert(root, newNode);
        cout << "Password saved successfully!\n";
    }

    void searchPassword() 
    {
        if(root == nullptr)
        {
            return;
        
        }
        char service[50];
        cout << "Enter Service Name to retrieve password: ";
        cin.ignore();
        cin.getline(service, 50);

        PasswordNode* result = search(root, service);
        if (result) 
        {
            char decrypted[50];
            xorProcess(result->encryptedPassword, decrypted, result->xorKey);
            cout << "Service: " << result->serviceName << "\nPassword: " << decrypted << endl;
        } 
        else 
        {
            cout << "Service not found.\n";
        }
    }

    void displayAll() 
    {
        if (root == nullptr) 
        {
            cout << "No Services available.\n";
            return;
        }
        cout << "--- Managed Services ---\n";
        inOrder(root);
        cout << "------------------------\n";
    }

    void removePassword() 
    {
        if (root == nullptr) 
        {
            return;
        }

        char service[50];
        cout << "Enter Service Name to delete: ";
        cin.ignore();
        cin.getline(service, 50);

        if (search(root, service) != nullptr) 
        {
            root = deleteNode(root, service);
            cout << "Service '" << service << "' deleted successfully.\n";
        } 
        else 
        {
            cout << "Service not found.\n";
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

        PasswordNode tempNode;
        while (inFile.read(reinterpret_cast<char*>(&tempNode), sizeof(PasswordNode))) 
        {
            PasswordNode* newNode = new PasswordNode();
        
            strcpy(newNode->serviceName, tempNode.serviceName);
        
            strcpy(newNode->encryptedPassword, tempNode.encryptedPassword);
        
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
        cout << "2. Retrive Service and Password\n";
        cout << "3. Display All Services\n";
        cout << "4. Delete Password\n";
        cout << "5. Exit\n";
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
                cout << "Saving data and exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
        manager.saveToFile();
    } while (choice != 5);

    return 0;
}