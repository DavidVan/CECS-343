#include <iostream>
#include <string>
#include <experimental\filesystem>
#include <fstream>
#include "Repository.h"


using namespace std;
namespace filesystem = std::experimental::filesystem;

Repository::Repository() {
    if (filesystem::exists("repo343")) {
        cout << "Repository is already created." << endl;
    }
    else {
        CreateRepository("repo343");
        cout << "Repository is created." << endl;
        CreateProjectTree();
        CreateManifest();
    }
}

void Repository::CreateRepository(const string s) {
    // Create the directory.
    filesystem::create_directory(s);
    filesystem::create_directory(s + "\\manifests");                 // Create Manifest Subdirectory

}

void Repository::CreateProjectTree() {

    filesystem::path currentPath = filesystem::current_path();       //grabs cd location

    //for loop that extracts the current directory without the full path
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }

    //this is a recursive iterator that will go through all the files/folders of the cd
    for (auto &p : filesystem::recursive_directory_iterator(currentPath)) {
        if (filesystem::is_regular_file(p)) {
            string path = p.path().string();                                           //string rep of referenced path
            int loc = path.find(currentDirectory) + currentDirectory.length();         //position of the cd name in the path
            string destination = path.substr(0, loc) + "\\repo343" + path.substr(loc); //path of new files in .drm

            filesystem::create_directories(destination);                               //directories are created in .drm if it doesnt exist
            string csum = CheckSum(path);                                              //calculates checksum of the file
            destination = destination + "\\" + csum;                                   //updates destination of the copy with checksum as the file name
            filesystem::copy_file(path, destination);                                  //will create a copy of the file in its respective folder inside .drm
        } 
    }
    filesystem::remove_all("repo343\\repo343"); //removes duplicate repo folders due to recursion
}

string Repository::CheckSum(string path) {
    FILE *file = NULL;
    fopen_s(&file, path.c_str(), "rb");  // opens file stream
    fseek(file, 0, SEEK_END);            // sets the file pointer to end of file
    int size = ftell(file);              // ftell gets the file size

    fclose(file);                        // close stream and release buffer
    return to_string(size);
}

void Repository::CreateManifest() {
    ofstream output(".\\repo343\\manifests\\manfile.txt");   
    char date[9];

    // Initial Manifest file writing - path & time
    output << "ProjectTree Path :" << filesystem::current_path() << endl;
    output << "Check in date: " << _strdate(date) << endl;

    //recursive directory iterator
    output << "Ptree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path()/"repo343")) {
        
        if (filesystem::is_regular_file(p) && p.path().filename()!="manfile.txt") {
            //prints filename alongside its artifact id. excludes the manfile.txt
            output << p.path().parent_path().filename() <<" | AID: "<< p.path().filename() <<endl;
        }
    }
    output.close();
}



