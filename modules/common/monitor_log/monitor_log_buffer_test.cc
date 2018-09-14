/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include <sys/resource.h>
#include <sys/time.h>

#include "modules/common/monitor_log/monitor_log_buffer.h"

#include "gtest/gtest.h"

#include "cybertron/common/log.h"

namespace apollo {
namespace common {
namespace monitor {

class MonitorBufferTest : public ::testing::Test {
 protected:
  void SetUp() override { cybertron::Init(); }
  void TearDown() override {}
  MonitorLogBuffer buffer_{MonitorMessageItem::CONTROL};
};

TEST_F(MonitorBufferTest, RegisterMacro) {
  {
    buffer_.INFO("Info");
    EXPECT_EQ(MonitorMessageItem::INFO, buffer_.level_);
    ASSERT_EQ(1, buffer_.monitor_msg_items_.size());
    const auto &item = buffer_.monitor_msg_items_.back();
    EXPECT_EQ(MonitorMessageItem::INFO, item.first);
    EXPECT_EQ("Info", item.second);
  }

  {
    buffer_.ERROR("Error");
    EXPECT_EQ(MonitorMessageItem::ERROR, buffer_.level_);
    ASSERT_EQ(2, buffer_.monitor_msg_items_.size());
    const auto &item = buffer_.monitor_msg_items_.back();
    EXPECT_EQ(MonitorMessageItem::ERROR, item.first);
    EXPECT_EQ("Error", item.second);
  }
}

TEST_F(MonitorBufferTest, AddMonitorMsgItem) {
  buffer_.AddMonitorMsgItem(MonitorMessageItem::ERROR, "TestError");
  EXPECT_EQ(MonitorMessageItem::ERROR, buffer_.level_);
  ASSERT_EQ(1, buffer_.monitor_msg_items_.size());
  const auto &item = buffer_.monitor_msg_items_.back();
  EXPECT_EQ(MonitorMessageItem::ERROR, item.first);
  EXPECT_EQ("TestError", item.second);
}

TEST_F(MonitorBufferTest, Operator) {
  buffer_.ERROR() << "Hi";
  EXPECT_EQ(MonitorMessageItem::ERROR, buffer_.level_);
  ASSERT_EQ(1, buffer_.monitor_msg_items_.size());
  auto &item = buffer_.monitor_msg_items_.back();
  EXPECT_EQ(MonitorMessageItem::ERROR, item.first);
  EXPECT_EQ("Hi", item.second);
  buffer_ << " How"
          << " are"
          << " you";
  EXPECT_EQ(MonitorMessageItem::ERROR, buffer_.level_);
  ASSERT_EQ(1, buffer_.monitor_msg_items_.size());
  EXPECT_EQ(MonitorMessageItem::ERROR, item.first);
  EXPECT_EQ("Hi How are you", item.second);

  buffer_.INFO() << 3 << "pieces";
  EXPECT_EQ(MonitorMessageItem::INFO, buffer_.level_);
  ASSERT_EQ(2, buffer_.monitor_msg_items_.size());
  auto item2 = buffer_.monitor_msg_items_.back();
  EXPECT_EQ(MonitorMessageItem::INFO, item2.first);
  EXPECT_EQ("3pieces", item2.second);

  const char *fake_input = nullptr;
  EXPECT_TRUE(&(buffer_.INFO() << fake_input) == &buffer_);
}

}  // namespace monitor
}  // namespace common
}  // namespace apollo
