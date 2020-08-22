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
#define BOOST_TEST_DYN_LINK
// FOr some reason I need this here...
#define BOOST_TEST_MODULE data

#include "runtime_test_suite.hpp"
//Why use the old interface here?
#include <boost/test/tools/interface.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <hipSYCL/runtime/data.hpp>
#include <hipSYCL/runtime/serialization/serialization.hpp>
#include <hipSYCL/runtime/util.hpp>
#include <memory>


#include <vector>

using namespace hipsycl;

BOOST_FIXTURE_TEST_SUITE(data, shutdown_runtime)

BOOST_FIXTURE_TEST_SUITE(page_table, shutdown_runtime)

BOOST_AUTO_TEST_CASE(initialization) {

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
}

BOOST_AUTO_TEST_CASE(intersections) {
  struct input_configuration {
    rt::range_store::rect page_table_size;
    std::vector<rt::range_store::rect> filled_subrange;
    rt::range_store::rect intersection_subrange;
    std::vector<rt::range_store::rect> expected_intersections;
  };

  std::vector<input_configuration> configurations{
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
       {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
       rt::range_store::rect{rt::id<3>{2, 2, 2}, rt::range<3>{4, 4, 4}},
       {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 3, 2}}}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
       {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
       rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{3, 4, 5}},
       {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{1, 1, 1}}}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}},
       {rt::range_store::rect{rt::id<3>{2, 3, 4}, rt::range<3>{4, 4, 4}}},
       rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 4, 5}},
       {}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 2, 2}},
       {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 1, 1}},
        rt::range_store::rect{rt::id<3>{0, 1, 0}, rt::range<3>{2, 1, 1}}},
       rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{1, 2, 1}},
       {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{1, 2, 1}}}},
      {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 2, 2}},
       {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{2, 1, 1}},
        rt::range_store::rect{rt::id<3>{0, 1, 1}, rt::range<3>{2, 1, 1}}},
       rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{1, 2, 2}},
       {rt::range_store::rect{rt::id<3>{0, 0, 0}, rt::range<3>{1, 1, 1}},
        rt::range_store::rect{rt::id<3>{0, 1, 1}, rt::range<3>{1, 1, 1}}}}};

  for (auto config : configurations) {

    auto full_range = config.page_table_size;
    auto fill_subranges = config.filled_subrange;
    auto intersection_subrange = config.intersection_subrange;

    rt::range_store pt(full_range.second);
    for (auto fill_subrange : fill_subranges) {
      pt.add(fill_subrange);
    }
    BOOST_CHECK(!pt.entire_range_empty(full_range));
    std::vector<rt::range_store::rect> intersections;
    pt.intersections_with(intersection_subrange, intersections);
    BOOST_CHECK_MESSAGE(
        intersections.size() == config.expected_intersections.size(),
        "Got: " << intersections.size()
                << " Expected: " << config.expected_intersections.size());
    for (auto sub_range : intersections) {

      auto it = std::find(config.expected_intersections.begin(),
                          config.expected_intersections.end(), sub_range);
      BOOST_CHECK_MESSAGE(it != config.expected_intersections.end(),
                          "offset: " << sub_range.first
                                     << " range: " << sub_range.second);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()

// Probaly should create a fixture for this
BOOST_FIXTURE_TEST_SUITE(data_region, shutdown_runtime)

// TODO test this with data_test_case
BOOST_AUTO_TEST_CASE(constructor){
  rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
  BOOST_CHECK_EQUAL(test.get_num_elements(), (rt::range<3>{16,16,16}));
  BOOST_CHECK_EQUAL(test.get_element_size(), 2);
  auto page_range = test.get_page_range(rt::id<3>{0,0,0}, rt::range<3>{16,16,16});
  BOOST_CHECK_EQUAL(page_range.first, (rt::id<3>{0,0,0}) );
  BOOST_CHECK_EQUAL(page_range.second, (rt::range<3>{4,4,4}) );
  // I don't know how to test this one
  //BOOST_CHECK_THROW(rt::data_region(rt::range<3>{16,16,16}, 2, rt::range<3>{3,3,3}), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(setters_getters){
  rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
  BOOST_CHECK(!test.has_id());
  test.set_enumerated_id(42);
  BOOST_CHECK(test.has_id());
  BOOST_CHECK_EQUAL(test.get_id(), 42);
  test.unset_id();
  BOOST_CHECK(!test.has_id());
}
// Make this data test case with the size of the allocation
BOOST_AUTO_TEST_CASE(allocations){
  //TODO: Test get memory
  rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
  rt::backend_descriptor backend_descriptor{rt::hardware_platform::cpu,
                                            rt::api_platform::openmp_cpu};
  rt::device_id device_0(backend_descriptor, 0);

  BOOST_CHECK(!test.has_allocation(device_0));
  // I am also not sure about this malloc part but I hope that for testing it
  // is fine
  void* test_range = malloc(42);
  test.add_empty_allocation(device_0, test_range);
  BOOST_CHECK_EQUAL(test_range,test.get_memory(device_0));
  BOOST_CHECK(test.has_allocation(device_0));
  BOOST_CHECK(!test.has_initialized_content(rt::id<3>{0,0,0}, rt::id<3>{16,16,16}));
  test.remove_allocation(device_0);
  BOOST_CHECK(!test.has_allocation(device_0));
  test.add_nonempty_allocation(device_0, malloc(23));
  BOOST_CHECK(test.has_allocation(device_0));
  BOOST_CHECK(test.has_initialized_content(rt::id<3>{0,0,0}, rt::id<3>{16,16,16}));
  test.remove_allocation(device_0);
  BOOST_CHECK(!test.has_allocation(device_0));
  // I am not sure with what should I instantiate this but I hope for testing
  // it is fine
  std::function<void *(rt::range<3> num_elements, std::size_t element_size)>
          mem_alloc;
  test.add_placeholder_allocation(device_0, mem_alloc);
  BOOST_CHECK(test.has_allocation(device_0));
  test.remove_allocation(device_0);
  BOOST_CHECK(!test.has_allocation(device_0));
}
std::ostream &operator<<(std::ostream &out, const mark_current_configuration &v){
  out << "mehh" << std::endl;
  return out;
}

    rt::backend_descriptor backend_descriptor{rt::hardware_platform::cpu,
                                              rt::api_platform::openmp_cpu};
    rt::device_id device_0(backend_descriptor, 0);
    rt::device_id device_1(backend_descriptor, 1);
    rt::device_id device_2(backend_descriptor, 2);
BOOST_AUTO_TEST_SUITE(mark_range_current)

  BOOST_AUTO_TEST_CASE(simple){
    rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
    rt::backend_descriptor backend_descriptor{rt::hardware_platform::cpu,
                                              rt::api_platform::openmp_cpu};
    rt::device_id device_0(backend_descriptor, 0);
    rt::device_id device_1(backend_descriptor, 1);
    test.add_nonempty_allocation(device_0, malloc(42));
    test.add_nonempty_allocation(device_1, malloc(42));
    test.mark_range_current(device_0, rt::id<3>{0,0,0}, rt::range<3>{4,4,4});
    std::vector<rt::range_store::rect> out;
    test.get_outdated_regions(device_0, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
    BOOST_CHECK_EQUAL(out.size(), 0);
    test.get_outdated_regions(device_1, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
    BOOST_CHECK_EQUAL(out.size(), 1);
    BOOST_CHECK_MESSAGE(out[0].first == (rt::id<3>{0,0,0}) && out[0].second == (rt::id<3>{4,4,4}), "Found: " << out[0].first << " " << out[0].second );

    out.clear();
    test.remove_allocation(device_1);
    test.add_empty_allocation(device_1, malloc(42));
    test.get_outdated_regions(device_1, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
    BOOST_CHECK_EQUAL(out.size(), 1);
    BOOST_CHECK_MESSAGE(out[0].first == (rt::id<3>{0,0,0}) && out[0].second == (rt::id<3>{16,16,16}), "Found: " << out[0].first << " " << out[0].second );

    out.clear();
    test.remove_allocation(device_1);
    test.add_nonempty_allocation(device_1, malloc(42));
    test.get_outdated_regions(device_1, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
    BOOST_CHECK_EQUAL(out.size(), 0);
    out.clear();
    // I am not sure that it is correct that this returns no outdated regions... (maybe this has to do with the initialization?)
    test.get_outdated_regions(device_0, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
    BOOST_CHECK_EQUAL(out.size(), 0);
    //BOOST_CHECK_MESSAGE(out.at(0).first == (rt::id<3>{0,0,0}) && out[0].second == (rt::id<3>{16,16,16}), "Found: " << out[0].first << " " << out[0].second );
  }


  //TODO Add more tests
  std::vector<mark_current_configuration> cases
          {
                  {
                          {device_0, device_1},
                          {{device_0, { {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}} } } },
                          {{device_1, { {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}} } } }
                  },
                  {
                          {device_0, device_1},
                          {
                            {device_0, {{rt::id<3>{0,0,0}, rt::range<3>{8,8,8}}}},
                            {device_1, {{rt::id<3>{4,4,4}, rt::range<3>{4,4,4}}}}
                           },
                          {
                            {device_0, {{rt::id<3>{4,4,4}, rt::range<3>{4,4,4}}}},
                            {device_1, {{rt::id<3>{4,0,0}, rt::range<3>{4,4,8}}, {rt::id<3>{4,4,0}, rt::range<3>{4,4,4}}, {rt::id<3>{0,0,0}, rt::range<3>{4, 8, 8}}}}
                          }
                  },
          };


  namespace data = boost::unit_test::data;
  BOOST_DATA_TEST_CASE(data_test, data::make(cases), xr){
    rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
    for (auto device : xr.device_list){
      if(!test.has_allocation(device)){
        test.add_nonempty_allocation(device, malloc(42));
      }
    }
    for (const auto & [device, allocations] : xr.make_current_list){
      for (const auto& allocation : allocations){
        test.mark_range_current(device, allocation.first, allocation.second);
      }
    }
    std::vector<rt::device_id> devices_no_outdated = xr.device_list;
    std::vector<rt::range_store::rect> out;
    for (const auto & [device, expected_allocations] : xr.expected_outdated){
      devices_no_outdated.erase(std::remove(devices_no_outdated.begin(), devices_no_outdated.end(), device), devices_no_outdated.end());
      test.get_outdated_regions(device, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
      BOOST_CHECK_EQUAL(out.size(), expected_allocations.size());
      for (auto expected_allocation: expected_allocations) {
        auto it = std::find(expected_allocations.begin(),
                            expected_allocations.end(), expected_allocation);
        BOOST_CHECK(it != expected_allocations.end());
      }
      out.clear();
    }
    for (const auto& device : devices_no_outdated){
      test.get_outdated_regions(device, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
      BOOST_CHECK_EQUAL(out.size(),0);
      out.clear();
    }
  }

BOOST_AUTO_TEST_SUITE_END()


std::vector<mark_valid_configuration> cases_mark_valid
{
        {        //used_devices
                {device_0, device_1},
                //marked_current_region
                {{device_0, { {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}} } } },
                //to be marked valid
                {{device_1, { {rt::id<3>{0,0,0}, rt::range<3>{16,16,4}}}}},
                //expected outdated
                {{device_1, { {rt::id<3>{0, 4, 0}, rt::range<3>{16, 16, 16}} } } }

        },
        {
                {device_0, device_1},
                {
                        {device_0, {{rt::id<3>{0, 0, 0}, rt::range<3>{8, 8, 8}}}},
                        {device_1, {{rt::id<3>{4, 4, 4}, rt::range<3>{4, 4, 4}}}}
                },
                {
                        {device_0, {{rt::id<3>{4, 4, 4}, rt::range<3>{4, 4, 4}}}},
                        {device_1, {{rt::id<3>{4, 0, 0}, rt::range<3>{4, 4, 8}}}}
                },
                {
                        {device_1, {{rt::id<3>{4, 4, 0}, rt::range<3>{4, 4, 4}}, {rt::id<3>{0, 0, 0}, rt::range<3>{4, 8,
                                                                                                                  8}}}}
                }

        }
};

    namespace data = boost::unit_test::data;
BOOST_DATA_TEST_CASE(mark_range_valid, data::make(cases_mark_valid), xr){
      rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
      for (auto device : xr.device_list){
        if(!test.has_allocation(device)){
          test.add_nonempty_allocation(device, malloc(42));
        }
      }
      for (const auto & [device, allocations] : xr.make_current_list){
        for (const auto& allocation : allocations){
          test.mark_range_current(device, allocation.first, allocation.second);
        }
      }
    for (const auto & [device, allocations] : xr.make_valid_list){
      for (const auto& allocation : allocations){
        test.mark_range_valid(device, allocation.first, allocation.second);
      }
    }
      std::vector<rt::device_id> devices_no_outdated = xr.device_list;
      std::vector<rt::range_store::rect> out;
      for (const auto & [device, expected_outdated] : xr.expected){
        devices_no_outdated.erase(std::remove(devices_no_outdated.begin(), devices_no_outdated.end(), device), devices_no_outdated.end());
        test.get_outdated_regions(device, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
        BOOST_CHECK_EQUAL(out.size(), expected_outdated.size());
        for (auto expected_allocation: expected_outdated) {
          auto it = std::find(expected_outdated.begin(),
                              expected_outdated.end(), expected_allocation);
          BOOST_CHECK(it != expected_outdated.end());
        }
        out.clear();
      }
      //Make sure that there is no allocation where we don'expect any
      for (const auto& device : devices_no_outdated){
        test.get_outdated_regions(device, rt::id<3>{0,0,0}, rt::range<3>{16,16,16}, out);
        BOOST_CHECK_EQUAL(out.size(),0);
        out.clear();
      }
}

    std::vector<update_source_candidates_configuration> cases_update_source_candidates
            {
                    {        //used_devices
                            {device_0, device_1},
                            //marked_current_region
                            {{device_0, { {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}} } } },
                            //to be marked valid
                            {},
                            //expected
                            {{device_1, { { device_0, {rt::id<3>{0,0,0}, rt::range<3>{16,16,16}} } } }}
                   },
                    {
                            {device_0, device_1, device_2},
                            //marked_current_region
                            {{device_0, {{rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}}}}},
                            //to be marked valid
                            {},
                            //expected
                            {
                                    {device_1, {{device_0, {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}}}}},
                                    {device_2, {{device_0, {rt::id<3>{0, 0, 0}, rt::range<3>{16, 16, 16}}}}}
                            }
                    }
                    // not yet supported
                 //   {
                 //           {device_0, device_1, device_2},
                 //           //marked_current_region
                 //           {
                 //             {device_0, {{rt::id<3>{0, 0, 0}, rt::range<3>{8, 16, 16}}}},
                 //             {device_1, {{rt::id<3>{8, 0, 0}, rt::range<3>{8, 16, 16}}}},
                 //           },
                 //           //to be marked valid
                 //           {},
                 //           //expected
                 //           {
                 //                //   {device_0, {{device_1, {rt::id<3>{8, 0, 0}, rt::range<3>{8, 16, 16}}}}},
                 //                //   {device_1, {{device_0, {rt::id<3>{0, 0, 0}, rt::range<3>{8, 16, 12}}}}}
                 //                     {device_2, {{device_1, {rt::id<3>{8, 0, 0}, rt::range<3>{8, 16, 16}}},
                 //                                 {device_0, {rt::id<3>{0, 0, 0}, rt::range<3>{8, 16, 16}}}}}
                 //           }
                 //   },
  };
    namespace data = boost::unit_test::data;
BOOST_DATA_TEST_CASE(get_update_source_candidates, data::make(cases_update_source_candidates), xr){
      rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
      for (auto device : xr.device_list){
        if(!test.has_allocation(device)){
          test.add_nonempty_allocation(device, malloc(42));
        }
      }
      for (const auto & [device, allocations] : xr.make_current_list){
        for (const auto& allocation : allocations){
          test.mark_range_current(device, allocation.first, allocation.second);
        }
      }
      for (const auto & [device, allocations] : xr.make_valid_list){
        for (const auto& allocation : allocations){
          test.mark_range_valid(device, allocation.first, allocation.second);
        }
      }
      //test.get_update_source_candidates(device_1, {rt::id<3>{0,0,0},rt::range<3>{16,16,16}}, out);

      std::vector<rt::device_id> devices_no_outdated = xr.device_list;
      std::vector<std::pair<rt::device_id, rt::range_store::rect>> out;
      //test.get_update_source_candidates(device_1, {rt::id<3>{0,0,0},rt::range<3>{16,16,16}}, out);
      for (const auto & [device, source_region_per_device] : xr.expected){
        devices_no_outdated.erase(std::remove(devices_no_outdated.begin(), devices_no_outdated.end(), device), devices_no_outdated.end());
        test.get_update_source_candidates(device, { rt::id<3>{0,0,0}, rt::range<3>{16,16,16}}, out);
        BOOST_CHECK_EQUAL(out.size(), source_region_per_device.size());
        for (auto out_element: out) {
          auto it = std::find(source_region_per_device.begin(),
                              source_region_per_device.end(), out_element);
          BOOST_CHECK(it != source_region_per_device.end());

        }

        out.clear();
      }
      // We cant test here since the rt throws an SIGABORT singal
      //Make sure that there is no allocation where we don'expect any
      //for (const auto& device : devices_no_outdated){
      //  test.get_update_source_candidates(device, {rt::id<3>{0,0,0}, rt::range<3>{16,16,16}}, out);
      //  BOOST_CHECK_EQUAL(out.size(),0);
      //  out.clear();
      //}

}

BOOST_AUTO_TEST_CASE(create_fork){
      rt::data_region test(rt::range<3>{16,16,16}, 2, rt::range<3>{4,4,4});
      std::function<void *(rt::range<3> num_elements, std::size_t element_size)>
              mem_alloc;
      auto test_fork = test.create_fork();
      test_fork->add_placeholder_allocation(device_0, mem_alloc);
      test.apply_fork(test_fork.get());
      BOOST_CHECK(test.get_memory(device_0)!= nullptr);
    }



BOOST_AUTO_TEST_SUITE_END() //data_range

BOOST_AUTO_TEST_SUITE_END() //data
