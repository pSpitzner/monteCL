#ifndef H_ASYSTEM
#define H_ASYSTEM

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include "string.h"
#include "gd.h"

#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#include "cllogger.h"
#include "clbuffer.h"
#include "clcontext.h"
#include "clkernel.h"
#include "clexport.h"
#include "parameters.h"

class asystem {
  public:
    parameters par;
    clcontext *context;
    cllogger *logger;
    cl_int ret;
    long frame_index;

    clbuffer *b_template;

    clkernel *k_init;

    // clexport *exporter[3];
    std::vector<clexport *> v_exporter;

    asystem(clcontext *contextn, cllogger *loggern);
    ~asystem();

    void set_par();
    void init_from_kernel();

    void step();
    void step(int damping, int kx, int ky, int kz);

    void write_files(int step);

    void write_state(std::string s_filename);
    void read_state(std::string s_filename);
};

#endif
