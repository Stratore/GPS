#include "meteo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QEventLoop>

Meteo::Meteo()
{
}

//cette fonction nous parmet de charger un un fichier json grace a un url
void Meteo::chargeJSON(QString url) {


    QNetworkRequest request((QUrl(url)));           //je crée un requet de l'url
    QNetworkReply* reply = manager.get(request);    //le manager envoie la requet et attend une reponse

    //de base Qt n'attend pas la réponse d'internet
    //et sans cette partie j'ai rencontrait un bug car avec le lien j'essayer directement une fois que je l'avais d'aller chercher les données, mais le site n'avais pas encore repondue
    // donc grace a ce loop ca me permet d'attendre un reponse avant d'aller extraire les données
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec(); // Le code s'arrête ici jusqu'à ce que l'API réponde

        // ici ca vérifi si il y a une erreur réseau
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "erreur reseau :" << reply->errorString();  //pour débuger
        reply->deleteLater();   // pour éviter les fuites de mémoires, si le programme fais un return trop vite l'objet reply peut rter bloqué en mémoire
        return;
    }

        QByteArray response_data = reply->readAll();    // ici ca lit toutes les données brutes reçues
        QJsonDocument doc = QJsonDocument::fromJson(response_data); // ca transforme ces octets en un document json
        QJsonObject objetJSON = doc.object();


        QJsonObject mainObj = objetJSON["main"].toObject(); //car dans le json les infos sont dans un sous objet nommé main

        // j'ai pris pas mal de données meme si dans mon gps je n'utilise pas tous, comme ca s j'en ai besoin j'ai deja les variables de crée plus qu'a les implentées

        double temp = mainObj["temp"].toDouble() - 273.15;       //convertin de kelvin a cellcuse
        double feels_like = mainObj["feels_like"].toDouble() - 273.15;
        double humidity = mainObj["humidity"].toDouble();

        QJsonValue weather = objetJSON["weather"];
        QString weather_description = weather[0]["description"].toString();


        double _vent = objetJSON["wind"].toObject()["speed"].toDouble();
        QJsonObject sysObj = objetJSON["sys"].toObject();
        double sunset = sysObj["sunset"].toDouble();
        double sunrise = sysObj["sunrise"].toDouble();

        qint64 ts = sunset; // la methode qu'on a vu en tp pour avoir l'ecriture hh//mm
        QString chaine = QDateTime::fromSecsSinceEpoch(ts).toString("hh:mm");       //pas besoin des secondes

        qint64 ts1 = sunrise;
        QString chaine1 = QDateTime::fromSecsSinceEpoch(ts1).toString("hh:mm");



        // je remplie les attributs de l'objet meteo
        temps = temp;
        ressentie = feels_like;
        humidite = humidity;
        vent = _vent;
        description = weather_description;
        ville = objetJSON["name"].toString();
        lever = chaine1;
        coucher = chaine;

        reply->deleteLater();   //si tous ce passe bien le code ne rentre pas dans le if au dessus donc il faut bien le netoiller ici
        // sans ca a chaques fois que ca calcule un itinéraire de 5 étapes ce cree 7 QNetworkReply qui restent bloqué dans la RAM (fuite de mémoire)
}

// getters tous est en const car ca garentie que je ne modifierai rien a l'interieur de l'objet Meteo
double Meteo::getTemps() const
{
    return temps;
}

double Meteo::getRessentie() const
{
    return ressentie;
}

int Meteo::getHumidite() const
{
    return humidite;
}

double Meteo::getVent() const
{
    return vent;
}

QString Meteo::getDescription() const
{
    return description;
}

QString Meteo::getVille() const
{
    return ville;
}

QString Meteo::getLever() const
{
    return lever;
}

QString Meteo::getCoucher() const
{
    return coucher;
}

