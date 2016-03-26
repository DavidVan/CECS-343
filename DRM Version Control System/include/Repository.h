#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>

class Repository {
    /*
        This class manages everything related to repository actions.
    */
private:
    const std::string mRepositoryFolderName;

    void CreateRepository(const std::string);
    void CreateProjectTree() const;
    void CreateManifest() const;
    const std::string CheckSum(const std::string) const;

public:
    Repository();
    ~Repository();
    void Initialize();
};

#endif