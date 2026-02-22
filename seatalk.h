// seatalk.h
#ifndef __SEATALK_H__
#define __SEATALK_H__

#include "esphome.h"
#include <vector>

namespace esphome {
namespace seatalk {

// Forward declarations
class SeaTalkListener;

class SeaTalkComponent : public PollingComponent, public uart::UARTDevice {
 public:
  SeaTalkComponent(UARTComponent *parent) : UARTDevice(parent), PollingComponent(1000) {}
  
  void setup() override {
    while (this->available()) {
      this->read();
    }
    ESP_LOGI("seatalk", "SeaTalk component initialized");
  }

  void loop() override {
    while (this->available()) {
      uint8_t byte = this->read();
      process_byte(byte);
    }
  }

  void update() override {
    // Nothing to do here - messages processed in loop
  }

  void add_listener(SeaTalkListener *listener) {
    listeners_.push_back(listener);
  }

 private:
  enum class ParseState {
    WAIT_FOR_START,
    IN_MESSAGE
  };

  ParseState state_ = ParseState::WAIT_FOR_START;
  uint8_t command_ = 0;
  uint8_t expected_length_ = 0;
  std::vector<uint8_t> data_;
  std::vector<SeaTalkListener *> listeners_;

  uint8_t get_expected_length(uint8_t command) {
    switch (command) {
      case 0x01: return 4;  // Depth
      case 0x20: return 3;  // Apparent wind angle
      case 0x21: return 2;  // Apparent wind speed
      case 0x22: return 2;  // Speed through water
      case 0x23: return 3;  // Water temperature
      case 0x26: return 3;  // Total log
      case 0x30: return 2;  // Compass heading
      default: return 2;
    }
  }

  void process_byte(uint8_t byte) {
    switch (state_) {
      case ParseState::WAIT_FOR_START:
        if (byte != 0x00) {
          command_ = byte;
          expected_length_ = get_expected_length(command_);
          data_.clear();
          state_ = ParseState::IN_MESSAGE;
        }
        break;

      case ParseState::IN_MESSAGE:
        data_.push_back(byte);
        if (data_.size() == expected_length_) {
          handle_complete_message();
          state_ = ParseState::WAIT_FOR_START;
        }
        break;
    }
  }

  void handle_complete_message() {
    ESP_LOGD("seatalk", "Message: cmd=0x%02X", command_);
    for (auto *listener : listeners_) {
      listener->on_seatalk_message(command_, data_);
    }
  }
};

class SeaTalkListener {
 public:
  virtual void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) = 0;
};

// Sensor implementations
class SeaTalkDepthSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x01 && data.size() >= 4) {
      uint16_t raw_depth = (data[0] << 8) | data[1];
      float depth_feet = raw_depth * 0.1f;
      float depth_meters = depth_feet * 0.3048f;
      publish_state(depth_meters);
    }
  }
};

class SeaTalkSpeedSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x22 && data.size() >= 2) {
      uint16_t raw_speed = ((data[0] & 0x7F) << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      publish_state(speed_knots);
    }
  }
};

class SeaTalkWindAngleSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x20 && data.size() >= 3) {
      int16_t angle = (data[0] * 2) - 180;
      publish_state(angle);
    }
  }
};

class SeaTalkWindSpeedSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x21 && data.size() >= 2) {
      uint16_t raw_speed = (data[0] << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      publish_state(speed_knots);
    }
  }
};

class SeaTalkHeadingSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x30 && data.size() >= 2) {
      uint16_t raw_heading = (data[0] << 8) | data[1];
      float heading = raw_heading * 0.1f;
      publish_state(heading);
    }
  }
};

class SeaTalkTemperatureSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x23 && data.size() >= 3) {
      uint16_t raw_temp = (data[0] << 8) | data[1];
      float temp_celsius = (raw_temp * 0.1f) - 10.0f;
      publish_state(temp_celsius);
    }
  }
};

class SeaTalkLogSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x26 && data.size() >= 3) {
      uint32_t raw_log = (data[0] << 16) | (data[1] << 8) | data[2];
      float distance_nm = raw_log * 0.1f;
      publish_state(distance_nm);
    }
  }
};

}  // namespace seatalk
}  // namespace esphome

#endif  // __SEATALK_H__
