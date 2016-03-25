#include <iostream>
#include <fstream>
#include <string>
#include <experimental\filesystem>
#include "Repository.h"

using namespace std;
namespace filesystem = std::experimental::filesystem;

Repository::Repository() {
    if (filesystem::exists(mRepositoryFolderName)) {
        cout << "Repository already exists" << endl;
    }
    else {
        CreateRepository(mRepositoryFolderName); // Creates repository folder with name specified in header file.
        cout << "Repository created." << endl;
        CreateProjectTree();
        CreateManifest();
    }
}

void Repository::CreateRepository(const string s) {
    // Create the directory.
    filesystem::create_directory(s);
    // Create the manifest.
    filesystem::create_directory(s + "\\manifests");
}

void Repository::CreateProjectTree() {

    filesystem::path currentPath = filesystem::current_path();
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }

    for (auto &p : filesystem::recursive_directory_iterator(currentPath)) {
        //cout << p.path() << endl;
        if (filesystem::is_regular_file(p)) {
            string filePath = p.path().string(); // Location of where the repository is stored.
            int cutOffLocation = filePath.find(currentDirectory) + currentDirectory.length(); // Total length of path containing repository.
            string destination = filePath.substr(0, cutOffLocation) + "\\" + mRepositoryFolderName + filePath.substr(cutOffLocation);
            filesystem::create_directories(destination); // Create folders for the files in the repository.
            string checkSum = CheckSum(filePath); // Create checksum for that file.
            destination = destination + "\\" + checkSum; // Update destination to include checksum. This will rename the file to that checksum.
            filesystem::copy_file(filePath, destination, filesystem::copy_options::overwrite_existing); // Copies over the file to its respective repository folder.
        }
    }
    filesystem::remove_all(mRepositoryFolderName + "\\" + mRepositoryFolderName); //removes duplicate repo folders due to recursion
}

void Repository::CreateManifest() {
    string manifestLocation = mRepositoryFolderName + "\\manifests\\manfile.txt";
    ofstream output(manifestLocation);
    char date[9];

    // Initial Manifest file writing - path & time
    output << "Project Tree Path :" << filesystem::current_path() << endl;
    output << "Check-in date: " << _strdate(date) << endl;

    //recursive directory iterator
    output << "Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path().string() + "//" + mRepositoryFolderName)) {

        if (filesystem::is_regular_file(p) && p.path().filename() != "manfile.txt") {
            //prints filename alongside its artifact id. excludes the manfile.txt
            output << p.path().parent_path().filename() << " | AID: " << p.path().filename() << endl;
        }
    }
    output.close();
}

string Repository::CheckSum(string path) {
    FILE *file = NULL;
    fopen_s(&file, path.c_str(), "rb");  // opens file stream
    fseek(file, 0, SEEK_END);            // sets the file pointer to end of file
    int size = ftell(file);              // ftell gets the file size

    fclose(file);                        // close stream and release buffer
    return to_string(size);
}