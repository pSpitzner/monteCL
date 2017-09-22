#include "clbuffer.h"

clbuffer::clbuffer(clcontext *context_, std::string name_, int sx_, int sy_, int sz_, int datatype_) {
  // texture data
  s_name = name_;
  context = context_;
  logger = context->logger;
  datatype = datatype_;
  sx = sx_;
  sy = sy_;
  sz = sz_;


  cl_image_desc image_desc;
  image_desc.image_type = CL_MEM_OBJECT_IMAGE3D;
  image_desc.image_width  = sx;
  image_desc.image_height = sy;
  image_desc.image_depth  = sz;
  image_desc.image_array_size = 1;
  image_desc.image_row_pitch = 0;
  image_desc.image_slice_pitch = 0;
  image_desc.num_mip_levels = 0;
  image_desc.num_samples = 0;
  image_desc.buffer = NULL;

  cl_image_format image_format;
  image_format.image_channel_order = CL_RGBA;
  if (datatype == 0) {
    data = new float[sx * sy * sz * 4];
    datasize = sx * sy * sz * 4 * sizeof(float);
    image_format.image_channel_data_type = CL_FLOAT;
    buffer = clCreateImage(context->context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, &image_format, &image_desc, data, &ret);
  } else if (datatype == 1) {
    data_i = new int[sx * sy * sz * 4];
    datasize = sx * sy * sz * 4 * sizeof(int);
    image_format.image_channel_data_type = CL_SIGNED_INT32;
    buffer = clCreateImage(context->context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, &image_format, &image_desc, data_i, &ret);
  }



  logger->log(1, ret, "%s\tBuffer Object construction\n", s_name.c_str());

  context->v_bufferList.push_back(this);
}

clbuffer::~clbuffer() {
  clReleaseMemObject(buffer);
  if (datatype == 0) {
    delete data;
  } else if (datatype == 1) {
    delete data_i;
  }
}


int clbuffer::getindex(int x, int y, int z) {
  return 4*(x + sx*y + sx*sy*z);
}

float clbuffer::get(int x, int y, int z, int c) {
  if (datatype!=0) abort();
  return data[getindex(x, y, z) + c];
}

void clbuffer::set(int x, int y, int z, int c, float v) {
  if (datatype!=0) abort();
  data[getindex(x, y, z) + c] = v;
}

float clbuffer::get_i(int x, int y, int z, int c) {
  if (datatype!=1) abort();
  return data_i[getindex(x, y, z) + c];
}

void clbuffer::set_i(int x, int y, int z, int c, int v) {
  if (datatype!=1) abort();
  data_i[getindex(x, y, z) + c] = v;
}

void clbuffer::ram2device() {
  //  ret = clEnqueueWriteBuffer(context->command_queue, buffer, CL_TRUE, 0, datasize, data, 0, NULL, NULL);

  size_t origin[] = {0, 0, 0}; // Defines the offset in pixels in the image from where to write.
  size_t region[] = {size_t(sx), size_t(sy), size_t(sz)}; // Size of object to be transferred

  if (context->profiling) {
    // logger->log(1, ret, "%s\tDevice to Ram\n", s_name.c_str());
    cl_ulong ev_start_time, ev_end_time;
    cl_event profiling_event;
    if (datatype == 0) {
      ret = clEnqueueWriteImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data, 0, NULL, &profiling_event);
    } else if (datatype == 1) {
      ret = clEnqueueWriteImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data_i, 0, NULL, &profiling_event);
    }
    ev_start_time = (cl_ulong)0;
    ev_end_time = (cl_ulong)0;
    size_t return_bytes;
    ret = clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &ev_start_time, &return_bytes);
    ret = clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_END,   sizeof(cl_ulong), &ev_end_time,  &return_bytes);
    logger->plog((long long)(ev_end_time - ev_start_time), s_name, "cl     ram_to_device");
  } else {
    cl_event dummy_event;
        if (datatype == 0) {
      ret = clEnqueueWriteImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data, 0, NULL, &dummy_event);
    } else if (datatype == 1) {
      ret = clEnqueueWriteImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data_i, 0, NULL, &dummy_event);
    }
    ret = clWaitForEvents(1, &dummy_event);
    clReleaseEvent(dummy_event);
    logger->log(1, ret, "%s\tRam to Device\n", s_name.c_str());
  }
  return;
}

void clbuffer::device2ram() {
  //  ret = clEnqueueReadBuffer(context->command_queue, buffer, CL_TRUE, 0, datasize, data, 0, NULL, NULL);

  size_t origin[] = {0, 0, 0}; // Defines the offset in pixels in the image from where to write.
  size_t region[] = {size_t(sx), size_t(sy), size_t(sz)}; // Size of object to be transferred

  if (context->profiling) {
    // logger->log(1, ret, "%s\tDevice to Ram\n", s_name.c_str());
    cl_ulong ev_start_time, ev_end_time;
    cl_event profiling_event;
    if (datatype == 0) {
    ret = clEnqueueReadImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data, 0, NULL, &profiling_event);
    } else if (datatype == 1) {
    ret = clEnqueueReadImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data_i, 0, NULL, &profiling_event);
    }
    ev_start_time = (cl_ulong)0;
    ev_end_time = (cl_ulong)0;
    size_t return_bytes;
    ret = clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &ev_start_time, &return_bytes);
    ret = clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_END,   sizeof(cl_ulong), &ev_end_time,  &return_bytes);
    logger->plog((long long)(ev_end_time - ev_start_time), s_name, "cl     device_to_ram");
  } else {
    cl_event dummy_event;
        if (datatype == 0) {
    ret = clEnqueueReadImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data, 0, NULL, &dummy_event);
    } else if (datatype == 1) {
    ret = clEnqueueReadImage(context->command_queue, buffer, CL_TRUE, origin, region, 0, 0, data_i, 0, NULL, &dummy_event);
    }
    // ret = clWaitForEvents(1, &dummy_event);
    clReleaseEvent(dummy_event);
  }
  return;
}

void clbuffer::debug() {
  device2ram();
  logger->open_debugstream(s_name, 0);
  for (int z = 0; z < sz; z++) {
    logger->Debug_stream << get(0, 0, z, 0) << "\t";
    logger->Debug_stream << get(0, 0, z, 1) << "\t";
    logger->Debug_stream << get(0, 0, z, 2) << "\t";
    logger->Debug_stream << get(0, 0, z, 3) << "\n";
  }
  logger->close_debugstream();
}

void clbuffer::write_raw(std::string s_filename) {
  device2ram();
  std::ofstream os_binary(s_filename, std::ios::binary);
  os_binary.write((char*) data, datasize);
  os_binary.close();
  os_binary.clear();
}

void clbuffer::read_raw(std::string s_filename) {
  std::ifstream is_binary(s_filename, std::ios::binary);
  is_binary.read((char*) data, datasize);
  is_binary.close();
  is_binary.clear();
}
