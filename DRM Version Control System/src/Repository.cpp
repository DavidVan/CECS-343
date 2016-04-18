#include <chrono>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <deque>
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
    CreateManifest(mRepositoryFolderName);
}

void Repository::Update() {
    cout << "Updating repository." << endl;
    CreateProjectTree();
    CreateManifest(mRepositoryFolderName);
}

/*
@Param: src = project tree folder ; target = repository folder
check in will update repository folder with new files/folders from the project tree directory.
Manifest will be updated as well.
*/
void Repository::CheckIn(string src, string target) {
    if (target == "") {
        target = filesystem::current_path().string() + "\\" + mRepositoryFolderName;
    }
    for (auto &p : filesystem::recursive_directory_iterator(src)) { // Go through the project tree folder and check for updated/new files.
        string  filePath = p.path().string();
        if (filesystem::is_regular_file(p)) {
            if (filePath.find(mRepositoryFolderName) == string::npos) {	// Excludes the project tree's repository folder from the iteration.
                bool isNewFile = true;
                for (auto &r : filesystem::recursive_directory_iterator(target)) { // Iterates through the repository folder
                    if (filesystem::is_directory(r)) {
                        string test1 = GetFileLocation(src, p.path().string());
                        string test2 = GetFileLocation(target, r.path().string());
                        if (GetFileLocation(src, p.path().string()).compare(GetFileLocation(target, r.path().string())) == 0) { // File already exists in repository. Update repository with project tree's version.
                            isNewFile = false;
                            string artifact = r.path().string() + "\\" + CheckSum(filePath);  // the theoretical file path of the updated file's artifact
                            if (!filesystem::exists(artifact)) { // If this file does not exist in the repository...
                                string repositoryProjectTreeFileLocation = r.path().parent_path().parent_path().string() + GetFileLocation(src, filePath); // Location of where to place the file in repository's project tree
                                filesystem::copy_file(filePath, artifact, filesystem::copy_options::overwrite_existing); // Copy it over to the repository.
                                filesystem::copy_file(artifact, repositoryProjectTreeFileLocation, filesystem::copy_options::overwrite_existing); // Now copy it to repository's project tree.
                                cout << "File updated: " << p.path().filename() << endl;
                                break; // Finished copying. Break out.
                            }
                        }
                    }
                }
                // new file is found. create a new subdirectory and new artifact in the repo
                if (isNewFile) {
                    string newFilePath = GetRepoPath(src, target, filePath);
                    filesystem::path path = newFilePath;
                    string repositoryProjectTreeFileLocation = path.parent_path().parent_path().string() + GetFileLocation(src, filePath); // Location of where to place the file in repository's project tree
                    filesystem::create_directories(newFilePath); // Create folders for the files in the repository.
                    newFilePath = newFilePath + "\\" + CheckSum(filePath); // Update filepath to include checksum. This will rename the file to that checksum.
                    filesystem::copy_file(filePath, newFilePath); // Copies over the file to its respective repository folder.
                    filesystem::copy_file(newFilePath, repositoryProjectTreeFileLocation); // Now copy it to repository's project tree.
                    cout << "New File is added to the repo: " << p.path().filename() << endl;
                    break; // Finish copying. Break out.
                }
            }
        }
    }
    CreateManifest(target); // updates manifest file
}

