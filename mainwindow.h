#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include "meteo.h"
#include "ville.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <QLineEdit>
#include <QGraphicsScene>
#include <QVBoxLayout>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);  //constructeur

    ~MainWindow();  //destructeur
    int infini = 999999;   //j'innitialise l'infinie ca va me servir pour initialiser mes matrices et pour savoire si un trajet est possible

    std::vector<int> reconstruire_chemin(int depart, int arrivee);  // comme sont nom l'incidque cette fonction renvoit un tableau de tous les ids de toutes les viles traversé

    void dessiner_chemin(std::vector<int> &l);  // dessine le chemain sur la carte grace au tableau de id qu'il prend en paramètre
    void initialiser_carte();                   // fonction pour afficher la carte et mettre les points dessus
    QString convertir_min_heure(int &temps);    // comme sont nom l'indique ;)
    std::vector<int> ids_selectionnes;          // ca va me servir a stocker les id pour faire le bouton generer matrice


private slots:
    void on_pushButton_clicked();           //bouton calculer

    void on_pushButton_Ajouter_clicked();   //bouton pour ajouter des etapes

    void on_pushButton_generer_matrice_clicked();   //bouton pour generer une matrice de temps

    void on_pushButton_plus_clicked();      // bouton pour zoomer +

    void on_pushButton_moin_clicked();      // bouton pour zoomer -


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    std::vector<Ville> liste_villes;                //c'est dans cette variable que va etre stocker toutes les viles grace a lecturecsv.cpp
    std::vector<std::vector<int>> matrice_temps;    //la matrice de temps
    std::vector<std::vector<int>> matrice_parent;   // la matrice parent pour reconstruire le chemain

    // pour innitialiser la carte

    double img_largeur;
    double img_hauteur;

    double lon_min;
    double lon_max;
    double lat_min;
    double lat_max;



    //pour les etapes
    std::vector<QLineEdit*> liste_edits_etapes; // pour retrouver les noms plus tard

    std::vector<int> tous_les_ids;


    Meteo * gestionMeteo; //pointeur pour avoir acces a la classe meteo



};
#endif
