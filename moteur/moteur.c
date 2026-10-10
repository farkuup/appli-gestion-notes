#include <string.h>
#include "moteur.h"

float moyenne_matiere(const Matiere *m){
    if(m->nb_notes == 0){ return -1;}
    float somme=0;
    float somme_des_coeff = 0;
    for(int i=0; i<m->nb_notes; i++){
        somme += m->notes[i].coeff * m->notes[i].valeur;
        somme_des_coeff += m->notes[i].coeff;
    }
    if(somme_des_coeff == 0){ return -1;}
    return (somme/somme_des_coeff);
}
float moyenne_UE(const UE *ue){
    if(ue->nb_matieres == 0){ return -1;}

    float somme=0;
    float somme_des_coeff = 0;
    for(int i=0; i<ue->nb_matieres; i++){
        float moy=moyenne_matiere(&ue->matieres[i]);
        if(moy>=0){
            somme += ue->matieres[i].coeff * moy;
            somme_des_coeff += ue->matieres[i].coeff;
        }
    }
    if(somme_des_coeff == 0){ return -1;}
    return (somme/somme_des_coeff);
}
float moyenne_generale(const UE *tab, int n){       // n : nb d'UE
    if(n == 0){ return -1;}

    float somme=0;
    float somme_des_coeff = 0;
    for(int i=0; i<n; i++){
        float moy=moyenne_UE(&tab[i]);
        if(moy>=0){
            somme += tab[i].coeff * moy;
            somme_des_coeff += tab[i].coeff;
        }
    }
    if(somme_des_coeff == 0){ return -1;}
    return (somme/somme_des_coeff);
}

Indice sous_limite_UE(const UE *tab, int n){
    Indice trouve = {-1, -1};
    for(int i=0; i<n; i++){
        float moy = moyenne_UE(&tab[i]);
        if(moy >= 0 && moy < tab[i].limite){
            trouve.UE = i;
            trouve.MAT = -1;
            return trouve;
        }
    }   
    return trouve;
}
Indice sous_limite_mat(const UE *tab, int n){
    Indice trouve = {-1, -1};
    for(int i=0; i<n; i++){
        for(int j=0; j<tab[i].nb_matieres; j++){
            float moy = moyenne_matiere(&tab[i].matieres[j]);
            if(moy >= 0 && moy < tab[i].matieres[j].limite){
                trouve.UE  = i;
                trouve.MAT = j;
                return trouve;
            }
        }
    }
    return trouve;
}

int nombre_matieres(const UE *tab, int n){
    int total = 0;
    for(int i=0; i<n; i++){
        total += tab[i].nb_matieres;
    }
    return total;
}

Indice trouver_matiere(const UE *tab, int n, int numero){
    Indice trouve = {-1, -1};
    if(numero < 0){ return trouve; }
    int vues = 0;                               // matières déjà passées
    for(int i=0; i<n; i++){
        if(numero < vues + tab[i].nb_matieres){
            trouve.UE  = i;
            trouve.MAT = numero - vues;
            return trouve;
        }
        vues += tab[i].nb_matieres;
    }
    return trouve;
}

static const char *NOMS_TYPES[NB_TYPES] = { "DS", "TP", "Khôlle", "CC", "Autre" };

const char *nom_type(int type){
    if(type < 0 || type >= NB_TYPES){ return "?"; }
    return NOMS_TYPES[type];
}

int ajouter_note(Matiere *m, int type, const char *intitule, float valeur, float coeff){
    if(m->nb_notes >= MAX_NOTES){
        return -1;
    }
    if(type < 0 || type >= NB_TYPES){
        return -2;
    }
    Note *n = &m->notes[m->nb_notes];
    n->type   = type;
    n->valeur = valeur;
    n->coeff  = coeff;

    strncpy(n->intitule, intitule, LONG_INTITULE - 1);
    n->intitule[LONG_INTITULE - 1] = '\0';
    for(char *p = n->intitule; *p != '\0'; p++){
        if(*p == ';' || *p == '\n' || *p == '\r'){ *p = ' '; }
    }

    m->nb_notes += 1;
    return 0;
}
