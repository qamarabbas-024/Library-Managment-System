#include "members.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void listMembers() {
    auto users = readCSV("data/users.csv");
    cout << "\n--- MEMBERS ---\n";
    for (auto u : users)
        if (u[5] == "member")
            cout << u[0] << " | " << u[1] << " | Status: " << u[6] << endl;
}

void addMember() {
    string username,email,phone,password;
    cout << "Username: "; cin >> username;
    cout << "Email: "; cin >> email;
    cout << "Phone: "; cin >> phone;
    cout << "Password: "; cin >> password;

    auto users = readCSV("data/users.csv");
    users.push_back({
        "UID" + to_string(users.size()+1),
        username,password,email,phone,
        "member","active"
    });
    writeCSV("data/users.csv", users);
    cout << "Member added successfully.\n";
}

void editMember() {
    string uid;
    cout << "Enter Member UID to edit: "; cin >> uid;
    auto users = readCSV("data/users.csv");
    bool found = false;

    for(auto &u: users){
        if(u[0]==uid && u[5]=="member"){
            cout << "Username (" << u[1] << "): "; cin >> u[1];
            cout << "Email (" << u[3] << "): "; cin >> u[3];
            cout << "Phone (" << u[4] << "): "; cin >> u[4];
            cout << "Password (" << u[2] << "): "; cin >> u[2];
            found = true;
            break;
        }
    }
    if(found) {
        writeCSV("data/users.csv", users);
        cout << "Member updated successfully.\n";
    } else {
        cout << "Member not found\n";
    }
}

void deleteMember() {
    string uid;
    cout << "Enter Member UID to delete: "; cin >> uid;
    auto users = readCSV("data/users.csv");
    vector<vector<string>> updated;
    for(auto u: users){
        if(!(u[0]==uid && u[5]=="member"))
            updated.push_back(u);
    }
    writeCSV("data/users.csv", updated);
    cout << "Member deleted if existed.\n";
}

void banMember() {
    string uid; cout << "Enter Member UID to ban: "; cin >> uid;
    auto users = readCSV("data/users.csv");
    for(auto &u: users)
        if(u[0]==uid && u[5]=="member") u[6]="banned";
    writeCSV("data/users.csv", users);
    cout << "Member banned if existed.\n";
}

void unbanMember() {
    string uid; cout << "Enter Member UID to unban: "; cin >> uid;
    auto users = readCSV("data/users.csv");
    for(auto &u: users)
        if(u[0]==uid && u[5]=="member") u[6]="active";
    writeCSV("data/users.csv", users);
    cout << "Member unbanned if existed.\n";
}
void updateMemberInfo(const string &uid){
    auto users = readCSV("data/users.csv");
    for(auto &u : users){
        if(u[0]==uid && u[5]=="member"){
            cout << "Update Username (" << u[1] << "): "; cin >> u[1];
            cout << "Update Email (" << u[3] << "): "; cin >> u[3];
            cout << "Update Phone (" << u[4] << "): "; cin >> u[4];
            cout << "Update Password (" << u[2] << "): "; cin >> u[2];
            writeCSV("data/users.csv", users);
            cout << "Profile updated successfully.\n";
            return;
        }
    }
    cout << "Member not found!\n";
}
