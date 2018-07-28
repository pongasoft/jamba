#include <pongasoft/Utils/Collection/CircularBuffer.h>
#include <gtest/gtest.h>

namespace pongasoft {
namespace Utils {
namespace Collection {
namespace Test {

// CircularBuffer - copyToBuffer
TEST(CircularBuffer, copyToBuffer)
{
  CircularBuffer<int> cb(5);
  cb.init(0);

  // underlying buffer will be [6,7,3,4,5]
  //                                ^ head
  for(int i = 1; i <= 7; i++)
    cb.push(i);

  // make sure the "head" is pointing at the right location
  ASSERT_EQ(cb.getAt(0), 3);

  int buf1[1];
  cb.copyToBuffer(0, buf1, 1);
  ASSERT_EQ(buf1[0], 3);

  int buf3[3];
  cb.copyToBuffer(0, buf3, 3);
  ASSERT_EQ(buf3[0], 3);
  ASSERT_EQ(buf3[1], 4);
  ASSERT_EQ(buf3[2], 5);

  cb.copyToBuffer(-1, buf3, 3);
  ASSERT_EQ(buf3[0], 7);
  ASSERT_EQ(buf3[1], 3);
  ASSERT_EQ(buf3[2], 4);

  cb.copyToBuffer(1, buf3, 3);
  ASSERT_EQ(buf3[0], 4);
  ASSERT_EQ(buf3[1], 5);
  ASSERT_EQ(buf3[2], 6);

  int buf7[7];
  cb.copyToBuffer(1, buf7, 7);
  ASSERT_EQ(buf7[0], 4);
  ASSERT_EQ(buf7[1], 5);
  ASSERT_EQ(buf7[2], 6);
  ASSERT_EQ(buf7[3], 7);
  ASSERT_EQ(buf7[4], 3);
  ASSERT_EQ(buf7[5], 4);
  ASSERT_EQ(buf7[6], 5);

}

// CircularBuffer - fold
TEST(CircularBuffer, fold)
{
  CircularBuffer<int> cb(5);
  cb.init(0);

  // underlying buffer will be [6,7,3,4,5]
  //                                ^ head
  for(int i = 1; i <= 7; i++)
    cb.push(i);

  auto sum = [](int x, int y) -> auto {return x+y;};

  ASSERT_EQ(0, cb.fold(0, 0, 0, sum));
  ASSERT_EQ(3, cb.fold(0, 1, 0, sum));
  ASSERT_EQ(7, cb.fold(0, 2, 0, sum));
  ASSERT_EQ(12, cb.fold(0, 3, 0, sum));
  ASSERT_EQ(18, cb.fold(0, 4, 0, sum));

  ASSERT_EQ(6, cb.fold(-2, -1, 0, sum));
  ASSERT_EQ(16, cb.fold(-2, 1, 0, sum));

  // going backward (first offset always included, second one not => different results)
  ASSERT_EQ(7, cb.fold(-1, -2, 0, sum));
  ASSERT_EQ(14, cb.fold(1, -2, 0, sum));

  // start offset assumed to be 0
  ASSERT_EQ(0, cb.fold(0, 0, sum));
  ASSERT_EQ(3, cb.fold(1, 0, sum));
  ASSERT_EQ(7, cb.fold(2, 0, sum));
  ASSERT_EQ(12, cb.fold(3, 0, sum));
  ASSERT_EQ(18, cb.fold(4, 0, sum));

  // full buffer
  ASSERT_EQ(25, cb.fold(0, sum));

  // more than 1 buffer [6,7,3,4,5][6,7,3,4,5][6,7,3,4,5]
  //                           ^1                ^9
  ASSERT_EQ(40, cb.fold(1, 9, 0, sum));

  std::vector<int> indices{};
  auto sum2 = [&indices](int index, int x, int y) -> int {
    indices.push_back(index);
    return x+y;
  };

  ASSERT_EQ(3, cb.foldWithIndex(0, 1, 0, sum2));
  ASSERT_EQ(1, indices.size());
  ASSERT_EQ(0, indices[0]);
  indices.clear();

  ASSERT_EQ(18, cb.foldWithIndex(0, 4, 0, sum2));
  ASSERT_EQ(4, indices.size());
  ASSERT_EQ(0, indices[0]);
  ASSERT_EQ(1, indices[1]);
  ASSERT_EQ(2, indices[2]);
  ASSERT_EQ(3, indices[3]);
  indices.clear();

  // going backward (first offset always included, second one not => different results)
  ASSERT_EQ(14, cb.foldWithIndex(1, -2, 0, sum2));
  ASSERT_EQ(3, indices.size());
  ASSERT_EQ(1, indices[0]);
  ASSERT_EQ(0, indices[1]);
  ASSERT_EQ(-1, indices[2]);

}

}
}
}
}

