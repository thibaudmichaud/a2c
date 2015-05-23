typedef struct t_noeud_bin
{
  int cle;
  struct t_noeud_bin *fg;
  struct t_noeud_bin *fd;
} t_noeud_bin;
