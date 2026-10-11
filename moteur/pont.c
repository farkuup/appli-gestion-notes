#include <emscripten.h>
#include <stdio.h>
#include "moteur.h"

#define TAILLE_EXPORT 16384

static UE tab[MAX_UE];
static int nb_ue = 0;
static int ligne_err = 0;
static char export_buf[TAILLE_EXPORT];

static Matiere *mat(int u, int m){
    if(u < 0 || u >= nb_ue || m < 0 || m >= tab[u].nb_matieres){ return NULL; }
    return &tab[u].matieres[m];
}

// __________________________________________________DEMARRAGE________________________________________________
EMSCRIPTEN_KEEPALIVE int sn_init(const char *chemin){ // "sn" comme Sonote
    int r = charger_structure(chemin, tab, MAX_UE, &ligne_err);
    nb_ue = (r > 0) ? r : 0;
    return r;
}
EMSCRIPTEN_KEEPALIVE int sn_ligne_erreur(void){ return ligne_err; }

// __________________________________________________STRUCTURE___________________________________________________________
EMSCRIPTEN_KEEPALIVE int sn_nb_ue(void){ return nb_ue; }
EMSCRIPTEN_KEEPALIVE int sn_nb_mat(int u){ return (u >= 0 && u < nb_ue) ? tab[u].nb_matieres : 0; }
EMSCRIPTEN_KEEPALIVE const char *sn_nom_ue(int u){ return (u >= 0 && u < nb_ue) ? tab[u].nom : ""; }
EMSCRIPTEN_KEEPALIVE float sn_coeff_ue(int u){ return (u >= 0 && u < nb_ue) ? tab[u].coeff : 0; }
EMSCRIPTEN_KEEPALIVE float sn_limite_ue(int u){ return (u >= 0 && u < nb_ue) ? tab[u].limite : 0; }
EMSCRIPTEN_KEEPALIVE const char *sn_nom_mat(int u, int m){ Matiere *x = mat(u, m); return x ? x->nom : ""; }
EMSCRIPTEN_KEEPALIVE float sn_coeff_mat(int u, int m){ Matiere *x = mat(u, m); return x ? x->coeff : 0; }
EMSCRIPTEN_KEEPALIVE float sn_limite_mat(int u, int m){ Matiere *x = mat(u, m); return x ? x->limite : 0; }

//__________________________________________________TYPES DE NOTES___________________________________________________________
EMSCRIPTEN_KEEPALIVE int sn_nb_types(void){ return NB_TYPES; }
EMSCRIPTEN_KEEPALIVE const char *sn_nom_type(int t){ return nom_type(t); }

// ________________________________________________NOTES_________________________________________________________________________
EMSCRIPTEN_KEEPALIVE int sn_nb_notes(int u, int m){ Matiere *x = mat(u, m); return x ? x->nb_notes : 0; }
EMSCRIPTEN_KEEPALIVE int sn_note_type(int u, int m, int k){
    Matiere *x = mat(u, m); return (x && k >= 0 && k < x->nb_notes) ? x->notes[k].type : -1;
}
EMSCRIPTEN_KEEPALIVE const char *sn_note_intitule(int u, int m, int k){
    Matiere *x = mat(u, m); return (x && k >= 0 && k < x->nb_notes) ? x->notes[k].intitule : "";
}
EMSCRIPTEN_KEEPALIVE float sn_note_valeur(int u, int m, int k){
    Matiere *x = mat(u, m); return (x && k >= 0 && k < x->nb_notes) ? x->notes[k].valeur : 0;
}
EMSCRIPTEN_KEEPALIVE float sn_note_coeff(int u, int m, int k){
    Matiere *x = mat(u, m); return (x && k >= 0 && k < x->nb_notes) ? x->notes[k].coeff : 0;
}
// 0 = ok, -1 = trop de notes, -2 = type invalide, -3 = matière inconnue, -4 = valeur/coeff absurde
EMSCRIPTEN_KEEPALIVE int sn_ajouter_note(int u, int m, int type, const char *intitule, float valeur, float coeff){
    Matiere *x = mat(u, m);
    if(x == NULL){ return -3; }
    if(valeur < 0 || valeur > 20 || coeff <= 0){ return -4; }
    return ajouter_note(x, type, intitule, valeur, coeff);
}
EMSCRIPTEN_KEEPALIVE int sn_supprimer_note(int u, int m, int k){
    Matiere *x = mat(u, m);
    return x ? supprimer_note(x, k) : -1;
}

// moyennes : -1 = pas encore de note
EMSCRIPTEN_KEEPALIVE float sn_moy_mat(int u, int m){ Matiere *x = mat(u, m); return x ? moyenne_matiere(x) : -1; }
EMSCRIPTEN_KEEPALIVE float sn_moy_ue(int u){ return (u >= 0 && u < nb_ue) ? moyenne_UE(&tab[u]) : -1; }
EMSCRIPTEN_KEEPALIVE float sn_moy_gen(void){ return moyenne_generale(tab, nb_ue); }

EMSCRIPTEN_KEEPALIVE int sn_mat_sous_limite(int u, int m){ // alertes : 1 si sous la limite, 0 sinon (on peut donc toutes les afficher)
    Matiere *x = mat(u, m);
    if(x == NULL){ return 0; }
    float moy = moyenne_matiere(x);
    return moy >= 0 && moy < x->limite;
}
EMSCRIPTEN_KEEPALIVE int sn_ue_sous_limite(int u){
    if(u < 0 || u >= nb_ue){ return 0; }
    float moy = moyenne_UE(&tab[u]);
    return moy >= 0 && moy < tab[u].limite;
}

EMSCRIPTEN_KEEPALIVE const char *sn_exporter(void){ //sauvegarde : la page garde le texte
    if(exporter_notes(tab, nb_ue, export_buf, TAILLE_EXPORT) < 0){ export_buf[0] = '\0'; }
    return export_buf;
}
EMSCRIPTEN_KEEPALIVE int sn_importer(const char *texte){ // renvoie le nombre de notes lues
    int ligne;
    int r = importer_notes(tab, nb_ue, texte, &ligne);
    return (r < 0) ? -ligne : r;
}
