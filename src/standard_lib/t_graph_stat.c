const int Max_sommets = 100;

int t_mat_adj[Max_sommets][Max_sommets];
float t_mat_cout[Max_sommets][Max_sommets];

typedef struct t_graph_stat
{
  booleen orient;
  int ordre;
  t_mat_adj adj;
  t_mat_cout cout;
} t_graph_stat;
