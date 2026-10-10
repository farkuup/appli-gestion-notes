#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include "moteur.h"



//___________________________________________________FONCTIONS___________________________________________________
void vider_tampon(){
    int c = getchar();
    while(c != '\n' && c != EOF){
        c = getchar();
    }
}
void sauvegarder(const UE *tab, int n){
    FILE *f = fopen("notes.txt", "w");
    if(f == NULL){ printf("ERREUR : Ouverture du fichier impossible\n"); return;}
    for(int i=0; i<n; i++){
        for(int j=0; j<tab[i].nb_matieres;j++){
            for(int k=0; k<tab[i].matieres[j].nb_notes; k++){
                const Note *no = &tab[i].matieres[j].notes[k];
                // UE ; matière ; type ; valeur ; coeff ; intitulé (texte libre, en dernier)
                fprintf(f, "%d;%d;%d;%.2f;%.2f;%s\n", i, j, no->type, no->valeur, no->coeff, no->intitule);
            }
        }
    }
    fclose(f);
}


void afficher_UE(const UE *ue){
    float moy = moyenne_UE(ue);
    if(moy<0){
        printf("%s: pas de notes...\n", ue->nom);
    }else{
        printf("%s : %.2f\n", ue->nom, moy);
    }
}


int choisir_UE(const UE *tab, int n){
    for(int i=0; i<n; i++){
        printf("%d. %s\n", i+1, tab[i].nom);
    }
    printf("Choix : ");
    int choix = -1;
    int acc = scanf("%d", &choix);
    if( (acc != 1) || (choix > n) || (choix < 1) ){
        printf("ERREUR : Saisie incorrecte\n");
        vider_tampon();
        return -1;
    }
    return choix - 1;
}
int choisir_matiere(const UE *ue){
    for(int i=0; i< ue->nb_matieres; i++){
        printf("%d. %s\n", i+1, ue->matieres[i].nom);
    }
    printf("Choix : ");
    int choix;
    int acc = scanf("%d", &choix);
    if( (acc != 1) || (choix > ue->nb_matieres) || (choix < 1) ){
        printf("ERREUR : Saisie incorrecte\n");
        vider_tampon();
        return -1;
    }
    return choix - 1;
}

// Affiche toutes les matières numérotées (avec leur UE) et renvoie celle choisie.
// Renvoie {-1, -1} si la saisie est invalide.
Indice choisir_matiere_globale(const UE *tab, int n){
    Indice vide = {-1, -1};
    int total = nombre_matieres(tab, n);
    for(int k=0; k<total; k++){
        Indice p = trouver_matiere(tab, n, k);
        printf("%2d. %s\n", k+1, tab[p.UE].matieres[p.MAT].nom);
    }
    printf("Matière : ");
    int choix = -1;
    int acc = scanf("%d", &choix);
    if( (acc != 1) || (choix > total) || (choix < 1) ){
        printf("ERREUR : Saisie incorrecte\n");
        vider_tampon();
        return vide;
    }
    return trouver_matiere(tab, n, choix - 1);
}
int choisir_type(){
    for(int i=0; i<NB_TYPES; i++){
        printf("%d. %s\n", i+1, nom_type(i));
    }
    printf("Type : ");
    int choix = -1;
    int acc = scanf("%d", &choix);
    if( (acc != 1) || (choix > NB_TYPES) || (choix < 1) ){
        printf("ERREUR : Saisie incorrecte\n");
        vider_tampon();
        return -1;
    }
    return choix - 1;
}
// Lit une ligne de texte (avec espaces) dans dest, sans le retour à la ligne.
void lire_ligne(char *dest, int taille){
    vider_tampon();   // jette le reste de la ligne précédente
    if(fgets(dest, taille, stdin) == NULL){ dest[0] = '\0'; return; }
    dest[strcspn(dest, "\n")] = '\0';
}

