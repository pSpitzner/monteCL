constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

// header
typedef struct {
  int x, y, z;
  int xr, yr, zr;
  int xl, yl, zl;
  int xrr, yrr, zrr;
  int xll, yll, zll;
  int s_xl, s_xr, s_yl, s_yr, s_zl, s_zr;
  int s_yll, s_yrr, s_xll, s_xrr, s_zll, s_zrr;
  float ulf, urf, vlf, vrf, wlf, wrf;
} position;

__private position get_pos_bc(parameters par, int x, int y, int z);
void write_f4(int x, int y, int z, float4 f, __write_only image3d_t buf_0);
void write_f8(int x, int y, int z, float8 f, __write_only image3d_t buf_0, __write_only image3d_t buf_1);
__private float4 read_f4(int x, int y, int z, __read_only image3d_t buf_0);
__private float8 read_f8(int x, int y, int z, __read_only image3d_t buf_0, __read_only image3d_t buf_1);


__private position get_pos_bc(parameters par, int x, int y, int z) {
  position pos;
  pos.x = x;
  pos.y = y;
  pos.z = z;

  // periodic bc
  pos.xr  = ((pos.x  + 1 == par.sx) ? 0          : pos.x + 1 );
  pos.xl  = ((pos.x  - 1 < 0)       ? par.sx - 1 : pos.x - 1 );
  pos.yr  = ((pos.y  + 1 == par.sy) ? 0          : pos.y + 1 );
  pos.yl  = ((pos.y  - 1 < 0)       ? par.sy - 1 : pos.y - 1 );
  pos.zr  = ((pos.z  + 1 == par.sz) ? 0          : pos.z + 1 );
  pos.zl  = ((pos.z  - 1 < 0)       ? par.sz - 1 : pos.z - 1 );

  pos.xrr = ((pos.xr + 1 == par.sx) ? 0          : pos.xr + 1 );
  pos.xll = ((pos.xl - 1 < 0)       ? par.sx - 1 : pos.xl - 1 );
  pos.yrr = ((pos.yr + 1 == par.sy) ? 0          : pos.yr + 1 );
  pos.yll = ((pos.yl - 1 < 0)       ? par.sy - 1 : pos.yl - 1 );
  pos.zrr = ((pos.zr + 1 == par.sz) ? 0          : pos.zr + 1 );
  pos.zll = ((pos.zl - 1 < 0)       ? par.sz - 1 : pos.zl - 1 );

  // fixing select bc
  // 1 normal cell, -1 boundary cell
  // pos.s_xl = 1;
  // pos.s_xr = 1;
  // pos.s_yl = 1;
  // pos.s_yr = 1;
  // pos.s_zl = 1;
  // pos.s_zr = 1;

  // pos.s_xll = 1;
  // pos.s_xrr = 1;
  // pos.s_yll = 1;
  // pos.s_yrr = 1;
  // pos.s_zll = 1;
  // pos.s_zrr = 1;

  // if (pos.z==par.sz-1) {pos.zr  = pos.z; pos.zrr = pos.zl; pos.s_zr = -1; pos.s_zrr=-1;}
  // if (pos.z==par.sz-2) {pos.zrr = pos.zr; pos.s_zrr=-1;}
  // if (pos.z==       0) {pos.zl  = pos.z; pos.zll = pos.zr; pos.s_zl = -1; pos.s_zll=-1;}
  // if (pos.z==       1) {pos.zll = pos.zl; pos.s_zll=-1;}

  return pos;
}

__private float4 read_f4(int x, int y, int z, __read_only image3d_t buf_0) {
  int4 pos = (int4)(x, y, z, 0);
  float4 f_0 = read_imagef(buf_0, pos);
  return f_0;
}
__private float8 read_f8(int x, int y, int z, __read_only image3d_t buf_0, __read_only image3d_t buf_1) {
  int4 pos = (int4)(x, y, z, 0);
  float4 f_0 = read_imagef(buf_0, pos);
  float4 f_1 = read_imagef(buf_1, pos);
  return (float8)(f_0, f_1);
}

void write_f4(int x, int y, int z, float4 f, __write_only image3d_t buf_0) {
  int4 pos = (int4)(x, y, z, 0);
  write_imagef(buf_0, pos, (float4)(f.s0, f.s1, f.s2, f.s3));
}
void write_f8(int x, int y, int z, float8 f, __write_only image3d_t buf_0, __write_only image3d_t buf_1) {
  int4 pos = (int4)(x, y, z, 0);
  write_imagef(buf_0, pos, (float4)(f.s0, f.s1, f.s2, f.s3));
  write_imagef(buf_1, pos, (float4)(f.s4, f.s5, f.s6, f.s7));
}













// have some decent number of lines in header to find kernel debug messages easily
