#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// --------------------------- Utility ----------------------------
void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

// --------------------------- Classes ----------------------------
class Book {
public:
    string isbn, title, author, category;
    int year, totalCopies, availableCopies;

    Book() : year(0), totalCopies(0), availableCopies(0) {}

    string toCSV() {
        return isbn + "," + title + "," + author + "," + category + "," +
               to_string(year) + "," + to_string(totalCopies) + "," + to_string(availableCopies);
    }

    void fromCSV(const string& line) {
        stringstream ss(line);
        string token;
        getline(ss, isbn, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, category, ',');
        getline(ss, token, ','); year = stoi(token);
        getline(ss, token, ','); totalCopies = stoi(token);
        getline(ss, token, ','); availableCopies = stoi(token);
    }
};

class Member {
public:
    string memberId, name, contact, email;
    bool banned;

    Member() : banned(false) {}

    string toCSV() {
        return memberId + "," + name + "," + contact + "," + email + "," + (banned ? "1" : "0");
    }

    void fromCSV(const string& line) {
        stringstream ss(line);
        string token;
        getline(ss, memberId, ',');
        getline(ss, name, ',');
        getline(ss, contact, ',');
        getline(ss, email, ',');
        getline(ss, token, ','); banned = (token == "1");
    }
};

class Loan {
public:
    string loanId, bookIsbn, memberId, issueDate, dueDate, returnDate;
    double fine;

    Loan() : fine(0.0) {}

    string toCSV() {
        return loanId + "," + bookIsbn + "," + memberId + "," + issueDate + "," + dueDate + "," + returnDate + "," + to_string(fine);
    }

    void fromCSV(const string& line) {
        stringstream ss(line);
        string token;
        getline(ss, loanId, ',');
        getline(ss, bookIsbn, ',');
        getline(ss, memberId, ',');
        getline(ss, issueDate, ',');
        getline(ss, dueDate, ',');
        getline(ss, returnDate, ',');
        getline(ss, token, ','); fine = stod(token);
    }
};

// --------------------------- Global Vectors ----------------------------
vector<Book> books;
vector<Member> members;
vector<Loan> loans;

// --------------------------- File I/O ----------------------------
void loadBooks() {
    books.clear();
    ifstream file("books.csv");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Book b; b.fromCSV(line);
        books.push_back(b);
    }
    file.close();
}

void saveBooks() {
    ofstream file("books.csv");
    for (auto &b : books) file << b.toCSV() << "\n";
}

void loadMembers() {
    members.clear();
    ifstream file("members.csv");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Member m; m.fromCSV(line);
        members.push_back(m);
    }
    file.close();
}

void saveMembers() {
    ofstream file("members.csv");
    for (auto &m : members) file << m.toCSV() << "\n";
}

void loadLoans() {
    loans.clear();
    ifstream file("loans.csv");
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Loan l; l.fromCSV(line);
        loans.push_back(l);
    }
    file.close();
}

void saveLoans() {
    ofstream file("loans.csv");
    for (auto &l : loans) file << l.toCSV() << "\n";
}

// --------------------------- Book Functions ----------------------------
void addBook() {
    clearScreen();
    Book b;
    cout << "Enter ISBN: "; cin >> b.isbn;
    cout << "Enter Title: "; cin.ignore(); getline(cin, b.title);
    cout << "Enter Author: "; getline(cin, b.author);
    cout << "Enter Category: "; getline(cin, b.category);
    cout << "Enter Year: "; cin >> b.year;
    cout << "Enter Total Copies: "; cin >> b.totalCopies;
    b.availableCopies = b.totalCopies;

    books.push_back(b);
    saveBooks();
    cout << "Book added successfully!\n"; system("pause");
}

void listBooks() {
    clearScreen();
    cout << "----- Books List -----\n";
    for (auto &b : books) {
        cout << b.isbn << " | " << b.title << " | " << b.author << " | "
             << b.category << " | Year: " << b.year << " | Available: " << b.availableCopies << "\n";
    }
    cout << "---------------------\n"; system("pause");
}

