#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>

class Repository {
    /*
        This class manages everything related to repository actions.
    */
private:
    const std::string mRepositoryFolderName = "repo343";
    void CreateRepository(const std::string);
    void CreateProjectTree();
    void CreateManifest();
    std::string CheckSum(std::string);

public:
    Repository();
};

#endif