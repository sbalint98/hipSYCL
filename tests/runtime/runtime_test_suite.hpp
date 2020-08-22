/*
 * This file is part of hipSYCL, a SYCL implementation based on CUDA/HIP
 *
 * Copyright (c) 2020 Aksel Alpay and contributors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <hipSYCL/runtime/serialization/serialization.hpp>
#include <hipSYCL/runtime/application.hpp>
#include <hipSYCL/runtime/util.hpp>
#include <hipSYCL/runtime/data.hpp>

#include <boost/test/unit_test.hpp>

#ifndef HIPSYCL_RT_TESTS_HPP
#define HIPSYCL_RT_TESTS_HPP

using namespace hipsycl;

struct mark_current_configuration{
  std::vector<rt::device_id> device_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > make_current_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > expected_outdated;

  friend std::ostream &operator<<(std::ostream &out, const mark_current_configuration &v);
};

std::ostream &operator<<(std::ostream &out, const mark_current_configuration &v);

struct mark_valid_configuration {
  std::vector<rt::device_id> device_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > make_current_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > make_valid_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > expected;
  friend std::ostream &operator<<(std::ostream &out, const mark_valid_configuration &v);
};

std::ostream &operator<<(std::ostream &out, const mark_valid_configuration &v);

struct update_source{
  rt::device_id target_device;
  std::vector<std::pair<rt::device_id, rt::range_store::rect> >  source_region_per_device;
};

struct update_source_candidates_configuration{
  std::vector<rt::device_id> device_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > make_current_list;
  std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > make_valid_list;
  std::vector<update_source> expected;
  friend std::ostream &operator<<(std::ostream &out, const mark_valid_configuration &v);
};

std::ostream &operator<<(std::ostream &out, const update_source_candidates_configuration &v);

struct shutdown_runtime {
  ~shutdown_runtime() {
   hipsycl::rt::application::reset();
  }
};

#endif
