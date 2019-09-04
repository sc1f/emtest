#include <vector>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

namespace emtest {
    template <typename T>
    std::vector<T> make_vector() {
        return std::vector<T>();
    }

    template <typename K, typename V>
    std::map<K, V> make_map() {
        return std::map<K, V>();
    }

    double
    speedtest_js_vector_read(val arr) {
        auto start = std::chrono::system_clock::now();

        auto end = std::chrono::system_clock::now();

        return duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    double
    speedtest_cpp_vector() {
        std::vector<std::vector<std::string>> vectors;

        auto start = std::chrono::system_clock::now();

        for (std::uint32_t i = 0; i < 100; ++i) {
            std::vector<std::string> v;
            for (std::uint32_t j = 0; j < 100; j++) {
                v.push_back("new_string");
            }
            vectors.push_back(v);
        }

        auto end = std::chrono::system_clock::now();

        return duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    double
    speedtest_js_array() {
       std::vector<val> arrays;

        auto start = std::chrono::system_clock::now();

        for (std::uint32_t i = 0; i < 100; ++i) {
            val array = val::array();
            for (std::uint32_t j = 0; j < 100; j++) {
                array.call<void>("push", val("new_string"));
            }
            arrays.push_back(array);
        }

        auto end = std::chrono::system_clock::now();

        return duration_cast<std::chrono::milliseconds>(end - start).count();
    }
}

int
main(int argc, char** argv) {
    // clang-format off
    EM_ASM({
        console.log('runs main');
    });
}

using namespace emtest;

// emscripten bindings
EMSCRIPTEN_BINDINGS(emtest) {
    // vectors
    register_vector<std::int32_t>("std::vector<std::int32_t>");
    register_vector<std::string>("std::vector<std::string>");

    // maps
    register_map<std::string, std::string>("std::map<std::string, std::string>");

    // create vectors/maps from JS
    function("make_string_vector", &make_vector<std::string>);
    function("make_2d_string_vector", &make_vector<std::vector<std::string>>);
    function("make_string_map", &make_map<std::string, std::string>);

    function("speedtest_cpp_vector", &speedtest_cpp_vector);
    function("speedtest_js_array", &speedtest_js_array);
    function("speedtest_js_vector_read", &speedtest_js_vector_read);
}