typedef *t_noeud_nuplet t_arbre_nuplets;

typedef struct t_noeud_nuplet
{
  int cle;
  t_noeud_nuplet fils;
  int nbFils;
} t_noeud_nuplet;
