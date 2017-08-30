#include "asystem.h"

void asystem::set_par() {
  par.sx = 100;
  par.sy = 100;
  par.sz = 1;
}

asystem::asystem(clcontext *contextn, cllogger *loggern) {
  context = contextn;
  logger = loggern;
  frame_index = 0;

  set_par();

  // ----------------------------------------------------------------- //
  // buffer //
  // ----------------------------------------------------------------- //

  b_template = new clbuffer(context, "b_temp", par.sx, par.sy, par.sz);

  // ----------------------------------------------------------------- //
  // kernel //
  // ----------------------------------------------------------------- //

  k_init = new clkernel(context, par, "./kernels/k_init.cl");

  // ----------------------------------------------------------------- //
  // bindings //
  // ----------------------------------------------------------------- //

  // init
  k_init->bind("b_target", b_template);

  // ----------------------------------------------------------------- //
  // exporter //
  // ----------------------------------------------------------------- //
  // syntax: (... "name", "src", ... int direction_xz/xy/zy, int offset, bool img_, bool ts_, bool vp_)

  // cutplanes XY
  v_exporter.push_back(new clexport(context, "XY_plain", "./kernels/exporter/ke_plain.cl", par, b_template, 2, 0,  1,1,0  ));
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
  b_template->write_raw(s_filename+"/s0.dat");
}

void asystem::read_state(std::string s_filename) {
  logger->log(0, "Reading states from %s\n", s_filename.c_str());
  b_template->read_raw(s_filename+"/s0.dat");
  b_template->ram2device();

  write_files(frame_index);
  frame_index+=1;
}

void asystem::init_from_kernel() {
  logger->log(0, "Init from kernel\n");
  k_init->step(par.sx, par.sy, par.sz);
}

void asystem::step() {
  // overload for default arguments
  step(0, par.sx, par.sy, par.sz);
}

void asystem::step(int damping, int kx, int ky, int kz) {

  frame_index += 1;
}



