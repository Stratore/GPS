#include "ville.h"

// contructeur avec toutes les donnée qui m'intersent
Ville::Ville(int _id, std::string _nom, double _lat, double _lon, int _pop)
    : id(_id), nom(_nom), latitude(_lat), longitude(_lon), population(_pop)
{}

// constructeur par défaut que j'utilise pour creer un tableau de 100 villes
Ville::Ville()
    : id(-1), nom(""), latitude(0.0), longitude(0.0), population(0)
{}

// destructeur
Ville::~Ville() {}


//les getters avec des const pour dire que je ne changerais pas les donnees de l'objet Ville
int Ville::getId() const {
    return id;
}

std::string Ville::getNom() const {
    return nom;
}

double Ville::getLatitude() const {
    return latitude;
}

double Ville::getLongitude() const {
    return longitude;
}

int Ville::getPopulation() const {
    return population;
}
