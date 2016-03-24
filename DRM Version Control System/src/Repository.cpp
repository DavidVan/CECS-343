#include <iostream>
#include <string>
#include <experimental\filesystem>
#include "Repository.h"

using namespace std;
namespace filesystem = std::experimental::filesystem;

Repository::Repository() {
    CreateRepository(".drm");
    CreateProjectTree();
}

void Repository::CreateRepository(const string s) {
    // Create the directory.
    filesystem::create_directory(s);
}

void Repository::CreateProjectTree() {
    filesystem::path currentPath = filesystem::current_path();
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }
    cout << currentDirectory << endl;
    // Here, "auto" is type "const std::experimental::filesystem::v1::recursive_directory_iterator::value_type"
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path())) {
        // Test code for now. Print out current directory.
        string path = p.path().parent_path().string();
        //cout << p.path().filename() << endl;
        //cout << path.substr(path.find(currentDirectory) + currentDirectory.length()) << endl;
    }
    for (auto &p : filesystem::directory_iterator(filesystem::current_path())) {
        cout << p.path().filename() << endl;
        filesystem::path filename = p.path().filename();
        if (filesystem::is_regular_file(filename)) {
            cout << "This is a file!" << endl;
        }
        else if (filesystem::is_directory(filename)) {
            cout << "This is a folder!" << endl;
        }
        else {
            cout << "This is neither a file nor a folder!" << endl;
        }
    }
}