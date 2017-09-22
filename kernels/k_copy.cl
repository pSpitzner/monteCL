__kernel void k_copy_kernel_main(__private parameters par,
                                 __read_only image3d_t bi_source,
                                 __write_only image3d_t bi_target)
{
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  int4 temp = read_imagei(bi_source, (int4)(pos.x, pos.y, pos.z, 0));
  write_imagei(bi_target, (int4)(pos.x, pos.y, pos.z, 0), temp);
}
