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

int ajouter_note(Matiere *m, float valeur, float coeff){
    if(m->nb_notes >= MAX_NOTES){
        return -1;
    }
    m->notes[m->nb_notes].coeff  = coeff;
    m->notes[m->nb_notes].valeur = valeur;
    m->nb_notes += 1;
    return 0;
}
