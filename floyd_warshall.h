#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <vector>

class Floyd_Warshall
{
public:
    Floyd_Warshall();   //constructeur
    static std::vector<std::vector<int>> floyd_warshall(std::vector<std::vector<int>>& matrice, std::vector<std::vector<int>>& parent); //c'est une matrice un vector de vector un tableau de tableau
};

#endif
