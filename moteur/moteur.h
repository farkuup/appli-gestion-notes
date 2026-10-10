#ifndef MOTEUR_H
#define MOTEUR_H

#define MAX_NOTES     20
#define MAX_MATIERES   4
#define MAX_UE         5

typedef struct {
    char type[32]; // DS, TP, Khôlle, CC
    float valeur; // /20
    float coeff;
}Note;
typedef struct {
    char nom[32];
    float coeff;
    Note notes[MAX_NOTES];
    int nb_notes;
    float limite;
}Matiere;
typedef struct {
    char nom[32];
    float coeff;
    Matiere matieres[MAX_MATIERES];
    int nb_matieres;
    float limite;
}UE;
typedef struct{
    int UE;
    int MAT;
}Indice;

float moyenne_matiere(const Matiere *m);
float moyenne_UE(const UE *ue);
float moyenne_generale(const UE *tab, int n);
Indice sous_limite_UE(const UE *tab, int n);
Indice sous_limite_mat(const UE *tab, int n);
// Renvoie 0 si la note est ajoutée, -1 s'il y a déjà trop de notes.
int ajouter_note(Matiere *m, float valeur, float coeff);

#endif
