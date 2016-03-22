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
    // Here, "auto" is type "const std::experimental::filesystem::v1::recursive_directory_iterator::value_type"
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path())) {
        // Test code for now. Print out current directory.
        cout << p << endl;
    }
}