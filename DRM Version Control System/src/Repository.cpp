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
        cout << "Repository created." << endl;
    }
    CreateProjectTree();
    CreateManifest();
}
void Repository::CheckIn( string src, string target) {
	cout << "src: " << src << endl << "target: " << target << endl;
}
void Repository::CheckOut( string src, string target) {
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
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }

    // This will be the current path of the repository.
    string repositoryPath = currentPath.string() + "\\" + mRepositoryFolderName;

    for (auto &p : filesystem::recursive_directory_iterator(currentPath)) {
        // We need to avoid adding our repositoy folder to our repository folder.
        // We find if the directory we're in contains the repository folder name.

        if (p.path().parent_path().string().find(repositoryPath) != string::npos) {
            continue;
        }
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

void Repository::CreateManifest() const{
	const vector<string> date = DateStamp();
	string dateString = date[1];

    // Open file for writing.
    string manifestLocation = mRepositoryFolderName + "\\manifests\\" + dateString + ".txt";
    ofstream output(manifestLocation);

    // Initial Manifest file writing - path & time
    output << "Project Tree Path :" << filesystem::current_path() << endl;
	output << "Check-in date: " << dateString <<endl;
    output << "Previous Manifest: "  << (GetPrevManifest().compare(dateString+".txt")==0 ? "none" : GetPrevManifest()) << endl;

    filesystem::path currentPath = filesystem::current_path();
    string currentDirectory;
    for (auto &p : currentPath) {
        currentDirectory = p.string();
    }

    output << "Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path())) {
        string path = p.path().string();
        if (path.find(mRepositoryFolderName) == string::npos) { // This will exclude paths with the repo name in the output. 
            output << path.substr(path.find(currentDirectory));
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


const vector<string> Repository::DateStamp() const{
	// Grab date/time.  
	vector<std::string> date;
	time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm *clock = localtime(&time);
	string dateStamp = to_string(1900 + clock->tm_year) + (1 + clock->tm_mon >= 10 ? to_string( 1 + clock->tm_mon) : "0" + to_string( 1+ clock->tm_mon)) + 
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