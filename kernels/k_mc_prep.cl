__kernel void k_mc_prep_kernel_main(__private parameters par,
                                    __write_only image3d_t bi_target)
{

  //periodic bc
  position pos = get_pos_bc(par, get_global_id(0), get_global_id(1), get_global_id(2));

  // set everything to zero
  write_imagei(bi_target, (int4)(pos.x, pos.y, pos.z, 0), (int4)(0,0,0,0));

}
