#include "Random123/philox.h"
#include "Random123/u01fixedpt.h"



__kernel void k_init_kernel_main(__private parameters par,
                                 __write_only image3d_t b_target)
{

  //periodic bc
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  // setting output
  float4 output = (float4)(0.0f,0.0f,0.0f,0.0f);

  // if ((pos.x+pos.y+pos.z)%2 == 0) output = (float4)(255.0f,0.0f,0.0f,0.0f);
  // else output = (float4)(0.0f,0.0f,255.0f,0.0f);

  uint32_t iteration = 2718;
  uint32_t seed = 3141;
  const philox4x32_key_t k1 = {{pos.x+pos.y*par.sx+pos.z*par.sx*par.sy, 0xdecafbad}};
  const philox4x32_ctr_t c = {{0, seed, iteration, 0xBADC0DED}};

  uint32_t i;
  float r, g, b;
  i = (philox4x32(c, k1).v)[0];
  r = 255.0f*u01fixedpt_open_closed_32_24(i);
  i = (philox4x32(c, k1).v)[1];
  g = 255.0f*u01fixedpt_open_closed_32_24(i);
  i = (philox4x32(c, k1).v)[2];
  b = 255.0f*u01fixedpt_open_closed_32_24(i);

  write_imagef(b_target, (int4)(pos.x, pos.y, pos.z, 0), (float4)(r,g,b,0.0f));
}
