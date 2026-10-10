#ifndef MOTEUR_H
#define MOTEUR_H

#define MAX_NOTES     20
#define MAX_MATIERES   4
#define MAX_UE         5

#define NB_TYPES        5
#define LONG_INTITULE  64

// Un type de note est un numéro : son nom est dans nom_type(numero).
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

// Lit le fichier de structure (voir structure.txt) et remplit tab.
// Renvoie le nombre d'UE lues (au moins 1), ou -1 en cas d'erreur.
// En cas d'erreur, *ligne_erreur vaut le numéro de la ligne fautive
// (0 si le fichier n'a pas pu être ouvert).
int charger_structure(const char *chemin, UE *tab, int max_ue, int *ligne_erreur);

// Nombre total de matières, toutes UE confondues.
int nombre_matieres(const UE *tab, int n);
// Retrouve (UE, matière) à partir d'un numéro "global" de matière (à partir de 0,
// dans l'ordre des UE puis des matières). Renvoie {-1, -1} si le numéro n'existe pas.
Indice trouver_matiere(const UE *tab, int n, int numero);
// Nom d'un type de note ("DS", "TP"...), ou "?" si le numéro est invalide.
const char *nom_type(int type);

// Renvoie 0 si la note est ajoutée, -1 s'il y a déjà trop de notes,
// -2 si le type est invalide. Les ';' et retours à la ligne de l'intitulé
// sont remplacés par des espaces (ils casseraient le fichier de sauvegarde).
int ajouter_note(Matiere *m, int type, const char *intitule, float valeur, float coeff);

#endif
