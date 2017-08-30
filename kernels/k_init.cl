__kernel void k_init_kernel_main(__private parameters par,
                                 __write_only image3d_t b_target)
{

  //periodic bc
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  // setting output
  float4 output = (float4)(0.0f,0.0f,0.0f,0.0f);

  if ((pos.x+pos.y+pos.z)%2 == 0) output = (float4)(255.0f,0.0f,0.0f,0.0f);
  else output = (float4)(0.0f,0.0f,255.0f,0.0f);

  write_imagef(b_target, (int4)(pos.x, pos.y, pos.z, 0), output);
}
