
#include "runtime_test_suite.hpp"
#include <iostream>

using namespace hipsycl;

void print_vector(std::ostream &out, std::vector<std::pair<rt::device_id, std::vector<rt::range_store::rect> > > list){
  for (auto [device, allocs]: list){
    std::string out_str = "Device: " + std::to_string(device.get_id()) + " ";
    out  << out_str;
    for (int i=0; i<allocs.size(); i++){
      std::string padding = std::string(out_str.length(), ' ');
      if (i>0){out << padding;}
      out << allocs[i].first << " " << allocs[i].second << std::endl;
    }
  }
}
std::ostream &operator<<(std::ostream &out, const mark_current_configuration &v){
  out << std::endl;
  out << "Marked current: " << std::endl;
  print_vector(out, v.make_current_list);
  out << "Expected outdated: " << std::endl;
  print_vector(out, v.expected_outdated);
  return out;
}

std::ostream &operator<<(std::ostream &out, const mark_valid_configuration &v){
  out << std::endl;
  out << "Marked current: " << std::endl;
  print_vector(out, v.make_current_list);
  out << "Make valid: " << std::endl;
  print_vector(out, v.make_valid_list);
  out << "Expected outdated: " << std::endl;
  print_vector(out, v.expected);
  return out;
}

std::ostream &operator<<(std::ostream &out, const update_source_candidates_configuration &v){
  out << std::endl;
  out << "Marked current: " << std::endl;
  print_vector(out, v.make_current_list);
  out << "Make valid: " << std::endl;
  print_vector(out, v.make_valid_list);
  out << "Expected candidate sources: " << std::endl;
  for (auto const& [device, copies]: v.expected){
    out << "  For device_" << device.get_id() << " source candidates:" << std::endl;
    for (auto const& [source_device, copy_region] : copies){
      out << "    From device_" << source_device.get_id()
          <<" "<< copy_region.first << copy_region.second << std::endl;
    }
  }

  return out;
}