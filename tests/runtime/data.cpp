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

#include "runtime_test_suite.hpp"

#include <boost/test/tools/old/interface.hpp>
#include <vector>
#include <memory>
#include <hipSYCL/runtime/data.hpp>
#include <hipSYCL/runtime/util.hpp>
#include <hipSYCL/runtime/serialization/serialization.hpp>

using namespace hipsycl;

BOOST_FIXTURE_TEST_SUITE(data, shutdown_runtime)
BOOST_AUTO_TEST_CASE(page_table) {

  rt::range_store::rect full_range{rt::id<3>{0, 0, 0},
                                   rt::range<3>{16, 16, 16}};
  rt::range_store::rect fill_subrange{rt::id<3>{2, 3, 4},
                                      rt::range<3>{4, 4, 4}};
  rt::range_store::rect intersection_subrange(rt::id<3>{2, 2, 2},
                                              rt::range<3>{4, 4, 4});
  rt::range_store pt(full_range.second);
  BOOST_CHECK(
  pt.entire_range_equals(full_range, rt::range_store::data_state::empty));
  BOOST_CHECK(pt.entire_range_empty(full_range));
  BOOST_CHECK(!pt.entire_range_filled(full_range));
  BOOST_CHECK(pt.get_size() == full_range.second);

  pt.add(fill_subrange);
  BOOST_CHECK(!pt.entire_range_filled(full_range));
  BOOST_CHECK(!pt.entire_range_empty(full_range));
  BOOST_CHECK(pt.entire_range_filled(fill_subrange));

  struct input_configuration {
    rt::range_store::rect page_table_size;
    std::vector<rt::range_store::rect> filled_subrange;
    rt::range_store::rect intersection_subrange;
    std::vector<rt::range_store::rect> expected_intersections;
  };

  std::vector<input_configuration> configurations {
    {
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
      {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
      rt::range_store::rect{rt::id<3>{2, 2, 2}, rt::range<3>{4, 4, 4}},
      {
        rt::range_store::rect{rt::id<3>{2,3,4}, rt::range<3>{4,3,2}}
      }
    },
    {
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
      {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{3, 4, 5}},
      {
        rt::range_store::rect{rt::id<3>{2,3,4}, rt::range<3>{1,1,1}}
      }
    },
    {
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
      {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 4, 5}},
      {}
    },
    // I think this a bug but I am not sure...
    // I belive here we have the situation that in a 2d case
    // there are 2 allocations with the with of 1 parallel with the x axis
    // and there is a perpendicular allocation to those 
    {
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 1, 1}}, rt::range_store::rect{rt::id<3>{0, 1, 0}, rt::range<3>{16, 1, 1}}},
      rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 1, 1}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{1, 1, 1}},rt::range_store::rect{rt::id<3>{0, 1, 0}, rt::range<3>{1, 1, 1}}}
    }
  };

  for (auto config : configurations) {

    auto full_range = config.page_table_size;
    auto fill_subranges = config.filled_subrange;
    auto intersection_subrange = config.intersection_subrange;

    rt::range_store pt(full_range.second);
    for (auto fill_subrange: fill_subranges){
      pt.add(fill_subrange);
    }
    BOOST_CHECK(!pt.entire_range_empty(full_range));
    std::vector<rt::range_store::rect> intersections;
    pt.intersections_with(intersection_subrange, intersections);
    // We require that the set of returned subranges is the same, but not
    // necessarily the order in which the subranges are returned
    BOOST_CHECK(intersections.size() == config.expected_intersections.size());
    // How could we print this out but as debug output? eg only if a test fails?
    std::cout << "Got: "<<intersections.size() << " Expected: " << config.expected_intersections.size() << std::endl;
    for (auto sub_range : intersections) {
      std::cout << "offset: " << sub_range.first << " range: " << sub_range.second << std::endl;
      auto it = std::find(config.expected_intersections.begin(),
                          config.expected_intersections.end(), sub_range);
      BOOST_CHECK(it != config.expected_intersections.end());
    }
  }
}
// Mark: Valid, current,             - range
// Add: placeholder, empty, nonempty - allocation
// get: outdated,                    - regions
//      update_source                - candidates
//      users 
//      element_size, num_elements
//      memory
// has_initialized_content
// create_fork
// 

// Test basic setup, eg num_elements not multiple of pt size 


BOOST_AUTO_TEST_CASE(data_region) {
  rt::range<3> num_elements= rt::range<3>{16,16,16};
  rt::data_region(num_elements, 1, 4);


}
BOOST_AUTO_TEST_SUITE_END()