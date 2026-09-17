#ifndef METEO_H
#define METEO_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QString>
#include <string>

//je me suis beaucoup servi du tp 7 sur les API en QT pour faire cette class
// pour avoir acces au api il faut changer un paramètre dans le .pro


class Meteo
{

public:

    Meteo();    //constructeur

    QNetworkAccessManager manager;
    void chargeJSON(QString url);   //pour charger le fichier que nous donne l'API

    // les getter
    double getTemps() const;
    double getRessentie() const;
    int getHumidite() const;
    double getVent() const;

    QString getDescription() const;
    QString getVille() const;
    QString getLever() const;
    QString getCoucher() const;


private :
    //toutes les données que je vais extraire je ne vais pas tous utiliser si je change d'avis j'aurais deja pas mal de truc
    double temps;
    double ressentie;
    int humidite;
    double vent;
    QString description;
    QString ville;
    QString lever;
    QString coucher;

};

#endif // METEO_H
