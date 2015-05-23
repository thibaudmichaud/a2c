typedef *s_som t_listsom;
typedef *s_adj t_listadj;

typedef struct s_som
{
  int som;
  t_listadj succ;
  t_listadj pred;
  t_listsom suiv;
} s_som;

typedef struct s_adj
{
  int nb;
  float cout;
  t_listadj suiv;
  t_listsom som;
} s_adj;

typedef struct t_graph_stat
{
  booleen orient;
  int ordre;
  t_mat_adj adj;
  t_mat_cout cout;
} t_graph_stat;
