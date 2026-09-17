#ifndef VILLE_H
#define VILLE_H
#include <iostream>
#include <string>

class Ville
{
public:
    Ville(int _id, std::string _nom, double _lat, double _lon, int _pop = 0);   //le constructeur de ville avec toutes les données que je trouve utile
    Ville();    //constructeur par defaut qui sera utile quand je crée u ntableau vide de 100 cases
    ~Ville();   // destructeur

    // les getters
    int getId() const;
    std::string getNom() const;
    double getLatitude() const;
    double getLongitude() const;
    int getPopulation() const;

private :
    //les données
    int id;
    std::string nom;
    double latitude;
    double longitude;
    int population;



};

#endif // VILLE_H
