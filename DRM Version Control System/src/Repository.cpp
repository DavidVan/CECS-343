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
    CreateManifest(filesystem::current_path().string(), mRepositoryFolderName, "");
}

void Repository::Update() {
    cout << "Updating repository." << endl;
    CreateProjectTree();
    CreateManifest(filesystem::current_path().string(), mRepositoryFolderName, "");
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
                    string newFilePath = GetRepositoryPath(src, target, filePath);
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
    CreateManifest(filesystem::current_path().string(), target, "# Check In Location: " + target); // updates manifest file
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
    string line;
	string manifestLocation = src + "\\manifests\\" + manFileName+ ".txt";
	if (!filesystem::exists(manifestLocation)) {
		cout << "Manifest doesn't exist" << endl;
		return;
	}
	cout << "Manifest Location: " << manifestLocation << endl;
	ifstream input(manifestLocation);
	filesystem::create_directories(target);
	while (getline(input, line))
	{
        string test = line.substr(0, 1);
        if (line.substr(0, 1).compare("#") != 0 && line.substr(0, 1).compare("@") != 0) { // Tries to find "#" or "@" at the beginning of a line. We need to find non-"#/@" containing lines to extract the paths.
			if ((line.find("Artifact ID") != string::npos) && (line.find("\\") != string::npos)) { // Found a line containing paths to files.
                string sourcePath, targetPath, targetDirectory;
                int cutOffLocation = line.find(" | ") - line.find("\\"); // Needed to cut off " | Artifact ID: ~~~~"... We find the location of " | " and then the location of the first "\", and subtract them.
                sourcePath = src + line.substr(line.find("\\"), cutOffLocation) + "\\" + line.substr(line.find_last_of(" ") + 1);
                targetPath = target + line.substr(line.find("\\"), cutOffLocation);
                int directoryCutOffLocation = targetPath.length() + (targetPath.find_last_of("\\") - targetPath.length());
                targetDirectory = targetPath.substr(0, directoryCutOffLocation);
                filesystem::create_directories(targetDirectory);
				cout << "Copying to: " << targetPath << endl;
				filesystem::copy_file(sourcePath, targetPath, filesystem::copy_options::overwrite_existing);
			}
		}
	}
	input.close();
	//END READING TO MANIFEST
	string newManifestLocation = target + "\\" + mRepositoryFolderName + "\\manifests\\" + manFileName + ".txt";
	if (!filesystem::exists(newManifestLocation)) {
        int directoryCutOffLocation = newManifestLocation.length() + (newManifestLocation.find_last_of("\\") - newManifestLocation.length());
        string manifestDirectory = newManifestLocation.substr(0, directoryCutOffLocation);
        filesystem::create_directories(manifestDirectory);
		filesystem::copy_file(manifestLocation, newManifestLocation, filesystem::copy_options::overwrite_existing); //copying source manifest to target manifest if not exists
	}
    CreateManifest(target, target + "\\" + mRepositoryFolderName, "@ Previous Project Tree Location: " + filesystem::current_path().string());
    cout << endl;
}

/*
@Param: src = repository path , target = ptree location, manFileName = manifest file name  
Merge the changes of a repository's version to a project tree using the specified manifest.
If no target is specified, it will assume target is the current directory (CD'ed location).
Merge will simply supply the files needed for merging, but won't do any actual merging.
*/
void Repository::Merge(string src, string target, string manFileName) {

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
            string destination = GetRepositoryPath(currentPath.string(), repositoryPath, filePath);
            filesystem::create_directories(destination); // Create folders for the files in the repository.
            destination = destination + "\\" + CheckSum(filePath); // Update destination to include checksum. This will rename the file to that checksum.
            filesystem::copy_file(filePath, destination, filesystem::copy_options::overwrite_existing); // Copies over the file to its respective repository folder.
        }
    }
}

