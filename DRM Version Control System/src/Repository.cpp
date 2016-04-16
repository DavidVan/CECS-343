#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
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
        cout << "Repository already exists. Updating manifests. This will be changed later." << endl;
    }
    else {
        CreateRepository(mRepositoryFolderName); // Creates repository folder with name specified in header file.
        cout << endl << "Repository created." << endl;
    }
    CreateProjectTree();
    CreateManifest();
}

/*
@Param: src = project tree folder ; target = repository folder
check in will update repository folder with new files/folders from the project tree directory.
Manifest will be updated as well.
*/
void Repository::CheckIn(string src, string target) {
    //iterate through ptree folder for files. 
    for (auto &p : filesystem::recursive_directory_iterator(src)) {
        string  filePath = p.path().string();
        if (filesystem::is_regular_file(p)) {
            if (filePath.find(mRepositoryFolderName) == string::npos) {	// excludes the cecs343 repo from the iteration
                bool isNewFile = true;
                // iterates through 343repo
                for (auto &r : filesystem::recursive_directory_iterator(target)) {
                    if (p.path().filename().string().compare(r.path().filename().string()) == 0) { // file already exists in repo. will update file in repo.
                        isNewFile = false;
                        string artifact = r.path().string() + "\\" + CheckSum(filePath);  // the theoretical file path of the updated file's artifact
                        if (!filesystem::exists(artifact)) {
                            filesystem::copy_file(filePath, artifact); // if artifact does not exist yet, copy that file in the repo's artifact folder.
                            cout << "File updated: " << p.path().filename() << endl;
                        }
                    }
                }
                // new file is found. create a new subdirectory and new artifact in the repo
                if (isNewFile) {
                    string newFilePath = GetRepoPath(p.path().string());
                    filesystem::create_directories(newFilePath); // Create folders for the files in the repository.
                    newFilePath = newFilePath + "\\" + CheckSum(filePath); // Update filepath to include checksum. This will rename the file to that checksum.
                    filesystem::copy_file(filePath, newFilePath); // Copies over the file to its respective repository folder.
                    cout << "New File is added to the repo: " << p.path().filename() << endl;
                }
            }
        }
    }
    CreateManifest(); // updates manifest file
}

void Repository::CheckOut(string src, string target) {
    cout << GetPrevManifest() << endl;
}

void Repository::CreateRepository(const string s) {
    // Create the directory.
    filesystem::create_directory(s);
    // Create the manifest.
    filesystem::create_directory(s + "\\manifests");
}

void Repository::CreateProjectTree() const {

    filesystem::path currentPath = filesystem::current_path();
    // name of the directory you cd into
    string currentDirectoryName = currentPath.filename().string();
    // This will be the current path of the repository.
    string repositoryPath = currentPath.string() + "\\" + mRepositoryFolderName;

    for (auto &p : filesystem::recursive_directory_iterator(currentPath)) {
        // We need to avoid adding our repository folder to our repository folder.
        // We find if the directory we're in contains the repository folder name.

        if (p.path().parent_path().string().find(repositoryPath) != string::npos) {
            continue;
        }
        if (filesystem::is_regular_file(p)) {
            string filePath = p.path().string(); // Location of where the repository is stored.
            string destination = GetRepoPath(filePath);
            filesystem::create_directories(destination); // Create folders for the files in the repository.
            destination = destination + "\\" + CheckSum(filePath); // Update destination to include checksum. This will rename the file to that checksum.
            filesystem::copy_file(filePath, destination); // Copies over the file to its respective repository folder.
        }
    }
}

void Repository::CreateManifest() const {
    const vector<string> date = DateStamp();
    string dateString = date[1];

    // Open file for writing.
    string manifestLocation = mRepositoryFolderName + "\\manifests\\" + dateString + ".txt";
    ofstream output(manifestLocation);

    // Initial Manifest file writing - path & time
    output << "Project Tree Path :" << filesystem::current_path() << endl;
    output << "Check-in date: " << date[0] << endl;
    output << "Previous Manifest: " << (GetPrevManifest().compare(dateString + ".txt") == 0 ? "none" : GetPrevManifest()) << endl;

    string currentDirectoryName = filesystem::current_path().filename().string();
    output << "Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path())) {
        string path = p.path().string();
        if (path.find(mRepositoryFolderName) == string::npos) { //will exclude paths with the repo name in the output 
            output << path.substr(path.find(currentDirectoryName));
            if (filesystem::is_regular_file(p)) {
                output << " | Artifact ID: " << CheckSum(p.path().string());
            }
            output << endl;
        }
    }
    output.close();
}

const string Repository::CheckSum(const string path) const {
    filesystem::path p = filesystem::canonical(path);
    return to_string(filesystem::file_size(p) % 256);
}


const vector<string> Repository::DateStamp() const {
    // Grab date/time.  
    vector<std::string> date;
    time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm *clock = localtime(&time);
    string dateStamp = to_string(1900 + clock->tm_year) + (1 + clock->tm_mon >= 10 ? to_string(1 + clock->tm_mon) : "0" + to_string(1 + clock->tm_mon)) +
        (clock->tm_mday >= 10 ? to_string(clock->tm_mday) : "0" + to_string(clock->tm_mday));
    string timeStamp = (clock->tm_hour >= 10 ? to_string(clock->tm_hour) : "0" + to_string(clock->tm_hour)) +
        (clock->tm_min >= 10 ? to_string(clock->tm_min) : "0" + to_string(clock->tm_min)) +
        (clock->tm_sec >= 10 ? to_string(clock->tm_sec) : "0" + to_string(clock->tm_sec));
    string datetime = dateStamp + timeStamp;
    string sDate = to_string(1 + clock->tm_mon) + "/" + to_string(clock->tm_mday) + "/" + to_string(1900 + clock->tm_year) +
        " time: " + to_string(clock->tm_hour % 12) + ":" + to_string(clock->tm_min) + ":" +
        to_string(clock->tm_sec) + (clock->tm_hour >= 12 ? "pm" : "am");

    date.push_back(sDate);	   //vector[0] returns toString rep of  full date & time.
    date.push_back(datetime);  //vector[1] returns date and time stamp for manifests.
    date.push_back(dateStamp); //vector[2] returns date
    date.push_back(timeStamp); // vector[3] returns time
    return date;
}

//Retrieves the name of the previous manifest file.
const string Repository::GetPrevManifest() const {
    string latest = "none";
    for (auto& p : filesystem::directory_iterator(mRepositoryFolderName + "\\manifests\\")) {
        if (std::atoi(p.path().filename().string().c_str()) > std::atoi(latest.c_str()))
            latest = p.path().filename().string();
    }
    return latest;
}

/*
**This retrieves the path relative to the the repo folder.
**From C:\\..\ptree\example , will return C:\\..\repo\example
*/
const string Repository::GetRepoPath(string filePath) const {
    string currentDirectoryName = filesystem::current_path().filename().string();
    int cutOffLocation = filePath.find(currentDirectoryName) + currentDirectoryName.length(); // location where dir name is found.
    string repopath = filePath.substr(0, cutOffLocation) + "\\" + mRepositoryFolderName + filePath.substr(cutOffLocation);
    return repopath;
}

//Prints out a nicely formated tree layout of the directory. Used for Manifest.
void Repository::PrintStructure(ofstream os, string dir) {
    filesystem::path c_path = dir;
    os << c_path.filename() << endl;
    int depth = 1;
    for (auto &p : filesystem::directory_iterator(c_path)) {

    }
}

void Repository::PrintStructure(ofstream os, filesystem::path dir, int depth) {

}
