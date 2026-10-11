#include <stdio.h>
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

int charger_structure(const char *chemin, UE *tab, int max_ue, int *ligne_erreur){
    *ligne_erreur = 0;
    FILE *f = fopen(chemin, "r");
    if(f == NULL){ return -1; }

    char ligne[256];
    int num = 0;       // numéro de la ligne lue
    int nb_ue = 0;     // UE déjà ouvertes

    while(fgets(ligne, sizeof(ligne), f) != NULL){
        num++;
        for(char *p = ligne; *p != '\0'; p++){
            if(*p == ','){ *p = '.'; }
        }

        if(ligne[0] == '#' || ligne[0] == '\n' || ligne[0] == '\r' || ligne[0] == '\0'){
            continue;
        }

        char nom[32];
        float coeff, limite;

        if(strncmp(ligne, "UE;", 3) == 0){
            if(sscanf(ligne, "UE;%31[^;];%f;%f", nom, &coeff, &limite) != 3 || nb_ue >= max_ue){
                *ligne_erreur = num; fclose(f); return -1;
            }
            UE *u = &tab[nb_ue];
            memset(u, 0, sizeof(UE));
            strcpy(u->nom, nom);
            u->coeff  = coeff;
            u->limite = limite;
            nb_ue++;

        }else if(strncmp(ligne, "MATIERE;", 8) == 0){
            if(nb_ue == 0 || sscanf(ligne, "MATIERE;%31[^;];%f;%f", nom, &coeff, &limite) != 3){
                *ligne_erreur = num; fclose(f); return -1;
            }
            UE *u = &tab[nb_ue - 1];
            if(u->nb_matieres >= MAX_MATIERES){
                *ligne_erreur = num; fclose(f); return -1;
            }
            Matiere *m = &u->matieres[u->nb_matieres];
            memset(m, 0, sizeof(Matiere));
            strcpy(m->nom, nom);
            m->coeff  = coeff;
            m->limite = limite;
            u->nb_matieres++;

        }else{
            *ligne_erreur = num; fclose(f); return -1;
        }
    }
    fclose(f);

    if(nb_ue == 0){ *ligne_erreur = num; return -1; }
    return nb_ue;
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

int supprimer_note(Matiere *m, int i){
    if(i < 0 || i >= m->nb_notes){ return -1; }
    for(int k=i; k<m->nb_notes-1; k++){
        m->notes[k] = m->notes[k+1];
    }
    m->nb_notes--;
    return 0;
}

int exporter_notes(const UE *tab, int n, char *sortie, int taille){
    int ecrit = 0;
    sortie[0] = '\0';
    for(int i=0; i<n; i++){
        for(int j=0; j<tab[i].nb_matieres; j++){
            const Matiere *m = &tab[i].matieres[j];
            for(int k=0; k<m->nb_notes; k++){
                const Note *no = &m->notes[k];
                int r = snprintf(sortie + ecrit, taille - ecrit, "%d;%d;%d;%.2f;%.2f;%s\n",
                                 i, j, no->type, no->valeur, no->coeff, no->intitule);
                if(r < 0 || r >= taille - ecrit){ return -1; }
                ecrit += r;
            }
        }
    }
    return ecrit;
}

int importer_notes(UE *tab, int n, const char *texte, int *ligne_erreur){
    *ligne_erreur = 0;
    for(int i=0; i<n; i++){
        for(int j=0; j<tab[i].nb_matieres; j++){ tab[i].matieres[j].nb_notes = 0; }
    }
    int lues = 0, num = 0;
    const char *p = texte;
    while(*p != '\0'){
        char ligne[160];
        int len = 0;
        while(*p != '\0' && *p != '\n'){
            if(len < (int)sizeof(ligne) - 1 && *p != '\r'){ ligne[len++] = *p; }
            p++;
        }
        if(*p == '\n'){ p++; }
        ligne[len] = '\0';
        num++;
        if(len == 0){ continue; }

        int u, m, type;
        float valeur, coeff;
        char intitule[LONG_INTITULE];
        if(sscanf(ligne, "%d;%d;%d;%f;%f;%63[^\n]", &u, &m, &type, &valeur, &coeff, intitule) != 6
           || u < 0 || u >= n || m < 0 || m >= tab[u].nb_matieres
           || ajouter_note(&tab[u].matieres[m], type, intitule, valeur, coeff) != 0){
            *ligne_erreur = num;
            return -1;
        }
        lues++;
    }
    return lues;
}
