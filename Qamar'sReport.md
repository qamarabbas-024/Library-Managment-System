In this part of the project, I created a basic module that contains common functions. The purpose of this module is to keep the main function clean and to avoid repeating the same code again and again.

First, I created a header file named utils.h. In this file, i declared some basic functions such as clearing the screen, reading CSV files, writing CSV files, and splitting CSV data. These functions are only declared in the header file so that they can be accessed from the main program by including this file.

All the main header files are also included in this File utils.h.

In the second file utils.cpp i defined all the function.

---------------------------------------------------------------------------------------------------
Then i created the member.h file, where i declared some member functions(add,del,list,update,ban,unban)
this section will handle all the member details in the main function and in the program.