#include "Random123/philox.h"
#include "Random123/u01fixedpt.h"



__kernel void k_init_kernel_main(__private parameters par,
                                 __write_only image3d_t bi_target)
{

  //periodic bc
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  uint32_t i, state;
  state = 1;
  // uncomment to init randomly
  // uint32_t iteration = 2718;
  // uint32_t seed = 3141;
  // const philox4x32_key_t k1 = {{pos.x+pos.y*par.sx+pos.z*par.sx*par.sy, 0xdecafbad}};
  // const philox4x32_ctr_t c = {{0, seed, iteration, 0xBADC0DED}};

  // i = (philox4x32(c, k1).v)[0];
  // state = (uint32_t)(2.0f*u01fixedpt_closed_open_32_24(i));

  write_imagei(bi_target, (int4)(pos.x, pos.y, pos.z, 0), (int4)(state,0,0,0));
}
