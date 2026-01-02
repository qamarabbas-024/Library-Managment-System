#include "loans.h"
#include "utils.h"
#include "books.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void applyLoan(const string &uid) {
    string book;
    cout << "Enter Book Code to borrow: "; cin >> book;

    auto loans = readCSV("data/loans.csv");
    auto books = readCSV("data/books.csv");
    bool available=false;

    for(auto &b: books)
        if(b[0]==book && stoi(b[5])>0) available=true;

    if(!available){
        cout << "Book not available.\n";
        return;
    }

    loans.push_back({
        "L" + to_string(loans.size()+1),
        uid, book, "pending"
    });
    writeCSV("data/loans.csv", loans);
    cout << "Loan request submitted.\n";
}
void returnBook(const string &uid){
    auto loans = readCSV("data/loans.csv");
    auto books = readCSV("data/books.csv");
    vector<string> myLoans;

    cout << "\n--- YOUR BORROWED BOOKS ---\n";
    for(auto &l : loans){
        if(l[1]==uid && l[3]=="approved"){
            string bookTitle="";
            for(auto &b : books)
                if(b[0]==l[2]) bookTitle=b[1];
            cout << l[0] << " | " << bookTitle << endl;
            myLoans.push_back(l[0]);
        }
    }

    if(myLoans.empty()){
        cout << "No approved loans found.\n";
        return;
    }

    string loanID;
    cout << "Enter Loan ID to return: ";
    cin >> loanID;

    bool found=false;
    for(auto &l: loans){
        if(l[0]==loanID && l[1]==uid && l[3]=="approved"){
            l[3] = "returned";
            for(auto &b: books)
                if(b[0]==l[2])
                    b[5] = to_string(stoi(b[5])+1);
            found=true;
            break;
        }
    }

    if(found){
        writeCSV("data/loans.csv", loans);
        writeCSV("data/books.csv", books);
        cout << "Book returned successfully.\n";
    } else {
        cout << "Invalid Loan ID.\n";
    }
}

void approveLoans() {
    auto loans = readCSV("data/loans.csv");
    for(auto &l: loans){
        if(l[3]=="pending"){
            cout << "Approve Loan " << l[0] << " (y/n): ";
            char c; cin >> c;
            l[3] = (c=='y')?"approved":"rejected";

            if(l[3]=="approved"){
                auto books = readCSV("data/books.csv");
                for(auto &b: books)
                    if(b[0]==l[2])
                        b[5] = to_string(stoi(b[5])-1);
                writeCSV("data/books.csv", books);
            }
        }
    }
    writeCSV("data/loans.csv", loans);
}

void memberActivities() {
    auto loans = readCSV("data/loans.csv");
    auto users = readCSV("data/users.csv");
    auto books = readCSV("data/books.csv");

    cout << "\n--- MEMBER ACTIVITIES ---\n";
    for(auto l: loans){
        string memberName="", bookTitle="";
        for(auto u: users) if(u[0]==l[1]) memberName=u[1];
        for(auto b: books) if(b[0]==l[2]) bookTitle=b[1];
        cout << l[0] << " | " << memberName << " | " << bookTitle << " | " << l[3] << endl;
    }
}
