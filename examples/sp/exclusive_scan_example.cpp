// SPDX-FileCopyrightText: Intel Corporation
//
// SPDX-License-Identifier: BSD-3-Clause

#include <dr/sp.hpp>
#include <sycl/sycl.hpp>

#include <iostream>

#include <fmt/core.h>
#include <fmt/ranges.h>

int main(int argc, char **argv) {
  printf("Creating NUMA devices...\n");
  auto devices = dr::sp::get_numa_devices(sycl::default_selector_v);
  dr::sp::init(devices);

  for (auto &device : devices) {
    std::cout << "  Device: " << device.get_info<sycl::info::device::name>()
              << "\n";
  }

  dr::sp::distributed_vector<int, dr::sp::device_allocator<int>> v(100);

  std::vector<int> lv(100);

  std::iota(lv.begin(), lv.end(), 0);
  dr::sp::copy(lv.begin(), lv.end(), v.begin());

  fmt::print(" v: {}\n", v);
  fmt::print("lv: {}\n", lv);

  std::exclusive_scan(lv.begin(), lv.end(), lv.begin(), 0);
  dr::sp::exclusive_scan(dr::sp::par_unseq, v, v, 0);

  fmt::print(" (after)  v: {}\n", v);
  fmt::print(" (after) lv: {}\n", lv);

  for (std::size_t i = 0; i < lv.size(); i++) {
    int x = lv[i];
    int y = v[i];
    if (x != y) {
      printf("(%lu) %d != %d\n", i, x, y);
    }
  }

  std::iota(lv.begin(), lv.end(), 0);
  dr::sp::copy(lv.begin(), lv.end(), v.begin());

  dr::sp::distributed_vector<int, dr::sp::device_allocator<int>> o(v.size() +
                                                                   100);

  std::exclusive_scan(lv.begin(), lv.end(), lv.begin(), 12, std::plus<>());
  dr::sp::exclusive_scan(dr::sp::par_unseq, v, o, 12, std::plus<>());

  fmt::print(" (after)  v: {}\n",
             rng::subrange(o.begin(), o.begin() + v.size()));
  fmt::print(" (after) lv: {}\n", lv);

  for (std::size_t i = 0; i < lv.size(); i++) {
    int x = lv[i];
    int y = o[i];
    if (x != y) {
      printf("(%lu) %d != %d\n", i, x, y);
    }
  }

  return 0;
}