void editBook() {
    clearScreen();
    string isbn;
    cout << "Enter ISBN of book to edit: "; cin >> isbn;
    auto it = find_if(books.begin(), books.end(), [&](Book &b){ return b.isbn == isbn; });
    if (it == books.end()) { cout << "Book not found!\n"; system("pause"); return; }

    cout << "Editing book: " << it->title << "\n";
    cin.ignore();
    cout << "Enter new Title: "; getline(cin, it->title);
    cout << "Enter new Author: "; getline(cin, it->author);
    cout << "Enter new Category: "; getline(cin, it->category);
    cout << "Enter new Year: "; cin >> it->year;
    cout << "Enter new Total Copies: "; cin >> it->totalCopies;
    it->availableCopies = it->totalCopies;
    saveBooks();
    cout << "Book updated successfully!\n"; system("pause");
}

void deleteBook() {
    clearScreen();
    string isbn;
    cout << "Enter ISBN of book to delete: "; cin >> isbn;
    auto it = remove_if(books.begin(), books.end(), [&](Book &b){ return b.isbn == isbn; });
    if (it != books.end()) {
        books.erase(it, books.end());
        saveBooks();
        cout << "Book deleted successfully!\n";
    } else {
        cout << "Book not found!\n";
    }
    system("pause");
}

// --------------------------- Member Functions ----------------------------
void addMember() {
    clearScreen();
    Member m;
    cout << "Enter Member ID: "; cin >> m.memberId;
    cout << "Enter Name: "; cin.ignore(); getline(cin, m.name);
    cout << "Enter Contact: "; getline(cin, m.contact);
    cout << "Enter Email: "; getline(cin, m.email);
    m.banned = false;

    members.push_back(m);
    saveMembers();
    cout << "Member added successfully!\n"; system("pause");
}

void listMembers() {
    clearScreen();
    cout << "----- Members List -----\n";
    for (auto &m : members) {
        cout << m.memberId << " | " << m.name << " | " << m.contact << " | " << m.email
             << " | Status: " << (m.banned ? "Banned" : "Active") << "\n";
    }
    cout << "-----------------------\n"; system("pause");
}

void editMember() {
    clearScreen();
    string id;
    cout << "Enter Member ID to edit: "; cin >> id;
    auto it = find_if(members.begin(), members.end(), [&](Member &m){ return m.memberId == id; });
    if (it == members.end()) { cout << "Member not found!\n"; system("pause"); return; }

    cin.ignore();
    cout << "Enter new Name: "; getline(cin, it->name);
    cout << "Enter new Contact: "; getline(cin, it->contact);
    cout << "Enter new Email: "; getline(cin, it->email);
    saveMembers();
    cout << "Member updated successfully!\n"; system("pause");
}

void deleteMember() {
    clearScreen();
    string id;
    cout << "Enter Member ID to delete: "; cin >> id;
    auto it = remove_if(members.begin(), members.end(), [&](Member &m){ return m.memberId == id; });
    if (it != members.end()) {
        members.erase(it, members.end());
        saveMembers();
        cout << "Member deleted successfully!\n";
    } else {
        cout << "Member not found!\n";
    }
    system("pause");
}

void banMember() {
    clearScreen();
    string id;
    cout << "Enter Member ID to ban/unban: "; cin >> id;
    auto it = find_if(members.begin(), members.end(), [&](Member &m){ return m.memberId == id; });
    if (it == members.end()) { cout << "Member not found!\n"; system("pause"); return; }

    it->banned = !it->banned;
    saveMembers();
    cout << "Member " << (it->banned ? "banned" : "unbanned") << " successfully!\n"; system("pause");
}

// --------------------------- Issue / Return ----------------------------
void issueBook() {
    clearScreen();
    string memberId, isbn;
    cout << "Enter Member ID: "; cin >> memberId;
    cout << "Enter Book ISBN: "; cin >> isbn;

    auto itM = find_if(members.begin(), members.end(), [&](Member &m){ return m.memberId == memberId; });
    auto itB = find_if(books.begin(), books.end(), [&](Book &b){ return b.isbn == isbn; });

    if (itM == members.end()) { cout << "Member not found!\n"; system("pause"); return; }
    if (itM->banned) { cout << "Member is banned! Cannot issue books.\n"; system("pause"); return; }
    if (itB == books.end()) { cout << "Book not found!\n"; system("pause"); return; }
    if (itB->availableCopies <= 0) { cout << "No copies available!\n"; system("pause"); return; }

    Loan l;
    l.loanId = "L" + to_string(loans.size() + 1);
    l.bookIsbn = isbn;
    l.memberId = memberId;
    l.issueDate = "2025-10-30"; // placeholder
    l.dueDate = "2025-11-06";   // placeholder
    l.returnDate = "";
    l.fine = 0.0;

    itB->availableCopies--;
    loans.push_back(l);
    saveBooks(); saveLoans();
    cout << "Book issued successfully!\n"; system("pause");
}

