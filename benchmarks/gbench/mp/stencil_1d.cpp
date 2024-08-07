// SPDX-FileCopyrightText: Intel Corporation
//
// SPDX-License-Identifier: BSD-3-Clause

#include "../common/dr_bench.hpp"

#ifdef SYCL_LANGUAGE_VERSION
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/async>
#include <oneapi/dpl/execution>
#include <oneapi/dpl/numeric>
#endif

using T = double;

static T init_val = 1;

auto stencil1d_slide_op = [](auto win) { return win[0] + win[1] + win[2]; };

static void Stencil1D_Slide_Std(benchmark::State &state) {
  std::vector<T> a(default_vector_size, init_val);
  std::vector<T> b(default_vector_size, init_val);
  Stats stats(state, sizeof(T) * a.size(), sizeof(T) * b.size());

  // Input is a window
  auto in_curr = rng::views::sliding(a, 3);
  auto in_prev = rng::views::sliding(b, 3);

  // Output is an element
  auto out_curr = rng::subrange(b.begin() + 1, b.end() - 1);
  auto out_prev = rng::subrange(a.begin() + 1, a.end() - 1);

  for (auto _ : state) {
    for (std::size_t i = 0; i < stencil_steps; i++) {
      stats.rep();
      rng::transform(in_curr, out_curr.begin(), stencil1d_slide_op);
      std::swap(in_curr, in_prev);
      std::swap(out_curr, out_prev);
    }
  }
}

DR_BENCHMARK(Stencil1D_Slide_Std);

auto stencil1d_subrange_op = [](auto &center) {
  auto win = &center;
  return win[-1] + win[0] + win[1];
};

static void Stencil1D_Subrange_Std(benchmark::State &state) {
  std::vector<T> a(default_vector_size, init_val);
  std::vector<T> b(default_vector_size, init_val);
  Stats stats(state, sizeof(T) * a.size(), sizeof(T) * b.size());

  auto in = rng::subrange(a.begin() + 1, a.end() - 1);
  auto out = rng::subrange(b.begin() + 1, b.end() - 1);

  for (auto _ : state) {
    for (std::size_t i = 0; i < stencil_steps; i++) {
      stats.rep();
      rng::transform(in, out.begin(), stencil1d_subrange_op);
      std::swap(in, out);
    }
  }
}

DR_BENCHMARK(Stencil1D_Subrange_Std);

static void Stencil1D_Subrange_DR(benchmark::State &state) {
  auto dist = dr::mp::distribution().halo(1);
  xp::distributed_vector<T> a(default_vector_size, init_val, dist);
  xp::distributed_vector<T> b(default_vector_size, init_val, dist);
  Stats stats(state, sizeof(T) * a.size(), sizeof(T) * b.size());

  auto in = rng::subrange(a.begin() + 1, a.end() - 1);
  auto out = rng::subrange(b.begin() + 1, b.end() - 1);

  for (auto _ : state) {
    for (std::size_t i = 0; i < stencil_steps; i++) {
      stats.rep();
      xp::halo(in).exchange();
      xp::transform(in, out.begin(), stencil1d_subrange_op);
      std::swap(in, out);
    }
  }
}

DR_BENCHMARK(Stencil1D_Subrange_DR);

#ifdef SYCL_LANGUAGE_VERSION
static void Stencil1D_Subrange_DPL(benchmark::State &state) {
  auto q = get_queue();
  auto policy = oneapi::dpl::execution::make_device_policy(q);

  auto in = sycl::malloc_device<T>(default_vector_size, q);
  auto out = sycl::malloc_device<T>(default_vector_size, q);
  Stats stats(state, sizeof(T) * default_vector_size,
              sizeof(T) * default_vector_size);
  q.fill(in, init_val, default_vector_size);
  q.fill(out, init_val, default_vector_size);
  q.wait();

  for (auto _ : state) {
    for (std::size_t i = 0; i < stencil_steps; i++) {
      stats.rep();
      std::transform(policy, in + 1, in + default_vector_size - 1, out + 1,
                     stencil1d_subrange_op);
      std::swap(in, out);
    }
  }
  sycl::free(in, q);
  sycl::free(out, q);
}

DR_BENCHMARK(Stencil1D_Subrange_DPL);
#endif