void Repository::CreateManifest(string directory, string repoPath, string words) const {
    const vector<string> date = DateStamp();
    string dateString = date[1];

    // Open file for writing.
    string manifestLocation = repoPath + "\\manifests\\" + dateString + ".txt";
    ofstream output(manifestLocation);

    // Initial Manifest file writing - path & time
    output << "# Project Tree Path: " << directory << endl;
    output << "# Check-in date: " << date[0] << endl;
    output << "# Previous Manifest: " << GetPreviousManifest(repoPath) << endl;
    output << words << endl; // Custom messages here.
    
    string currentDirectoryName = (directory.find("\\") ? (directory.substr(directory.find_last_of("\\") + 1)) : directory);
    output << "# Project tree Files and Artifact IDs:\n" << endl;
    for (auto &p : filesystem::recursive_directory_iterator(directory)) {
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

//Retrieves the name of the previous manifest file in the given repopath.
const string Repository::GetPreviousManifest(string repopath) const {
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
/*Retrieves the latest manifest file with the manifest folder as the parameter*/
const string Repository::GetLatestManifest(string manifestfolder) const {
	string latest = "none";
	for (auto& p : filesystem::directory_iterator(manifestfolder)) {
		if (std::atoi(p.path().filename().string().c_str()) > std::atoi(latest.c_str())) {}
		latest = p.path().filename().string();
	}
	return latest;
}
const string Repository::ReadPrevManifest(string manfile) const{
	return "";
}

/*
**This retrieves the path relative to the the repo folder.
**From C:\\..\ptree\example , will return C:\\..\repo\example
*/
const string Repository::GetRepositoryPath(string ptreepath, string repopath, string filePath) const {
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

// Given the path to the manifest folder, finds and returns the path to the
// previous project tree location (where it was checked out from) if any.
const string Repository::GetPreviousProjectTreeLocation(string previousManifest) const {
	return "hi";
}

// Retrieves the "grandpa" file location of the repo and the given project tree. 
const  string Repository::GetGrandpa(string src, string target)const{
	string manipath= "//repo343//manifests";
	string currentSrc = src + manipath;
	string currentTarget = target + manipath;

	string latestSrcManifest = GetLatestManifest(currentSrc);
	string latestTargetManifest = GetLatestManifest(currentTarget);

	while (GetPreviousProjectTreeLocation(currentSrc) != "none" && GetPreviousProjectTreeLocation(currentTarget) != "none") {
		if (latestSrcManifest.compare(latestTargetManifest) == 0) {
			return latestSrcManifest.filename();
		}
		if (latestSrcManifest.string() == "none" && latestTargetManifest.string() != "none") {
			latestSrc = GetLatestManifest(src + manipath);
			latestTarget = ReadPrevManifest(currentSrc);
		}
		if (latestTarget == "none" && latestSrc != "none") {
			latestTarget = GetLatestManifest(target + manipath);
			latestSrc = ReadPrevManifest();

		}
	}
	/* PSEUDOCODE
	initialize curSrcLocation (will change when it hits end of man dir) and curTargetLocation
	initialize latestsrc & latesttarget(helper method will be made for this)
	while repo's  && ptree's previous ptree location != "none" && their getPrevManFile!= "none")
		create full length paths for latestsrc and latesttarget
		if latestsrc == latesttarget
			return latestsrc (path)

		if latestsrc == "none" && latestarget != "none"
			reset latestsrc = getlatestmanfile (src)
			latesttarget = getprevmanfile (latesttarget)
		else if (latestsrc !=none && latesttarget =="none")
			reset latesttarget = getlatestmanfile (target)
			latestsrs = getprevmanfile (latestsrc)

		if latestsrc > latesttarget
			if (getPrevPtreeLoc (latestsrc) !=none)
				curSrcLocation = getPrevPtreeLoc (latestsrc)
				latestsrc = getlatestmanfile (curSrcLocation)
			else
				latestscc = getpreviousmanifest(latestsrc full path)

		if latestsrc < latesttarget
			if (getPrevPtreeLoc (latesttarget) !=none)
				curTargetLocation = getPrevPtreeLoc (latesttarget)
				latestTarget = getlatestmanfile (curTargetLocation)
			else
				latestTarget = getpreviousmanifest(latestTarget full path)

	*/
	
	return "grandpapath";
}
