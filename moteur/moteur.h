#ifndef MOTEUR_H
#define MOTEUR_H

#define MAX_NOTES     20
#define MAX_MATIERES   4
#define MAX_UE         5

#define NB_TYPES        5
#define LONG_INTITULE  64

typedef struct {
    int type;                       // 0 = DS, 1 = TP, 2 = Khôlle, 3 = CC, 4 = Autre
    char intitule[LONG_INTITULE];   // ex. "DST 1"
    float valeur;                   // /20
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

int charger_structure(const char *chemin, UE *tab, int max_ue, int *ligne_erreur); /* lit le fichier de structure (voir structure.txt) et remplit tab. Renvoie le nombre d'UE lues (au moins 1), ou -1 en cas d'erreur
En cas d'erreur, *ligne_erreur vaut le numéro de la ligne fautive (0 si le fichier n'a pas pu être ouvert) */

int nombre_matieres(const UE *tab, int n); // nb total de matières
Indice trouver_matiere(const UE *tab, int n, int numero);

const char *nom_type(int type); // nom d'un type de note ("DS", "TP"...), ou "?" si le numéro est invalide

int ajouter_note(Matiere *m, int type, const char *intitule, float valeur, float coeff); // renvoie 0 si la note est ajoutée, -1 s'il y a déjà trop de notes, -2 si le type est invalide
int supprimer_note(Matiere *m, int i);

int exporter_notes(const UE *tab, int n, char *sortie, int taille); // écrit toutes les notes dans `sortie` (UE;matière;type;valeur;coeff;intitulé)*/
int importer_notes(UE *tab, int n, const char *texte, int *ligne_erreur); // efface les notes de tab puis relit le texte au format ↑
// (*ligne_erreur = son numéro).

#endif
