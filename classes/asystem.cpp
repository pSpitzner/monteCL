#include "asystem.h"

void asystem::set_par() {
  par.dim = 2;
  par.s[0] = 12;
  par.s[1] = 12;
  par.s[2] = 1;
  par.sx = par.s[0];
  par.sy = par.s[1];
  par.sz = par.s[2];
  par.seed = 3141;
  par.alpha = 0.5;
  par.p_cons = 0.58;
  par.p_mean = 0.71;
  par.p_devi = 0.53; // sigma, beware: argument for normal_dist in cpp11 is sigmasquared
}

asystem::asystem(clcontext *contextn, cllogger *loggern) {
  context = contextn;
  logger = loggern;
  frame_index = 0;

  set_par();

  // ----------------------------------------------------------------- //
  // buffer //
  // ----------------------------------------------------------------- //

  bi_sys_a = new clbuffer(context, "bi_sys_a", par.sx, par.sy, par.sz, 1);
  bi_sys_b = new clbuffer(context, "bi_sys_b", par.sx, par.sy, par.sz, 1);
  bf_mag   = new clbuffer(context, "bf_mag", 1, 1, 1, 0);

  // ----------------------------------------------------------------- //
  // kernel //
  // ----------------------------------------------------------------- //

  k_init = new clkernel(context, par, "./kernels/k_init.cl");
  k_mag  = new clkernel(context, par, "./kernels/k_magnetisation.cl");
  k_copy = new clkernel(context, par, "./kernels/k_copy.cl");
  k_mc_step = new clkernel(context, par, "./kernels/k_mc_step.cl");
  k_mc_prep = new clkernel(context, par, "./kernels/k_mc_prep.cl");

  // ----------------------------------------------------------------- //
  // bindings //
  // ----------------------------------------------------------------- //

  k_init->bind("bi_target", bi_sys_a);
  k_mag->bind("bi_source", bi_sys_a);
  k_mag->bind("bf_mag", bf_mag);
  k_copy->bind("bi_source", bi_sys_b);
  k_copy->bind("bi_target", bi_sys_a);

  k_mc_prep->bind("bi_target", bi_sys_b);

  k_mc_step->bind("bi_source", bi_sys_a);
  k_mc_step->bind("bi_target", bi_sys_b);

  // ----------------------------------------------------------------- //
  // exporter //
  // ----------------------------------------------------------------- //
  // syntax: (... "name", "src", ... int direction_xz/xy/zy, int offset, bool img_, bool ts_, bool vp_)

  // cutplanes XY
  v_exporter.push_back(new clexport(context, "XY_plain", "./kernels/exporter/ke_plain.cl", par, bi_sys_a, 2, 0,  1,1,0  ));
}

asystem::~asystem() {
}

void asystem::write_files(int step) {
  for (int e = 0; e<v_exporter.size(); e++) {
    v_exporter.at(e)->write_files(step);
  }
}

void asystem::write_state(std::string s_filename) {
  logger->log(0, "\nWriting states to %s\n", s_filename.c_str());
  int ret = system(("mkdir -p " + s_filename).c_str());
  bi_sys_a->write_raw(s_filename+"/s0.dat");
}

void asystem::read_state(std::string s_filename) {
  logger->log(0, "Reading states from %s\n", s_filename.c_str());
  bi_sys_a->read_raw(s_filename+"/s0.dat");
  bi_sys_a->ram2device();

  write_files(frame_index);
  frame_index+=1;
}

void asystem::init_from_kernel() {
  logger->log(0, "Init from kernel\n");
  k_init->step(par.sx, par.sy, par.sz);
  write_files(frame_index);
  frame_index+=1;
}

void asystem::step() {
  // overload for default arguments
  step(0, par.sx, par.sy, par.sz);
}

void asystem::step(int damping, int kx, int ky, int kz) {
  bool simulate = true;
  float avg;
  while (simulate) {
    k_mc_step->bind("iteration", frame_index);
    k_mc_step->step(kx, ky, kz);

    k_copy->step(kx, ky, kz);
    k_mag->step(1,1,1);
    bf_mag->device2ram();

    float mag = bf_mag->get(0,0,0,0);
    avg = ((float)(frame_index)*avg + mag)/(float)(frame_index+1);
    if (mag == 0) simulate = false;

    k_mc_prep->step(kx, ky, kz);
    write_files(frame_index);
    frame_index += 1;
  }
  printf("%f %d\n", avg, frame_index);


}



