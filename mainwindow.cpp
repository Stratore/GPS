#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lecturecsv.h"
#include "floyd_warshall.h"
#include "ville.h"

#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QCompleter>
#include <QGroupBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);


    this->setWindowTitle("GPierreS");   // petit jeux de mot ;)

    gestionMeteo = new Meteo(); //definition du gestionnaire de meteo

    ui->graphicsView_carte->setDragMode(QGraphicsView::ScrollHandDrag); // pour pouvoire ce déplacer sur la carte avec un clique gauche


    // j'innitialise les matrices
    matrice_temps.assign(100, std::vector<int>(100, infini));   // . assigne alloue dynamiquement deux matrices carrées de 100 par 100
    matrice_parent.assign(100, std::vector<int>(100, -1));

    for(int i = 0; i < 100; i++) {      // remplie les diagonales
        matrice_temps[i][i] = 0;        // car le temps pour aller d'une ville a elle meme seera toujour de 0
    }

    // je charge les données et je met tous dans liste_villes
    liste_villes = LectureCSV::lireVilles("villes.csv");


    QStringList listeNoms;  // je vais tous mettre dans un QStringList pour pouvoire faire un sort dessus
    for (Ville &v : liste_villes) {
        listeNoms.push_back(QString::fromStdString(v.getNom()));     // convertion de string en QString
    }

    listeNoms.sort();   // tri par ordre croissant

    // le QCompleter sert a fais une liste de suggestion et j'y met ma lice triée
    QCompleter *completer = new QCompleter(listeNoms, this);

    // caseSensitive pour pas supporter la casse reconnaitre la ville meme sans majuscule,
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    // je met donc ces list de suggestion pour le depart et l'arrivé
    ui->lineEdit_depart->setCompleter(completer);
    ui->lineEdit_arrivee->setCompleter(completer);



    LectureCSV::lireTemps("temps.csv", matrice_temps, matrice_parent);  // je remplie mes matrices avec temps.csv
    Floyd_Warshall::floyd_warshall(matrice_temps, matrice_parent);      // j'utilise l'algorithme de tri

    // j'innitialise la scène graphique pour la carte
    scene = new QGraphicsScene(this);
    ui->graphicsView_carte->setScene(scene);


    initialiser_carte();   // fonction qui crée le carte et met le point
}



QString MainWindow::convertir_min_heure(int &temps) {   // toujour utile dans son code

    int heure = temps / 60;  // je fais passer les min en h
    int minute = temps % 60; // le reste de la division par 60

    QString res = QString::number(heure) + " h " + QString::number(minute) + " min ";   // le résultat

    return res;
}


void MainWindow::initialiser_carte() {
    QPixmap image_carte("./images/carte_france.png");   // je charge la carte que j'ai dans un fichier en dure dans le dossier

    if (image_carte.isNull()) {     // securite
        qDebug() << "problem avec l'image ";
    } else {

        scene->addPixmap(image_carte);  // je mes la carte dans la scene
    }

    // dimensions de l'image
    img_largeur = image_carte.width();
    img_hauteur = image_carte.height();

    // bornes GPS constantes que j'ai du f*aire a la main pour bien caller les villes
    lon_min = -5.3; // point le plus à l'Ouest
    lon_max = 9.7; // point le plus à l'Est
    lat_min = 41.3; // point le plus au Sud
    lat_max = 51.4; // point le plus au Nord

    for (const Ville &v : liste_villes) {
        if (v.getNom().empty()) continue; // sécurite

        //qDebug() << "ville :" << QString::fromStdString(v.getNom()) << " lat :" << v.getLatitude() << " long :" << v.getLongitude(); //pour tester si les lattitudes et le longe sont simplifier

        // conversion GPS vers coordonnees écran
        // x = ((long - min) / (max - min)) * largeur
        // y= ((max - lat) / (max - min)) * hauteur
        double x = (v.getLongitude() - lon_min) / (lon_max - lon_min) * img_largeur;
        double y = (lat_max - v.getLatitude()) / (lat_max - lat_min) * img_hauteur;

        // j'ai implementté ca pour un rendu utilisateur, pour que le point sois plus ou moin gros selon le nb d'habitant
        int taille = 6;
        if (v.getPopulation() < 20000) taille = 4;
        if (v.getPopulation() >= 20000 && v.getPopulation() < 100000) taille = 6;
        if (v.getPopulation() >= 100000 && v.getPopulation() < 500000) taille = 8;
        if (v.getPopulation() >= 500000) taille = 12;


        scene->addEllipse(x - (taille/2), y - (taille/2), taille, taille, Qt::NoPen, Qt::red); // ca centre le point sur les coordonnées GPS en décalant de la moitié de sa taille, et en rayon taille

    }
}




