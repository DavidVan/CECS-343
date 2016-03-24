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


    //PathType can be used to check whether a dir exists. 
    //This checks whether drm repo was already created.
    /*
    if (PathType(dir_name) != 0) {
        filesystem::copy(currentPath, dir_name, filesystem::copy_options::recursive);
        cout << "Repo Created at: " << dir_name << endl;
    }
    else
        cout << "Repo Already Exists At: " << dir_name << endl;
    */


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
            //when a file is found, make a folder and put file in that folder
            if (PathType(p.path().string()) == 1) {
                //cout << "filename: " << p.path().filename() << "parent path: " << p.path().parent_path() << endl;
                //filesystem::create_directory(p.path().parent_path(), p);
            }
        }
    }
}

int Repository::CheckSum(string path) {
    FILE *file = NULL;
    fopen_s(&file, path.c_str(), "rb");  // opens file stream
    fseek(file, 0, SEEK_END);            // sets the file pointer to end of file
    int size = ftell(file);              // ftell gets the file size

    fclose(file);                        // close stream and release buffer
    return size;
}

int Repository::PathType(string path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFREG)
        {
            return 1; //is a file
        }
        else if (s.st_mode & S_IFDIR){
            return 0; //is a folder
        } 
    }
    return -1;
}
