#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>

class Repository {
    /*
        This class manages everything related to repository actions.
    */
private:
    void CreateRepository(const std::string);
    void CreateProjectTree();
    std::string CheckSum(std::string);
    int PathType(std::string);

public:
    Repository();
};

#endif