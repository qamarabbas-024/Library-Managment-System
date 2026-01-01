#include "auth.h"
#include "utils.h"
#include <iostream>
#include <vector>

using namespace std;

void signup() {
    clearScreen();
    string email, username, phone, password;
    auto users = readCSV("data/users.csv");

    cout << "--- SIGNUP ---\n";
    cout << "Email: "; cin >> email;
    for (auto u : users)
        if (u[3] == email) {
            cout << "Email already exists\n";
            return;
        }

    cout << "Username: "; cin >> username;
    for (auto u : users)
        if (u[1] == username) {
            cout << "Username already exists\n";
            return;
        }

    cout << "Phone: "; cin >> phone;
    cout << "Password: "; cin >> password;

    users.push_back({
        "UID" + to_string(users.size() + 1),
        username, password, email, phone,
        "member", "active"
    });

    writeCSV("data/users.csv", users);
    cout << "Signup successful\n";
}

string login(string &userID) {
    clearScreen();
    string u, p;
    cout << "--- LOGIN ---\n";
    cout << "Username: "; cin >> u;
    cout << "Password: "; cin >> p;

    auto users = readCSV("data/users.csv");
    for (auto user : users) {
        if (user[1] == u && user[2] == p) {
            if (user[6] == "banned") {
                cout << "Account banned\n";
                return "";
            }
            userID = user[0];
            return user[5]; // return role
        }
    }
    cout << "Invalid login\n";
    return "";
}

void forgotPassword() {
    clearScreen();
    string email;
    cout << "--- FORGOT PASSWORD ---\n";
    cout << "Email: "; cin >> email;

    auto users = readCSV("data/users.csv");
    for (auto &u : users) {
        if (u[3] == email) {
            cout << "New Password: ";
            cin >> u[2];
            writeCSV("data/users.csv", users);
            cout << "Password updated\n";
            return;
        }
    }
    cout << "Email not found\n";
}
