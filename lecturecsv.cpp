#include "lecturecsv.h"
#include <QDebug>

LectureCSV::LectureCSV() {}

LectureCSV::~LectureCSV() {}


std::vector<Ville> LectureCSV::lireVilles(const std::string& cheminFichier) {

    std::vector<Ville> tableau_villes(100); // je crée un tabeau de 100 cases

    std::ifstream fichier_ville(cheminFichier);     // je change le nom du fichier en istream

    if(!fichier_ville.is_open()) {      //toujour la sécurité si on trouve pas le fichier
        std::cerr << "Erreur : fichier villes introuvable à l'emplacement " << cheminFichier << std::endl;
        return tableau_villes; // ca renvoie le tableau vide
    }

    std::string ligne;

    std::getline(fichier_ville, ligne); //il faut ignoré la première ligne de ce fichier car elle est faite pour nous indiquer l'ordre

    // ca parcour le fichier ligne par ligne
    while(std::getline(fichier_ville, ligne)) {
        std::stringstream ss(ligne);    // pour creer des séparations

        // toutes les colonnes lues sont temporairement des strings
        std::string nom, id_str, lat_str, lon_str, pop_str, poubelle;   // c'est strings seront les données qui m'interesse


        // 1) city  je garde
        std::getline(ss, nom, ',');

        // 2) city_id   je garde
        std::getline(ss, id_str, ',');

        // 3) city_alt
        std::getline(ss, poubelle, ',');

        // 4) lat   je garde
        std::getline(ss, lat_str, ',');

        // 5) lng   je garde
        std::getline(ss, lon_str, ',');


        // 6 à 14 country, iso2, iso3, admin_name, admin_name_ascii, admin_code, admin_type
        for(int i = 0; i < 9; i++) {
            std::getline(ss, poubelle, ',');
        }

        // 15) population   je garde
        std::getline(ss, pop_str, ',');

        //le reste pas besoin


        // sécurité que la ligne n'est pas vide
        if (!id_str.empty() && !lat_str.empty() && !lon_str.empty()) {
            try {
                // je passe par un try car les convertion peuvent faire cracher le programme si il y a un problem
                int id = std::stoi(id_str);

                QString qLat = QString::fromStdString(lat_str);
                double latitude = qLat.toDouble();


                QString qLon = QString::fromStdString(lon_str);
                double longitude = qLon.toDouble();


                int population = std::stoi(pop_str);


                tableau_villes.push_back(Ville(id, nom, latitude, longitude, population));  //on crée le ville avec les attribut qu'on viens d'extraire et on le met das le tableau


            } catch (const std::exception &e) {     //j'attrape les erreurs
                qDebug() << "crash de ville.csv";       //petit debuggeur pour savoir ce qui va pas en cas de crash
                qDebug() << "l'erreur est : " << e.what();
                qDebug() << "impossible de convertir cette ligne : " << QString::fromStdString(ligne);
                qDebug() << "id_str vaut : '" << QString::fromStdString(id_str);        // je doit convertir en QString car qDebug marche avec des QString
                qDebug() << "lat_str vaut : '" << QString::fromStdString(lat_str);
                qDebug() << "lon_str vaut : '" << QString::fromStdString(lon_str);
                qDebug() << "pop_str vaut : '" << QString::fromStdString(pop_str);
            }
        }
    }

    return tableau_villes;
}


// le role de cette fonction est de remplir les matrices, le passage en référence est super important pour eviter les copi et travailler sur les matrices originels
void LectureCSV::lireTemps(const std::string& cheminFichier, std::vector<std::vector<int>>& matrice, std::vector<std::vector<int>>& parent) {

    std::ifstream fichier_temps(cheminFichier); //meme chose que pour ville

    if(!fichier_temps.is_open()) {  //sécurité
        std::cerr << "Erreur : fichier temps introuvable à l'emplacement " << cheminFichier << std::endl;
        return;
    }

    std::string ligne;

    while(std::getline(fichier_temps, ligne)) { //ici c'est le meme principe sauf qu'il y a que 3 valeur par ligne donc c'est plus rapide

        std::stringstream ss(ligne);    //decoupage des donnés

        std::string dep_str, arr_str, temps_str;    //pas de poubelle on garde tous

        std::getline(ss, dep_str, ','); //je decoupe les données par les virgules
        std::getline(ss, arr_str, ',');
        std::getline(ss, temps_str, ',');


        if (!dep_str.empty() && !arr_str.empty() && !temps_str.empty()) {   //sécurité
            try {
                // pareil convertion, je passe par try pk ca peut crash
                int dep = std::stoi(dep_str);
                int arr = std::stoi(arr_str);
                int temps = std::stoi(temps_str);

                matrice[dep][arr] = temps;  //initialisation de la matrice
                matrice[arr][dep] = temps;  //praeil car le graphe est non orienté
                parent[dep][arr] = arr;     //pour l'instant le seul moyen que tu connais d'aller de dep a arr c'est d'y aller direct ca sert de base pour l'algo floyd warshall pour ensuite trouver le chemain le plus cours
                parent[arr][dep] = dep;     //pareil

            } catch (const std::exception &e) { // j'attrate tous type d'erreure
                qDebug()<< "crash de temps.csv";            // meme utilité que la fonction du haut
                qDebug() << "l'erreur est : " << e.what();
                qDebug() << "impossible de convertir cette ligne : " << QString::fromStdString(ligne);
                qDebug() << "dep_str vaut : '" << QString::fromStdString(dep_str);
                qDebug() << "arr_str vaut : '" << QString::fromStdString(arr_str);
                qDebug() << "temps_str vaut : '" << QString::fromStdString(temps_str);
            }
        }
    }
}
