#include "hipSYCL/sycl/access.hpp"

#include <iostream>
#include <map>
using namespace hipsycl::sycl::access;

// Maybe we some template magic we can use one function?
// SIFNE so we can put these in one function in theory right?
// but now this will get called all the time since there probably
// wont be an errror during template resulotion right?
// There aren't any template overloads that I know of right that would
// cause this problem but there might will be in the future

// Maybe we should check if T value is in range, or should we just list the
// types that we are affecting
template<typename T>
std::ostream& operator<<(std::ostream& out, const T value){
    static std::map<T, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(mode::read);
        INSERT_ELEMENT(mode::write);
        INSERT_ELEMENT(mode::atomic);
        INSERT_ELEMENT(mode::read_write);
        INSERT_ELEMENT(mode::discard_write);
        INSERT_ELEMENT(mode::discard_read_write);
        INSERT_ELEMENT(target::image);
        INSERT_ELEMENT(target::image);
        INSERT_ELEMENT(target::host_image);
        INSERT_ELEMENT(target::host_buffer);
        INSERT_ELEMENT(target::image_array);
        INSERT_ELEMENT(target::global_buffer);
        INSERT_ELEMENT(target::constant_buffer);
        INSERT_ELEMENT(placeholder::false_t);
        INSERT_ELEMENT(placeholder::true_t);
        INSERT_ELEMENT(fence_space::local_space);
        INSERT_ELEMENT(fence_space::global_space);
        INSERT_ELEMENT(fence_space::global_and_local);
#undef INSERT_ELEMENT
    }
    auto search = strings.find(value);
    if (strings.end() != search){
        return out << strings[value];
    } else {
        throw;
    }
}

/*
std::ostream& operator<<(std::ostream& out, const mode value){
    static std::map<mode, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(mode::read);
        INSERT_ELEMENT(mode::write);
        INSERT_ELEMENT(mode::atomic);
        INSERT_ELEMENT(mode::read_write);
        INSERT_ELEMENT(mode::discard_write);
        INSERT_ELEMENT(mode::discard_read_write);
#undef INSERT_ELEMENT
    }
    return out << strings[value];
}

std::ostream& operator<<(std::ostream& out, const target value){
    static std::map<target, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(target::image);
        INSERT_ELEMENT(target::image);
        INSERT_ELEMENT(target::host_image);
        INSERT_ELEMENT(target::host_buffer);
        INSERT_ELEMENT(target::image_array);
        INSERT_ELEMENT(target::global_buffer);
        INSERT_ELEMENT(target::constant_buffer);
#undef INSERT_ELEMENT
    }
    return out << strings[value];
}

std::ostream& operator<<(std::ostream& out, const placeholder value){
    static std::map<placeholder, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(placeholder::false_t);
        INSERT_ELEMENT(placeholder::true_t);
#undef INSERT_ELEMENT
    }
    return out << strings[value];
}

std::ostream& operator<<(std::ostream& out, const fence_space value){
    static std::map<fence_space, std::string> strings;
    if (strings.size() == 0){
#define INSERT_ELEMENT(p) strings[p] = #p
        INSERT_ELEMENT(fence_space::local_space);
        INSERT_ELEMENT(fence_space::global_space);
        INSERT_ELEMENT(fence_space::global_and_local);
#undef INSERT_ELEMENT
    }
    return out << strings[value];
}
*/
