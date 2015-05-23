typedef *t_noeud_ag t_arbre_dyn;

typedef struct t_noeud_ag
{
  int cle;
  t_arbre_dyn fils;
  t_arbre_dyn frere;
} t_noeud_ag;
