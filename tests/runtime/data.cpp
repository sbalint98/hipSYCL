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
  
  // Should make the list of devices part of this 
  
  struct input_configuration {
    std::size_t page_size;
    std::size_t element_size;
    rt::range<3> num_elements;
    rt::backend_descriptor backend_descriptor{rt::hardware_platform::cpu,
                             rt::api_platform::openmp_cpu};
  };

  std::vector<input_configuration> input_configurations{
    {4, 1, rt::range<3>{16,16,16}}

  };


  //std::size_t page_size = 4;
  //std::size_t element_size = 1; 
  //rt::range<3> num_elements= rt::range<3>{16,16,16};
  //rt::backend_descriptor a;
  //a.hw_platform = rt::hardware_platform::cpu;
  //a.sw_platform = rt::api_platform::openmp_cpu;
  //a.id = rt::backend_id::openmp_cpu;

  for (auto a : input_configurations){
    rt::device_id device_0(a.backend_descriptor, 1);
    rt::data_region test_region_1(a.num_elements, a.element_size, a.page_size);

    //Testing If allocations are actually allocated 
    BOOST_CHECK(!test_region_1.has_allocation(device_0));
    //I am also not sure about this malloc part but I hope that for testing it is fine
    test_region_1.add_empty_allocation(device_0, malloc(23));
    BOOST_CHECK(test_region_1.has_allocation(device_0));
    test_region_1.remove_allocation(device_0);
    BOOST_CHECK(!test_region_1.has_allocation(device_0));
    test_region_1.add_nonempty_allocation(device_0, malloc(23));
    BOOST_CHECK(test_region_1.has_allocation(device_0));
    test_region_1.remove_allocation(device_0);
    BOOST_CHECK(!test_region_1.has_allocation(device_0));
    // I am not sure with what should I instantiate this but I hope for testing it is fine
    std::function<void*(rt::range<3> num_elements, std::size_t element_size)> mem_alloc;
    test_region_1.add_placeholder_allocation(device_0, mem_alloc);
    BOOST_CHECK(test_region_1.has_allocation(device_0));
    test_region_1.remove_allocation(device_0);
    BOOST_CHECK(!test_region_1.has_allocation(device_0));
    // At this point we have one device and no allocations  

    rt::device_id device_1 (a.backend_descriptor, 2);
    rt::device_id device_2 (a.backend_descriptor, 3);
    rt::device_id device_3 (a.backend_descriptor, 4);
    rt::device_id device_4 (a.backend_descriptor, 5);
    


    test_region_1.add_nonempty_allocation(device_0, malloc(23));
    test_region_1.add_nonempty_allocation(device_1, malloc(23));
    test_region_1.add_nonempty_allocation(device_2, malloc(23));
    test_region_1.add_nonempty_allocation(device_3, malloc(23));
    //Why don't I have to pass a allocation shape for add_placeholder and add_nonempty?
    test_region_1.mark_range_current(device_0, rt::id<3>{0,0,0}, rt::range<3>{2,2,2});
    BOOST_CHECK(test_region_1.has_initialized_content(rt::id<3>{0,0,0}, rt::range<3>{4,4,4}));
    
    std::vector<rt::range_store::rect> out;
    test_region_1.get_outdated_regions(device_1, rt::id<3>{0,0,0}, rt::range<3>{16,16,16},  out);
    BOOST_CHECK(out.size()==1);
    // Here I need some sor of iterator over the outdated regions 
    BOOST_CHECK((out[0].first==rt::id<3>{0,0,0}));
    BOOST_CHECK((out[0].second==rt::range<3>{4,4,4}));

    //Mark Entire range valid on other device
    test_region_1.mark_range_valid(device_2, rt::id<3>{0,0,0}, rt::range<3>{2,2,2});
    std::vector<rt::range_store::rect> out_2;
    test_region_1.get_outdated_regions(device_2, rt::id<3>{0,0,0}, rt::range<3>{16,16,16},  out_2);
    BOOST_CHECK(out_2.size()==0);

    //Mark part of the range valid on other device
    test_region_1.mark_range_valid(device_3, rt::id<3>{0,0,0}, rt::range<3>{1,1,1});
    std::vector<rt::range_store::rect> out_3;
    test_region_1.get_outdated_regions(device_3, rt::id<3>{0,0,0}, rt::range<3>{16,16,16},  out_3);
    BOOST_CHECK(out_3.size()==1);
    std::cout << out_3.size() << std::endl;
    // Here I need some sor of iterator over the outdated regions
    // On device_3 the region rt::id<3>{1,1,1} rt::range{1,1,1} should still be invalid
    // Therfore the entire page should be invalid region 
    BOOST_CHECK((out_3[0].first==rt::id<3>{0,0,0}));
    BOOST_CHECK((out_3[0].second==rt::range<3>{4,4,4}));

    //Make this universal for every page size
    BOOST_CHECK(test_region_1.get_page_range(rt::id<3>{0,0,0}, rt::range<3>{16,16,16}) 
                == std::make_pair (rt::id<3>{0,0,0}, rt::range<3>(16/a.page_size, 16/a.page_size, 16/a.page_size)) );
  }
}
BOOST_AUTO_TEST_SUITE_END()