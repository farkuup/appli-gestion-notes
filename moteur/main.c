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
                fprintf(f, "%d %d %.2f %.2f\n", i, j, tab[i].matieres[j].notes[k].valeur, tab[i].matieres[j].notes[k].coeff);
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

void charger(UE *tab, int n){
    FILE *f = fopen("notes.txt", "r"); if(f==NULL){return;}
    int i, j;
    float valeur, coeff;

    while((fscanf(f, "%d %d %f %f", &i, &j, &valeur, &coeff) == 4)){
        if( (i>=0 && i<n) && (j>=0 && j<(tab[i].nb_matieres)) ){
            ajouter_note(&tab[i].matieres[j], valeur, coeff);
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
    UE ue4 = {0};
        strcpy(ue4.nom, "Option"); ue4.coeff = 3; ue4.nb_matieres = 1;
            strcpy(ue4.matieres[0].nom, "Réseaux"); ue4.matieres[0].coeff = 3; ue4.matieres[0].nb_notes = 0;

    UE ue3 = {0};
        strcpy(ue3.nom, "Culture Ingé"); ue3.coeff = 5.5; ue3.nb_matieres = 3; ue3.limite = 10;
            strcpy(ue3.matieres[0].nom, "Anglais"); ue3.matieres[0].coeff = 2.5; ue3.matieres[0].nb_notes = 0; ue3.matieres[0].limite = 8;
            strcpy(ue3.matieres[1].nom, "Sport"); ue3.matieres[1].coeff = 1.5; ue3.matieres[1].nb_notes = 0; ue3.matieres[1].limite = 8;
            strcpy(ue3.matieres[2].nom, "TEC"); ue3.matieres[2].coeff = 1.5; ue3.matieres[2].nb_notes = 0; ue3.matieres[2].limite = 10;

    UE ue2 = {0};
        strcpy(ue2.nom, "Physique-Chimie"); ue2.coeff = 9; ue2.nb_matieres = 2; ue2.limite = 7;
            strcpy(ue2.matieres[0].nom, "Physique"); ue2.matieres[0].coeff = 5; ue2.matieres[0].nb_notes = 0;
            strcpy(ue2.matieres[1].nom, "Chimie"); ue2.matieres[1].coeff = 4; ue2.matieres[1].nb_notes = 0;

    UE ue1 = {0};
    strcpy(ue1.nom, "Maths-Info"); ue1.coeff = 12; ue1.nb_matieres = 3; ue1.limite = 7;
        strcpy(ue1.matieres[0].nom, "Analyse"); ue1.matieres[0].coeff = 4; ue1.matieres[0].nb_notes = 0;
        strcpy(ue1.matieres[1].nom, "Algèbre"); ue1.matieres[1].coeff = 4; ue1.matieres[1].nb_notes = 0;
        strcpy(ue1.matieres[2].nom, "Info"); ue1.matieres[2].coeff = 4; ue1.matieres[2].nb_notes = 0;


    UE tab[4] = { ue1, ue2, ue3, ue4 };

    int choix=-1;
    int nb_ue = sizeof(tab) / sizeof(tab[0]);
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
            int i_ue  = choisir_UE(tab,nb_ue); if(i_ue == -1){ continue;} putchar('\n');
            int i_mat = choisir_matiere(&tab[i_ue]); if(i_mat == -1){ continue;}
            float valeur, coeff;

            printf("Insérer la note : ");
                int acc = scanf("%f", &valeur);
                if(acc != 1 || valeur < 0 || valeur > 20){ vider_tampon(); printf("ERREUR : Note invalide (entre 0 et 20)\n");continue;}

            printf("...et son coeff : ");
                int acc1 = scanf("%f", &coeff);
                if(acc1 != 1 || coeff <= 0 || coeff > 5){ vider_tampon(); printf("ERREUR : Coeff invalide (entre 0 et 5)\n"); continue;}

            if(ajouter_note(&tab[i_ue].matieres[i_mat], valeur, coeff) == -1){
                printf("ERREUR : Trop de notes !\n");
            }else{
                printf("Note ajoutée !\n");
            }

        }else{ printf("ERREUR : Choix invalide !\n");}

    } while (choix !=0);
}