#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define MAX_NOTES     20
#define MAX_MATIERES   4
#define MAX_UE         5

//_______________________________________________________TYPES_________________________________________________
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

void afficher_UE(const UE *ue){
    float moy = moyenne_UE(ue);
    if(moy<0){
        printf("%s: pas de notes...\n", ue->nom);
    }else{
        printf("%s : %.2f\n", ue->nom, moy);
    }
}
int sous_limite(const UE *tab, int n){
    int alertes = 0;
    for(int i=0; i<n; i++){
        float moy = moyenne_UE(&tab[i]);
        if(moy >= 0 && moy < tab[i].limite){
            printf("Moyenne critique en %s : %.2f (moy. minimale : %.2f)\n", tab[i].nom, moy, tab[i].limite);
            alertes++;
        }
        for(int j=0;j< tab[i].nb_matieres ; j++){
            float moy_mat = moyenne_matiere(&tab[i].matieres[j]);
            if(moy_mat >= 0 && moy_mat < tab[i].matieres[j].limite){
                printf("Moyenne critique en  %s : %.2f (moy. minimale : %.2f)\n", tab[i].matieres[j].nom, moy_mat, tab[i].matieres[j].limite);
                alertes++;
            }
        }
    }   
    return alertes;
}

typedef struct{
    int UE;
    int MAT;
}Indice;
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

void ajouter_note(Matiere *m, float valeur, float coeff){
    if(m->nb_notes>=MAX_NOTES){
        printf("ERREUR : Trop de notes !\n");
        return;
    }else{
        m->notes[m->nb_notes].coeff=coeff;
        m->notes[m->nb_notes].valeur=valeur;
        m->nb_notes += 1;
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

void MCC(){
    printf("Pas dispo...");
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

            ajouter_note(&tab[i_ue].matieres[i_mat], valeur, coeff);
            printf("Note ajoutée !\n");

        }else{ printf("ERREUR : Choix invalide !\n");}

    } while (choix !=0);
}