
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "aruna/comm/Link.h"

using ::testing::Return;

class MockLink : public aruna::comm::Link {
public:
    MOCK_METHOD(size_t, _write, (uint8_t * data, size_t data_size), (override));

    MOCK_METHOD(size_t, _read, (uint8_t * buffer, size_t buffer_size), (override));

    MOCK_METHOD(uint32_t, get_speed, (), (override));
};

TEST(Link, sending) {
  MockLink link;
  uint8_t data = 0x04;
  EXPECT_CALL(link, _write(&data, 1))
    .Times(1)
    .WillOnce(Return(1));

  link.transmit(&data, 1);
}

// TODO add receiving.