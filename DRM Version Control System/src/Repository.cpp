#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <experimental\filesystem>
#include "Repository.h"

using namespace std;
namespace filesystem = std::experimental::filesystem;

Repository::Repository() : mRepositoryFolderName("repo343") {

}

Repository::~Repository() {

}

void Repository::Initialize() {
    if (filesystem::exists(mRepositoryFolderName)) {
        cout << "Repository already exists. Updating manifests. This will be changed later."<< endl;
    }
    else {
        CreateRepository(mRepositoryFolderName); // Creates repository folder with name specified in header file.
        cout << "Repository created." << endl;
    }
    CreateProjectTree();
    CreateManifest();
}

void Repository::CreateRepository(const string s) {
    // Create the directory.
    filesystem::create_directory(s);
    // Create the manifest.
    filesystem::create_directory(s + "\\manifests");
}

void Repository::CreateProjectTree() const {

    filesystem::path currentPath = filesystem::current_path();
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }

    // This will be the current path of the repository.
    string repositoryPath =  currentPath.string() + "\\" + mRepositoryFolderName;
    
    for (auto &p : filesystem::recursive_directory_iterator(currentPath)) {
        // We need to avoid adding our repositoy folder to our repository folder.
        // We find if the directory we're in contains the repository folder name.

        if (p.path().parent_path().string().find(repositoryPath) != string::npos) {
            continue;
        }
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
}

void Repository::CreateManifest() const {
    // Grab date/time.
    time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm *clock = localtime(&time);
    string dateString = "Check-in date: " + to_string(1 + clock->tm_mon) + "/" + to_string(clock->tm_mday) + "/" + to_string(1900 + clock->tm_year);
    string dateStamp = to_string(1 + clock->tm_mon) + "-" + to_string(clock->tm_mday) + "-" + to_string(1900 + clock->tm_year);
    string timeStamp = to_string(clock->tm_hour % 12) + "-" + to_string(clock->tm_min) + "-"
        + (clock->tm_sec >= 10 ? to_string(clock->tm_sec) : "0" + to_string(clock->tm_sec)) + (clock->tm_hour >= 12 ? "pm" : "am");

    // Open file for writing.
    string manifestLocation = mRepositoryFolderName + "\\manifests\\" + dateStamp + " at " + timeStamp + ".txt";
    ofstream output(manifestLocation);

    // Initial Manifest file writing - path & time
    output << "Project Tree Path :" << filesystem::current_path() << endl;
    output << "Check-in date: " << dateString << endl;

    output << "Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path().string() + "//" + mRepositoryFolderName)) {
        if (filesystem::is_regular_file(p) && p.path().filename() != "manfile.txt") {
            //prints filename alongside its artifact id. excludes the manfile.txt
            output << p.path().parent_path().filename() << " | AID: " << p.path().filename() << endl;
        }
    }
    output.close();
}

const string Repository::CheckSum(const string path) const {
    filesystem::path p = filesystem::canonical(path);
    return to_string(filesystem::file_size(p) % 256);
}