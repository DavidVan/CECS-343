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
public:
    Repository();
};

#endif