void returnBook() {
    clearScreen();
    string loanId;
    cout << "Enter Loan ID: "; cin >> loanId;

    auto itL = find_if(loans.begin(), loans.end(), [&](Loan &l){ return l.loanId == loanId; });
    if (itL == loans.end()) { cout << "Loan not found!\n"; system("pause"); return; }
    if (!itL->returnDate.empty()) { cout << "Book already returned!\n"; system("pause"); return; }

    itL->returnDate = "2025-11-01"; // placeholder
    auto itB = find_if(books.begin(), books.end(), [&](Book &b){ return b.isbn == itL->bookIsbn; });
    if (itB != books.end()) itB->availableCopies++;

    saveBooks(); saveLoans();
    cout << "Book returned successfully!\n"; system("pause");
}

// --------------------------- Reports ----------------------------
void listLoans() {
    clearScreen();
    cout << "----- Loans List -----\n";
    for (auto &l : loans) {
        cout << l.loanId << " | Book ISBN: " << l.bookIsbn << " | Member ID: " << l.memberId
             << " | Issue: " << l.issueDate << " | Due: " << l.dueDate
             << " | Return: " << (l.returnDate.empty() ? "Not returned" : l.returnDate)
             << " | Fine: " << l.fine << "\n";
    }
    cout << "---------------------\n"; system("pause");
}

// --------------------------- Group Info ----------------------------
void displayGroupInfo() {
    clearScreen();
    cout << "===== Group Information =====\n";
    cout << "Group Name: A\n";
    cout << "Members:\n";
    cout << "1. QAMAR ABBAS        (25014119-024)\n";
    cout << "2. MUHAMMAD HAMMAS    (25014119-164)\n";
    cout << "3. Khuzama Aleem      (25014119-053)\n";
    cout << "4. SYED SAFI HADIER   (25014119-209)\n";
    cout << "5. BUSHRA BUTT        (25014119-216)\n";
    cout << "6. LEEZA              (25014119-232)\n";
    cout << "----------------------------\n";
    system("pause");
}

// --------------------------- Main Menu ----------------------------
void mainMenu() {
    loadBooks(); loadMembers(); loadLoans();
    int choice;
    do {
        clearScreen();
        cout << "===== Library Management System =====\n";
        cout << "1. Manage Books\n";
        cout << "2. Manage Members\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Reports (Loans)\n";
        cout << "6. About Group\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: "; cin >> choice;

        switch(choice) {
            case 1: {
                int ch;
                do {
                    clearScreen();
                    cout << "----- Manage Books -----\n";
                    cout << "1. Add Book\n2. List Books\n3. Edit Book\n4. Delete Book\n5. Back\n";
                    cout << "Enter choice: "; cin >> ch;
                    switch(ch) {
                        case 1: addBook(); break;
                        case 2: listBooks(); break;
                        case 3: editBook(); break;
                        case 4: deleteBook(); break;
                    }
                } while(ch != 5);
                break;
            }
            case 2: {
                int ch;
                do {
                    clearScreen();
                    cout << "----- Manage Members -----\n";
                    cout << "1. Add Member\n2. List Members\n3. Edit Member\n4. Delete Member\n5. Ban/Unban Member\n6. Back\n";
                    cout << "Enter choice: "; cin >> ch;
                    switch(ch) {
                        case 1: addMember(); break;
                        case 2: listMembers(); break;
                        case 3: editMember(); break;
                        case 4: deleteMember(); break;
                        case 5: banMember(); break;
                    }
                } while(ch != 6);
                break;
            }
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: listLoans(); break;
            case 6: displayGroupInfo(); break;
        }
    } while(choice != 7);
    cout << "Exiting... Goodbye!\n";
}

// --------------------------- Main ----------------------------
int main() {
    mainMenu();
    return 0;
}