void Repository::CheckOut(string src, string target, string manFileName) {
	if (src == "") {
		src = filesystem::current_path().string() + "\\" + mRepositoryFolderName;
	}
	if (!filesystem::exists(src) || !filesystem::is_directory(src)) {
		cout << "Source doesn't exist or is not a directory." << endl;
		return;
	}
	if (!filesystem::is_directory(target) && filesystem::exists(target)) {
		cout << "Target is not a directory." << endl;
		return;
	}
	//READING into manifest file
	string line, sourcePath, targetPath;
	string manifestLocation = src + "\\manifests\\" + ((manFileName == "") ? GetPrevManifest(src) : (manFileName + ".txt"));
	if (!filesystem::exists(manifestLocation)) {
		cout << "Manifest doesn't exist" << endl;
		return;
	}
	cout << manifestLocation << endl;
	ifstream input(manifestLocation);
	filesystem::create_directories(target);
	while (getline(input, line))
	{
		if (!(line.find("#") != string::npos)) {
			if ((line.find("Artifact ID") != string::npos) && line.find("\\") != string::npos && (line.find(".") != string::npos)) {
				sourcePath = src + line.substr(line.find("\\"), line.find(" ") - line.find("\\")) + "\\" + line.substr(line.find_last_of(" ") + 1);
				targetPath = target + line.substr(line.find("\\"), line.find(" ") - line.find("\\"));
				cout << sourcePath << endl;
				cout << targetPath << endl;
				if (!filesystem::exists(target)) {
					filesystem::copy_file(sourcePath, targetPath);
				}
			}
		}
	}
	input.close();
	//END READING TO MANIFEST
	string newManifestLocation = target + "\\manifests\\" + ((manFileName == "") ? GetPrevManifest(src) : (manFileName + ".txt"));
	cout << newManifestLocation << endl;
	cout << manifestLocation << endl;
	if (!filesystem::exists(newManifestLocation)) {
		filesystem::copy_file(manifestLocation, newManifestLocation);//copying source manifest to target manifest if not exists
	}
	string temp = mRepositoryFolderName;

	//CreateManifest();
	//cout << GetPrevManifest() << endl;
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
            string destination = GetRepoPath(currentPath.string(), repositoryPath, filePath);
            filesystem::create_directories(destination); // Create folders for the files in the repository.
            destination = destination + "\\" + CheckSum(filePath); // Update destination to include checksum. This will rename the file to that checksum.
            filesystem::copy_file(filePath, destination, filesystem::copy_options::overwrite_existing); // Copies over the file to its respective repository folder.
        }
    }
}

void Repository::CreateManifest(string repopath) const {
    const vector<string> date = DateStamp();
    string dateString = date[1];

    // Open file for writing.
    string manifestLocation = repopath + "\\manifests\\" + dateString + ".txt";
    ofstream output(manifestLocation);

    // Initial Manifest file writing - path & time
    output << "# Project Tree Path :" << filesystem::current_path() << endl;
    output << "# Check-in date: " << date[0] << endl;
    output << "# Previous Manifest: " << GetPrevManifest(repopath) << endl;

    string currentDirectoryName = filesystem::current_path().filename().string();
    output << "# Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(filesystem::current_path())) {
        string path = p.path().string();
        if (path.find(mRepositoryFolderName) == string::npos && filesystem::is_regular_file(p)) { // Will exclude repository directory and empty folders.
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
const string Repository::GetPrevManifest(string repopath) const {
    string latest = "0";
    string previous = "";
    for (auto& p : filesystem::directory_iterator(repopath + "\\manifests\\")) {
        string file = p.path().filename().string();
        int cutOffLocation = file.find(".");
        file = file.substr(0, cutOffLocation);
        if (atoll(file.c_str()) > atoll(latest.c_str())) {
            previous = latest;
            latest = file;
        }
    }
    return (previous == "0" ? "none" : (previous + ".txt"));
}

/*
**This retrieves the path relative to the the repo folder.
**From C:\\..\ptree\example , will return C:\\..\repo\example
*/
const string Repository::GetRepoPath(string ptreepath, string repopath, string filePath) const {
    filesystem::path ptree = ptreepath;
    string currentDirectoryName = ptree.filename().string();
    int cutOffLocation = filePath.find(currentDirectoryName) + currentDirectoryName.length(); // location where dir name is found.
    string finalPath = repopath + filePath.substr(cutOffLocation);
    return finalPath;
}

// Cuts off path. Example: C:\Projects\ProjectTree\File.txt -> \File.txt
// Pass in "C:\Projects\ProjectTree" as rootPath and "C:\Projects\ProjectTree\File.txt" as filePath
const string Repository::GetFileLocation(string rootPath, string filePath) const {
    filesystem::path path = rootPath;
    string directory = path.filename().string();
    int cutOffLocation = filePath.find(directory) + directory.length();
    string finalPath = filePath.substr(cutOffLocation);
    return finalPath;
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
