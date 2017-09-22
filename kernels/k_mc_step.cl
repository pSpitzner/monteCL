#include "Random123/philox.h"
#include "Random123/u01fixedpt.h"

__private position random_neighbour(parameters par, position pos, float r) {
  int dim = 2;

  int cs = int(r*(float)(dim));
  int pm = (fmod(r*(float)(dim),1.0f) >= 0.5 ? 1 : -1);

  switch (cs) {
    case 0 : return (pm < 0 ? get_pos_bc(par, pos.xl, pos.y , pos.z ) : get_pos_bc(par, pos.xr, pos.y , pos.z ));
    case 1 : return (pm < 0 ? get_pos_bc(par, pos.x , pos.yl, pos.z ) : get_pos_bc(par, pos.x , pos.yr, pos.z ));
    case 2 : return (pm < 0 ? get_pos_bc(par, pos.x , pos.y , pos.zl) : get_pos_bc(par, pos.x , pos.y , pos.zr));
  }
}

__kernel void k_mc_step_kernel_main(__private parameters par,
                                    __private uint32_t iteration,
                                    __read_only image3d_t bi_source,
                                    __write_only image3d_t bi_target)
{

  //periodic bc
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  // we assume that bi_target is filled with 0 everywhere
  uint32_t state = read_imagei(bi_source, (int4)(pos.x, pos.y, pos.z, 0)).s0;

  // RNG
  // need 5 random numbers in total - 2 for box mueller, 1 for scheme, 1 for dying, maybe 1 for neighbour
  // 4 provided by philox
  uint32_t seed = uint32_t(par.seed);
  philox4x32_key_t k1 = {{pos.x+pos.y*par.sx+pos.z*par.sx*par.sy, 0xdecafbad}};
  philox4x32_ctr_t c = {{0, seed, iteration, 0xBADC0DED}};
  float u0 = u01fixedpt_open_open_32_24((philox4x32(c, k1).v)[0]); // dont want the log(0) in box müller
  float u1 = u01fixedpt_closed_open_32_24((philox4x32(c, k1).v)[1]);
  float u2 = u01fixedpt_closed_open_32_24((philox4x32(c, k1).v)[2]);
  float u3 = u01fixedpt_closed_open_32_24((philox4x32(c, k1).v)[3]);
  c.v[0]+=1;
  float u4 = u01fixedpt_closed_open_32_24((philox4x32(c, k1).v)[0]);

  // box müller
  float g0, g1;
  g0 = par.p_devi*sqrt(-2.0 * log(u0)) * cospi(2.0f*u1) + par.p_mean;
  g1 = par.p_devi*sqrt(-2.0 * log(u0)) * sinpi(2.0f*u1) + par.p_mean;

  float p_of_t = g1;

  if (p_of_t < 0.0) p_of_t = 0.0; else if (p_of_t > 1.0) p_of_t = 1.0;
  float p = (u2 <= par.alpha ? p_of_t : par.p_cons);

  // avoid problems if state > 1...
  if (state >= 1 && u3 > p) state = 0;  // dying with 1-p, cleanup old
  if (state >= 1) {
    write_imagei(bi_target, (int4)(pos.x, pos.y, pos.z, 0), (int4)(1,0,0,0));
    position pos_n = random_neighbour(par, pos, u4);
    // printf("%d %d %d \t %d %d %d\n", pos.x, pos.y, pos.z,  pos_n.x, pos_n.y, pos_n.z);
    write_imagei(bi_target, (int4)(pos_n.x, pos_n.y, pos_n.z, 0), (int4)(1,0,0,0));
  }

}
