__kernel void k_magnetisation_kernel_main(__private parameters par,
                                          __read_only image3d_t bi_source,
                                          __write_only image3d_t bf_mag)
{
  float mag = 0;

  for (int x = 0; x < par.sx; x++) {
    for (int y = 0; y < par.sy; y++) {
      for (int z = 0; z < par.sz; z++) {
        mag+=(read_imagei(bi_source, (int4)(x, y, z, 0)).s0 > 0 ? 1.0f : 0.0f);
        // mag+=read_imagef(bi_source, (int4)(x, y, z, 0)).s0;
      }
    }
  }

  write_imagef(bf_mag, (int4)(0,0,0,0), (float4)(mag,0,0,0));
}
