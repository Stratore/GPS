#include "floyd_warshall.h"

//j'adapte le fonction qu'on nous a donné en python en cpp en lui rajoutant la matrice parent utile pour recontruire un chemain aprrs
Floyd_Warshall::Floyd_Warshall() {}

std::vector<std::vector<int>> Floyd_Warshall::floyd_warshall(std::vector<std::vector<int>>& matrice, std::vector<std::vector<int>>& parent){
    int nb_sommets = matrice.size();

    for(int k = 0; k < nb_sommets; k++) {                  //intermediaire
        for(int u = 0; u < nb_sommets; u++) {              //depart
            for(int v = 0; v < nb_sommets; v++) {          //arrivée
                //ici l'algo teste si c'est plus rapide d'aller d'un point U directemtna un point V ou si c'ets plus rapide de passer par un intermediaire K et il met a jour ca matrice
                if (matrice[u][k] + matrice[k][v] < matrice[u][v]) {
                    matrice[u][v] = matrice[u][k] + matrice[k][v];
                    parent[u][v] = parent[u][k]; // On mémorise le nouveau chemin
                }
            }
        }
    }
    return matrice;
}
