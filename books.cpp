#include "books.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void listBooks() {
    auto books = readCSV("data/books.csv");
    cout << "\n--- BOOK LIST ---\n";
    for (auto b : books) {
        cout << b[0] << " | " << b[1]
             << " | Author: " << b[2]
             << " | Copies: " << b[5] << endl;
    }
}

void addBook() {
    string c,t,a,cat,y,cp;
    cout << "Code: "; cin >> c;
    cin.ignore();
    cout << "Title: "; getline(cin, t);
    cout << "Author: "; getline(cin, a);
    cout << "Category: "; getline(cin, cat);
    cout << "Year: "; cin >> y;
    cout << "Copies: "; cin >> cp;

    auto books = readCSV("data/books.csv");
    books.push_back({c,t,a,cat,y,cp});
    writeCSV("data/books.csv", books);
    cout << "Book added successfully.\n";
}

void editBook() {
    string code;
    cout << "Enter Book Code to edit: ";
    cin >> code;

    auto books = readCSV("data/books.csv");
    bool found = false;
    for (auto &b : books) {
        if (b[0] == code) {
            cin.ignore();
            cout << "Title (" << b[1] << "): "; getline(cin, b[1]);
            cout << "Author (" << b[2] << "): "; getline(cin, b[2]);
            cout << "Category (" << b[3] << "): "; getline(cin, b[3]);
            cout << "Year (" << b[4] << "): "; cin >> b[4];
            cout << "Copies (" << b[5] << "): "; cin >> b[5];
            found = true;
            break;
        }
    }
    if(found) {
        writeCSV("data/books.csv", books);
        cout << "Book updated successfully.\n";
    } else {
        cout << "Book not found\n";
    }
}

void deleteBook() {
    string code;
    cout << "Enter Book Code to delete: ";
    cin >> code;

    auto books = readCSV("data/books.csv");
    vector<vector<string>> updated;
    for (auto b : books)
        if (b[0] != code)
            updated.push_back(b);

    if (updated.size() == books.size()) {
        cout << "Book not found.\n";
    } else {
        writeCSV("data/books.csv", updated);
        cout << "Book deleted successfully.\n";
    }
}
