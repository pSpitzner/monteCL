__kernel void ke_plain_kernel_main(__private parameters par,
                                   __private uint ref,
                                   __private uint dim,
                                   __read_only image3d_t b_source,
                                   __write_only image3d_t b_target)
{
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  float4 w = (float4)(0.0f);

  if      (dim == 0) w = read_f4(pos.x+ref, pos.y,     pos.z,     b_source);
  else if (dim == 1) w = read_f4(pos.x    , pos.y+ref, pos.z,     b_source);
  else if (dim == 2) w = read_f4(pos.x    , pos.y,     pos.z+ref, b_source);


  float4 rgba = w;
  write_f4(pos.x, pos.y, pos.z, rgba, b_target);
}
