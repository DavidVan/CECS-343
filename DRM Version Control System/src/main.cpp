#include <iostream>
#include <string>
#include "Repository.h"

using namespace std;

void commandList();
void runProg(int, char*[]);
int main(int argc, char* argv[]) {
	cout << "Hello World!" << endl;
	//cout << argv[1] << endl;
	Repository h;
	if (argc < 2 || argc > 8) {//eventual input size validation
		cout << "Invalid input";
		return 0;
	}
	runProg(argc, argv);
	cin.get();
}
void runProg(int argc, char* argv[]) {
	if (string(argv[1]) != "drm") {
		commandList();
		return;
	}
	string a = "add";
	if (string(argv[2]) == "add")
		cout << "ADD";//runs the add function to be put in later
	else if (string(argv[2]) == "create")
		cout << "CREATE";//runs the add function to be put in later
	else if (string(argv[2]) == "delete")
		cout << "DELETE";//runs the delete function
	else if (string(argv[2]) == "remove")
		cout << "REMOVE";//runs the remove function
	else if (string(argv[2]) == "rename")
		cout << "RENAME";//runs the rename function
	else
		commandList();
};
void commandList() {
	cout << "List of available commands:" << endl <<
		"help" << endl <<
		"add" << endl <<
		"create" << endl <<
		"delete" << endl <<
		"remove" << endl <<
		"rename" << endl;
}