MainWindow::~MainWindow()   // destructeur
{
    delete ui;
}


std::vector<int> MainWindow::reconstruire_chemin(int depart, int arrivee) {     // qui renvoit un vector de tous les id des villes qui sont traversées du départ a l'arrivé
    std::vector<int> chemin;
    QStringList liste_noms;

    // sécurité, si ya pas de chemain on s'arrete
    if (matrice_parent[depart][arrivee] == -1) return chemin;

    // je boucle du départe jusqua l'avant dernière ville
    int actuel = depart;
    while (actuel != arrivee) {
        chemin.push_back(actuel);   // je remplie avec la première car je l'ai deja grace au paramètre pas besoin de le chercher
        for (const Ville &v : liste_villes) {
            if (v.getId() == actuel) {
                liste_noms << QString::fromStdString(v.getNom());   // je convertie en qstring pour que ca rentre dans la QStringList pour pouvoire afficher les liste avec -> pour montrer le chemain
                break; // si ca trouve ca sort de la boucle
            }
        }

        actuel = matrice_parent[actuel][arrivee];
    }

    // j'ajoute l'arrivé au chemain
    chemin.push_back(arrivee);

    return chemin;  //ca renvoit le vector d'id du chemin
}



void MainWindow::dessiner_chemin(std::vector<int> &l) { // cette fonction dessine le chemin sur la carte qu'on passe en paramètre

    // cette boucle affiche les noms des villes du trajet sur la carte
    for (int id : l) {
        for (const Ville &v : liste_villes) {
            if (v.getId() == id) {
                // je calcu la possition reel sur la caret
                double x = (v.getLongitude() - lon_min) / (lon_max - lon_min) * img_largeur;
                double y = (lat_max - v.getLatitude()) / (lat_max - lat_min) * img_hauteur;

                // creation du text qui sera le nom de la ville
                QGraphicsTextItem *txt = scene->addText(QString::fromStdString(v.getNom()));

                // je partitionne avec un petit décalage pour que le nom de la ville ne cache pas le point
                txt->setPos(x + 5, y - 15);

                // un petit peu de style
                txt->setDefaultTextColor(Qt::black);
                QFont font = txt->font();
                font.setBold(true);
                font.setPointSize(9);
                txt->setFont(font);
                txt->setZValue(10); // pour que ce soit au dessus des lignes bleux du trajet

                break;
            }
        }
    }

    // maintenant je dessine le trajet
    for (size_t i = 0; i < l.size() - 1; i++) { // je trace lignes par lignes une etape par une etape
        int idA = l[i];     //id de la ville de départe du segement
        int idB = l[i+1];   // et l'id de la ville de l'arrivé du segement

        // j'innitialise des pointeurs vides pour récupéré les objets Ville au complet
        const Ville *vA = nullptr;
        const Ville *vB = nullptr;

        // je recherche les ville qui correstondent
        for (const Ville &v : liste_villes) {
            if (v.getId() == idA) vA = &v;
            if (v.getId() == idB) vB = &v;
        }

        if (vA && vB) { // sécurié sil le pointeur vaut nullptr il renvoit false
            //conversion gps, produit en croix
            double xA = (vA->getLongitude() - lon_min) / (lon_max - lon_min) * img_largeur; // pour convertir distance en degrès en distance en pixel de apporte a la carte
            double yA = (lat_max - vA->getLatitude()) / (lat_max - lat_min) * img_hauteur;

            double xB = (vB->getLongitude() - lon_min) / (lon_max - lon_min) * img_largeur;
            double yB = (lat_max - vB->getLatitude()) / (lat_max - lat_min) * img_hauteur;

            // partie graphique du stylo couleur / épaisseur
            QPen stylo(Qt::blue);
            stylo.setWidth(3);
            // je dessine le segement de route sur la scène graphique grace au coordonné que je viens de trouver
            scene->addLine(xA, yA, xB, yB, stylo);
        }
    }
}


