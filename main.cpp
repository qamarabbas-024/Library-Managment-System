#include <iostream>
#include "utils.h"
#include "auth.h"
#include "books.h"
#include "members.h"
#include "loans.h"

using namespace std;

void adminMenu() {
    int ch;
    do{
        clearScreen();
        cout << "--- ADMIN MENU ---\n";
        cout << "1. Add Book\n2. Edit Book\n3. Delete Book\n4. List Books\n";
        cout << "5. Add Member\n6. Edit Member\n7. Delete Member\n";
        cout << "8. List Members\n9. Ban Member\n10. Unban Member\n";
        cout << "11. Approve Loans\n12. Member Activities\n0. Back\nChoice: ";
        cin >> ch;

        clearScreen();
        switch(ch) {
            case 1: addBook(); break;
            case 2: editBook(); break;
            case 3: deleteBook(); break;
            case 4: listBooks(); break;
            case 5: addMember(); break;
            case 6: editMember(); break;
            case 7: deleteMember(); break;
            case 8: listMembers(); break;
            case 9: banMember(); break;
            case 10: unbanMember(); break;
            case 11: approveLoans(); break;
            case 12: memberActivities(); break;
        }

        if(ch!=0){ cout << "\nPress Enter..."; cin.ignore(); cin.get(); }

    } while(ch!=0);
}

void memberMenu(string uid){
    int ch;
    do{
        clearScreen();
        cout << "--- MEMBER MENU ---\n";
        cout << "1. View Books\n2. Apply Loan\n3. Update Profile\n4. Return Book\n0. Back\nChoice: ";
        cin >> ch;
        clearScreen();
        switch(ch) {
            case 1: listBooks(); break;
            case 2: applyLoan(uid); break;
            case 3: updateMemberInfo(uid); break;
            case 4: returnBook(uid); break;
        }
        if(ch!=0){ cout << "\nPress Enter..."; cin.ignore(); cin.get(); }
    }while(ch!=0);
}

int main(){
    int choice;
    string role, uid;

    do{
        clearScreen();
        cout << "=== LIBRARY MANAGEMENT SYSTEM ===\n";
        cout << "1. Login\n2. Signup\n3. Forgot Password\n0. Exit\nChoice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                role=login(uid);
                if(role=="admin") adminMenu();
                else if(role=="member") memberMenu(uid);
                break;
            case 2: signup(); cin.ignore(); cin.get(); break;
            case 3: forgotPassword(); cin.ignore(); cin.get(); break;
        }

    } while(choice!=0);

    clearScreen();
    cout << "Project By:\nQamar Abbas\nSafi Haider\nLeeza Shehzadi\n";
    return 0;
}
