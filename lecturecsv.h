#ifndef LECTURECSV_H
#define LECTURECSV_H

#include <vector>
#include <string>
#include <QString>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ville.h"

class LectureCSV
{
public:
    LectureCSV();   //constructeur
    ~LectureCSV();  // desctructeur

    static std::vector<Ville> lireVilles(const std::string& cheminFichier); // ca lit le csv et ca renvoit un tableau de villes


    static void lireTemps(const std::string& cheminFichier, std::vector<std::vector<int>>& matrice, std::vector<std::vector<int>>& parent);
};

#endif