void MainWindow::on_pushButton_clicked()    // quand le bouton calcluer est cliqué (le gros du programme)
{
    // si on a deja calculer un trajet avant il faut netoyer la carte
    scene->clear();
    initialiser_carte();

    tous_les_ids.clear();

    // je supprime tout ce qui est dans la colonne météo (widgets et ressorts)
    while (QLayoutItem* item = ui->verticalLayout_2->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // juste une sécurité si l'utilisateur ne marque rien
    if (ui->lineEdit_depart->text().isEmpty()) {
        ui->label_resultat->setText("Saisir une ville de départ");
        return;
    }

    if (ui->lineEdit_arrivee->text().isEmpty()) {
        ui->label_resultat->setText("Saisir une ville de d'arrivé");
        return;
    }

    // je viens récupéré le texte de mes lineEdite
    QString depart_str = ui->lineEdit_depart->text();
    QString arrivee_str = ui->lineEdit_arrivee->text();


    // come pour la fonction d'avant j'initialise des pointeurs vide pour récupérer les objets ville au complet
    const Ville* vDep = nullptr;
    const Ville* vArr = nullptr;

    // recherche des id et initialisation des poitneurs
    for (const Ville &v : liste_villes) {
        if (v.getNom() == depart_str.toStdString()) vDep = &v;
        if (v.getNom() == arrivee_str.toStdString()) vArr = &v;
    }


    // sécurité pour gérer les erreure meme principe que la fonction d'avant
    if (!vDep || !vArr) {
        ui->label_resultat->setText("Ville introuvable");
        return;
    }



    // je récupère les id
    int id_depart = vDep->getId();
    int id_arrivee = vArr->getId();

    std::vector<int> chemin;    // je vais venir creer mon vecteur d'id qui va recevoire tous les id du chemin

    int temps_le_plus_court = 0; // je le met à 0 car on va additionner

    if(liste_edits_etapes.size() == 0) {    // cette liste est initialiser dans le hpp et elle est remplie quand l'utilisateur utilise le bouton ajouter étape
        temps_le_plus_court = matrice_temps[id_depart][id_arrivee]; // donc si il n'y a pas d'étape je remplie les matrcies avec la valeur de départ et d'arrivé

        if(temps_le_plus_court < infini)   // si il existe un chemin possible, car tous les chemins sont initialmisés de base a l'infini
            chemin = reconstruire_chemin(id_depart, id_arrivee);    // je remplie le vector du chemin de dep vers arr
    }
    else {  // si il y a a des étapes c'est la que c'est plus compliqué

        // je creer une liste ou je vais petter tous les point dans le bonne ordre
        tous_les_ids.push_back(id_depart); // on commence généralement par le départ ;)

        // ici ca cherche l'id de chaque étapes
        for (size_t i = 0; i < liste_edits_etapes.size(); i++) {    // contrairement a plus haut ici liste_edits_etapes est remplie avec tous les QLineEdites
            std::string nom_str = liste_edits_etapes[i]->text().toStdString();  // je charge la valeur des chaques QLineEdite dans un varaible nom_str

            int id_e = -1; // -1 pour dire qu'on a pas trouvé, c'est un sécurité
            for (const Ville &v : liste_villes) {   // recherche de l'id grace au nom des QLineEdite
                if (v.getNom() == nom_str) {
                    id_e = v.getId();
                }
            }

            if (id_e == -1) {   // sécuritée
                ui->label_resultat->setText("Etape inconnue");
                return;
            }
            tous_les_ids.push_back(id_e); // j'ajoute l'étapes au vector arrivé + tous j'usca lavant dernier + l'arrivée
        }

        tous_les_ids.push_back(id_arrivee);


        // maintenant qu'on a notre vecteur dans l'ordre il faut calculer le temps en aditionnant les matrice de chaque trajets (étapes)
        for (size_t i = 0; i < tous_les_ids.size() - 1; i++) {
            int depart_segment = tous_les_ids[i];
            int arrivee_segment = tous_les_ids[i+1];

            // j'additionn le temps de chaque trajets pour le temps final
            temps_le_plus_court += matrice_temps[depart_segment][arrivee_segment];

            // c'est la liste de toutes les villes intermédiaires que notre algo a trouvé poiur relier les deux étapes
            std::vector<int> petit_chemin = reconstruire_chemin(depart_segment, arrivee_segment);

            // je copie le petit chemin dans le grand chemin case par case
            for (size_t j = 0; j < petit_chemin.size(); j++) {
                // Je saute la premiere case j==0 si on n'est pas au premier trajet (i>0) pour pas de marquer l'étape 2 fois genre Paris->Lyon puis Lyon->Marseille
                if (i > 0 && j == 0) {
                    continue;
                }
                chemin.push_back(petit_chemin[j]);  // maintenant que j'ai toutes les les étapes et les villes intermédiaires je mes tous ca dans mon chemin
            }
        }
    }

    // ici c'est la partie affichage
    if(temps_le_plus_court >= infini) { //une autre sécurité
        ui->label_resultat->setText("Inaccessible");
        ui->label_trace->setText("");
    } else {
        dessiner_chemin(chemin);    // comme son nom l'indique on dessine le chemin
        ui->label_resultat->setText(convertir_min_heure(temps_le_plus_court));  // et j'affiche le résultat que je convertie en h avec ma fonction

        // maintenant il faut afficher les nom des villes de l'itinéraire
        QStringList liste_noms; // je crée un QStringListe plutot qu'un vecteur de string pour pouvoire utiliser .join pour les séparations
        for (int id_chemin : chemin) {  // pour chaques id de ville de chemin
            for (const Ville &v : liste_villes) {
                if (v.getId() == id_chemin) {       // je cherche la ville qui correspond a l'id
                    liste_noms << QString::fromStdString(v.getNom());   // je l'ajoute a ma QStringList
                    break;
                }
            }
        }
        ui->label_trace->setText("Itinéraire : " + liste_noms.join("->"));  // j'affiche tous le chemin Paris -> lyon -> ... c'est propre comme ca



        // ici on passe a la partie météo
        std::vector<int> villes_meteo;  // je prépart le liste des ville ou on vourdra la météo
        // je ne peut pas utiliser chemin qu'on a deja car il contient un eliste énorme de ville et moi je veux juste la météo dap arr et etp

        if (liste_edits_etapes.empty()) {   // RAPPEL ma liste_edits_etapes est un vecteur de QLineEdite
            // si ya pas d'étapes j'ai juste besoin du depart et de l'arrivée
            villes_meteo = {id_depart, id_arrivee};
        } else {
            // si ya des étapes je réutilise mon vecteur que j'ai déjà crée en haut
            villes_meteo = tous_les_ids;
        }

        // alors j'affiche la météo uniquement pour les villes clés
        for (int id_ville : villes_meteo) {
            const Ville* vActuelle = nullptr;
            for (const Ville &v : liste_villes) {
                if (v.getId() == id_ville) {
                    vActuelle = &v;
                    break;
                }
            }

            if (!vActuelle) continue;

            // je crée la GroupBox pour la ville
            QGroupBox *box = new QGroupBox(QString::fromStdString(vActuelle->getNom()));
            QVBoxLayout *layoutInterne = new QVBoxLayout(box);

            // petit calcule de population pour simplifier
            int pop = vActuelle->getPopulation();
            QString textePop;
            if (pop >= 1000000) textePop = QString::number(pop / 1000000.0, 'f', 1) + " million";
            else if (pop >= 1000) textePop = QString::number(pop / 1000.0, 'f', 1) + " millier";
            else textePop = QString::number(pop) + " hab";

            // je récupère la météo comme on a fais dans le tp en IHM
            QString url = "https://api.openweathermap.org/data/2.5/weather?q="
                          + QString::fromStdString(vActuelle->getNom())     // je change just ele nom de la ville dans mon lien
                          + ",france&appid=" + qEnvironmentVariable("OPENWEATHER_API_KEY");   // jamais de clé dans le code (dépôt public)
            gestionMeteo->chargeJSON(url);  // j'interoge l'API et je charge le JSON

            // je crée des labels
            QLabel *lPop = new QLabel("👥 Population : " + textePop);
            QLabel *lTemp = new QLabel("🌡️ Temp : " + QString::number(gestionMeteo->getTemps(), 'f', 1) + " °c");
            QLabel *lVent = new QLabel("💨 Vent : " + QString::number(gestionMeteo->getVent()) + " km/h");
            QLabel *lSoleil = new QLabel("🌅 " + gestionMeteo->getLever() + " | 🌇 " + gestionMeteo->getCoucher());

            // j'ajoute les labels dans la boîte
            layoutInterne->addWidget(lPop);
            layoutInterne->addWidget(lTemp);
            layoutInterne->addWidget(lVent);
            layoutInterne->addWidget(lSoleil);

            // et j'ajoute la boîte dans la colonne de droite
            ui->verticalLayout_2->addWidget(box);
        }

        // j'ajoute un "ressort" (spacer) à la fin pour éiter que me boites prennent toute la hauteur ou qu'il y est des grands espaces entre o
        ui->verticalLayout_2->addStretch();
    }
}

void MainWindow::on_pushButton_plus_clicked()
{
    ui->graphicsView_carte->scale(1.2, 1.2); // ca agrandit de 20%
}




void MainWindow::on_pushButton_moin_clicked()
{
    ui->graphicsView_carte->scale(0.8, 0.8); // ca réduit de 20%
}


void MainWindow::on_pushButton_Ajouter_clicked()    // le bouton pour ajouter une étape
{
    //contrairement au code d'avant je ne peux rien crée avec le disigner car ce n'est pas qqc de fix
    QWidget *ligne = new QWidget();     // je crée un widget qui va tous contenir
    QHBoxLayout *layout = new QHBoxLayout(ligne);   // je crée un layout horizontale que je mes dans mon conteneur
    layout->setContentsMargins(0, 2, 0, 2); // pour controler l'emplacement autour des éléments de ma ligne

    // je crée un pointeur local pour ne pas écraser les autres
    QLineEdit *nouveauEdit = new QLineEdit();
    nouveauEdit->setPlaceholderText("Etape " + QString::number(liste_edits_etapes.size() + 1));
    nouveauEdit->setCompleter(ui->lineEdit_depart->completer());    // comme pour au début du code on veux avoire des suggestions

    QPushButton *btn = new QPushButton("X");    // création du bouton pour suprimer j'ai pris X pour faire comme une croix
    btn->setFixedWidth(30);

    layout->addWidget(nouveauEdit); //j'ajoute la QLigneEdite au layout
    layout->addWidget(btn);         // pareil pour la bnt
    ui->widget_etape->layout()->addWidget(ligne);   // je rajoute tous ca dans le layout de widget étape

    // j'ajoute ce nouveau champe dans liste_edits_etapes qui est un vector de QLineEdit je m'en sert dans la fonction du bouton calculer
    liste_edits_etapes.push_back(nouveauEdit);

    // Petit lambda pour supprimer quand on appuie sur X
    connect(btn, &QPushButton::clicked, this, [=]() {   // j'aurais pu passer par un connect normal a un slot mais le probleme c'est que la fonction que j'aurais crée qu'elle bouton a été cliqué et pareil pour la ligne
        // il faut maintenant retirer le pointeur de la liste
        for (size_t i = 0; i < liste_edits_etapes.size(); i++) {
            if (liste_edits_etapes[i] == nouveauEdit) {
                liste_edits_etapes.erase(liste_edits_etapes.begin() + i);   //liste_edits_etapes.begin() donne un pointeur qui montre l'index 0 et + i c'est pour rajouterle décalage, je fais ca car erase ne prend pas d'index numérique direct en paramètre
                // donc je passe par un innitiateur liste_edits_etapes.begin() + le décalage pour sa position
                break;
            }
        }
        ligne->deleteLater();   // comme pour meteo.cpp je dis au programme de d'attendre la fin de l'évènement (le btn cliqué) avant de détruir l'objet
        // c'est pour éviter un crash de l'app
    });
}


void MainWindow::on_pushButton_generer_matrice_clicked() {  // le bouton pour créer la matrice de temps

    QString nom = ui->lineEdit_depart->text();  // je récuère le texte


    if (nom.isEmpty()) {
        ui->label_resultat->setText("Mettre une ville");
        return;
    }

    std::string nom_str = nom.toStdString();    // convertion
    bool ville_trouvee = false;    //sécurité

    ui->lineEdit_depart->clear();   // quand la ville est mis dans la matrice il faut que le départ sois clear



    for (const Ville &v : liste_villes) {   // je cherche l'id grace au nom
        if (v.getNom() == nom_str) {
            ids_selectionnes.push_back(v.getId());
            ville_trouvee = true;
            break;
        }
    }

    if (!ville_trouvee) {
        ui->label_resultat->setText("ville inconnue");
        return;
    }

    int n = ids_selectionnes.size();    // juste pour simplifier

    ui->tableWidget->setRowCount(n);
    ui->tableWidget->setColumnCount(n);

    QStringList nomsVilles;

    for (int ligne = 0; ligne < n; ++ligne) {
        int idA = ids_selectionnes[ligne];

        // ici je cherche le nom de la ville pour l'entete
        for(const Ville &v : liste_villes) {
            if(v.getId() == idA) {
                nomsVilles << QString::fromStdString(v.getNom());   // j'ajoute a mon QStringList
                break;
            }
        }
        for (int col = 0; col < n; ++col) {
            int idB = ids_selectionnes[col];

            int temps = matrice_temps[idA][idB];        // calcule du temps avec l'algo

            if (temps == 0)
                ui->tableWidget->setItem(ligne, col, new QTableWidgetItem(" - "));
            else
                ui->tableWidget->setItem(ligne, col, new QTableWidgetItem(convertir_min_heure(temps)));
        }
    }
    ui->tableWidget->setHorizontalHeaderLabels(nomsVilles); // pour un beau tableau
    ui->tableWidget->setVerticalHeaderLabels(nomsVilles);
}

