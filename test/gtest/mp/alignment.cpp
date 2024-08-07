// SPDX-FileCopyrightText: Intel Corporation
//
// SPDX-License-Identifier: BSD-3-Clause

#include "xp-tests.hpp"

using DV = dr::mp::distributed_vector<int>;

TEST(Alignment, One) {
  Ops1<DV> ops(10);
  EXPECT_TRUE(dr::mp::aligned(ops.dist_vec));
}

TEST(Alignment, Two) {
  Ops2<DV> ops(10);
  EXPECT_TRUE(dr::mp::aligned(ops.dist_vec0, ops.dist_vec1));
}

TEST(Alignment, Three) {
  Ops3<DV> ops(10);
  EXPECT_TRUE(dr::mp::aligned(ops.dist_vec0, ops.dist_vec1, ops.dist_vec2));
}

TEST(Alignment, OffsetBy1) {
  Ops2<DV> ops(10);
  bool is_aligned =
      dr::mp::aligned(rng::views::drop(ops.dist_vec0, 1), ops.dist_vec1);
  if (comm_size == 1) {
    // If there is a single segment, then it is aligned
    EXPECT_TRUE(is_aligned);
  } else {
    EXPECT_FALSE(is_aligned);
  }
}

TEST(Alignment, Subrange) {
  Ops2<DV> ops(10);
  auto is_aligned = dr::mp::aligned(
      rng::subrange(ops.dist_vec0.begin() + 1, ops.dist_vec0.end() - 1),
      rng::views::drop(ops.dist_vec1, 2));
  if (comm_size == 1) {
    // If there is a single segment, then it is aligned
    EXPECT_TRUE(is_aligned);
  } else {
    EXPECT_FALSE(is_aligned);
  }
}

TEST(Alignment, Iota) { EXPECT_TRUE(dr::mp::aligned(rng::views::iota(100))); }

TEST(Alignment, Iota2) {
  Ops1<DV> ops(10);
  EXPECT_TRUE(dr::mp::aligned(ops.dist_vec, rng::views::iota(100)));
}

TEST(Alignment, ZipAligned) {
  Ops2<DV> ops(10);
  EXPECT_TRUE(
      dr::mp::aligned(dr::mp::views::zip(ops.dist_vec0, ops.dist_vec1)));
}

TEST(Alignment, ZipMisaligned) {
  Ops2<DV> ops(10);
  auto is_aligned = dr::mp::aligned(
      dr::mp::views::zip(dr::mp::views::drop(ops.dist_vec0, 1), ops.dist_vec1));
  if (comm_size == 1) {
    // If there is a single segment, then it is aligned
    EXPECT_TRUE(is_aligned);
  } else {
    EXPECT_FALSE(is_aligned);
  }
}