void charger(UE *tab, int n){
    FILE *f = fopen("notes.txt", "r"); if(f==NULL){return;}
    int i, j, type;
    float valeur, coeff;
    char intitule[LONG_INTITULE];

    // %63[^\n] lit tout le reste de la ligne (les espaces compris)
    while(fscanf(f, "%d;%d;%d;%f;%f;%63[^\n]", &i, &j, &type, &valeur, &coeff, intitule) == 6){
        if( (i>=0 && i<n) && (j>=0 && j<(tab[i].nb_matieres)) ){
            ajouter_note(&tab[i].matieres[j], type, intitule, valeur, coeff);
        }
    }
    fclose(f);
}
void sous_limite_2(const UE *tab, int n){
    Indice r = sous_limite_UE(tab, n);
    if(r.UE != -1){
        printf("Attention moyenne critique en %s : %.2f (< %.2f)\n", tab[r.UE].nom, moyenne_UE(&tab[r.UE]), tab[r.UE].limite);
    }
    Indice s = sous_limite_mat(tab, n);
    if(s.MAT != -1){
        printf("Attention moyenne critique en %s : %.2f (< %.2f)\n", tab[s.UE].matieres[s.MAT].nom, moyenne_matiere(&tab[s.UE].matieres[s.MAT]), tab[s.UE].matieres[s.MAT].limite);
    }
    
}
void afficher_moyennes(const UE *tab, int n){   
    for(int i=0; i<n; i++){
        afficher_UE(&tab[i]);
    }
    float moy = moyenne_generale(tab, n);
    if(moy<0){
        printf("ERREUR : Pas de notes...\n");
    }else{
        printf("Moyenne générale : %.2f\n", moy);
        sous_limite_2(tab,n);
        if(moy<10){
            printf("Attention moyenne générale insuffisane ! (<10)\n");
        }
    }
}
//__________________________________________________MAIN_________________________________________________
int main(){
    UE tab[MAX_UE] = {0};
    int ligne_erreur;
    int nb_ue = charger_structure("structure.txt", tab, MAX_UE, &ligne_erreur);
    if(nb_ue < 0){
        if(ligne_erreur == 0){
            printf("ERREUR : impossible d'ouvrir structure.txt (lance le programme depuis son dossier)\n");
        }else{
            printf("ERREUR : structure.txt est incorrect à la ligne %d\n", ligne_erreur);
        }
        return 1;
    }

    int choix=-1;
    charger(tab, nb_ue);
    
    do{
        printf("\n1. Afficher les moyennes\n");
            printf("2. Ajouter une note\n");
            printf("0. Quitter\n\n");
            printf("Choix : ");
            int lu = scanf("%d", &choix); putchar('\n');
                if(lu != 1){
                    printf("ERREUR : Saisie incorrecte\n");
                    vider_tampon();
                    choix = -1;
                    continue;
                }

        if(choix==1){
            afficher_moyennes(tab, nb_ue);

        }else if(choix==0){
            sauvegarder(tab, nb_ue);
            printf("\nAu revoir !\n");

        }else if(choix==2){ putchar('\n');
            Indice cible = choisir_matiere_globale(tab, nb_ue); if(cible.UE == -1){ continue;} putchar('\n');
            int type = choisir_type(); if(type == -1){ continue;} putchar('\n');

            char intitule[LONG_INTITULE];
            printf("Intitulé (ex. DST 1) : ");
            lire_ligne(intitule, LONG_INTITULE);

            float valeur, coeff;
            printf("Insérer la note : ");
                int acc = scanf("%f", &valeur);
                if(acc != 1 || valeur < 0 || valeur > 20){ vider_tampon(); printf("ERREUR : Note invalide (entre 0 et 20)\n");continue;}

            printf("...et son coeff : ");
                int acc1 = scanf("%f", &coeff);
                if(acc1 != 1 || coeff <= 0 || coeff > 5){ vider_tampon(); printf("ERREUR : Coeff invalide (entre 0 et 5)\n"); continue;}

            if(ajouter_note(&tab[cible.UE].matieres[cible.MAT], type, intitule, valeur, coeff) != 0){
                printf("ERREUR : Trop de notes !\n");
            }else{
                printf("Note ajoutée !\n");
            }

        }else{ printf("ERREUR : Choix invalide !\n");}

    } while (choix !=0);
}