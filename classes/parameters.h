#ifndef Tropos_parameters_h
#define Tropos_parameters_h

typedef struct {
  int sx, sy, sz; //unsigned long = size_t
  int s[3]; //unsigned long = size_t
  int dim;
  int seed;
  float alpha;
  float p_cons;
  float p_mean;
  float p_devi;

  float dx, dy, dz;
  float dT;

} parameters;




#endif
