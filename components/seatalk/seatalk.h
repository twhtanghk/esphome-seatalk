#ifndef __SEATALK_H__
#define __SEATALK_H__

#include "esphome.h"
#include "esphome/components/uart/uart.h"
#include <vector>

namespace esphome {
namespace seatalk {

// Forward declarations
class SeaTalkListener;
class SeaTalkComponent;

class SeaTalkListener {
 public:
  virtual void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) = 0;
  virtual ~SeaTalkListener() = default;
};

class SeaTalkComponent : public PollingComponent, public uart::UARTDevice {
 public:
  SeaTalkComponent(uart::UARTComponent *parent) 
      : PollingComponent(1000),
        uart::UARTDevice(parent)
  {
    ESP_LOGI("seatalk", "SeaTalk component created");
  }
  
  void setup() override {
    ESP_LOGI("seatalk", "SeaTalk component initializing...");
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

  void update() override {}

  void add_listener(SeaTalkListener *listener) {
    if (listener) {
      listeners_.push_back(listener);
    }
  }

 private:
  std::vector<SeaTalkListener *> listeners_;
  std::vector<uint8_t> data_;
  uint8_t command_ = 0;
  uint8_t expected_length_ = 0;
  
  enum class ParseState {
    WAIT_FOR_START,
    IN_MESSAGE
  } state_ = ParseState::WAIT_FOR_START;

  uint8_t get_expected_length(uint8_t command) {
    switch (command) {
      case 0x01: return 4;
      case 0x20: return 3;
      case 0x21: return 2;
      case 0x22: return 2;
      case 0x23: return 3;
      case 0x26: return 3;
      case 0x30: return 2;
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
          data_.push_back(byte);
          state_ = ParseState::IN_MESSAGE;
        }
        break;

      case ParseState::IN_MESSAGE:
        data_.push_back(byte);
        if (data_.size() >= expected_length_ + 1) {
          handle_complete_message();
          state_ = ParseState::WAIT_FOR_START;
        }
        break;
    }
  }

  void handle_complete_message() {
    std::vector<uint8_t> payload(data_.begin() + 1, data_.end());
    for (auto *listener : listeners_) {
      if (listener) {
        listener->on_seatalk_message(command_, payload);
      }
    }
  }
};

// Sensor base class - now stores parent reference
class SeaTalkSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  SeaTalkSensor(SeaTalkComponent *parent) : sensor::Sensor() {
    if (parent) {
      parent->add_listener(this);
    }
  }
  virtual ~SeaTalkSensor() = default;
};

// Individual sensor implementations - updated constructors
class SeaTalkDepthSensor : public SeaTalkSensor {
 public:
  SeaTalkDepthSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x01 && data.size() >= 3) {
      uint16_t raw_depth = (data[0] << 8) | data[1];
      float depth_feet = raw_depth * 0.1f;
      float depth_meters = depth_feet * 0.3048f;
      publish_state(depth_meters);
    }
  }
};

class SeaTalkSpeedSensor : public SeaTalkSensor {
 public:
  SeaTalkSpeedSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x22 && data.size() >= 2) {
      uint16_t raw_speed = ((data[0] & 0x7F) << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      publish_state(speed_knots);
    }
  }
};

class SeaTalkWindAngleSensor : public SeaTalkSensor {
 public:
  SeaTalkWindAngleSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x20 && data.size() >= 2) {
      int16_t angle = (data[0] * 2) - 180;
      publish_state(angle);
    }
  }
};

class SeaTalkWindSpeedSensor : public SeaTalkSensor {
 public:
  SeaTalkWindSpeedSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x21 && data.size() >= 2) {
      uint16_t raw_speed = (data[0] << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      publish_state(speed_knots);
    }
  }
};

class SeaTalkHeadingSensor : public SeaTalkSensor {
 public:
  SeaTalkHeadingSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x30 && data.size() >= 2) {
      uint16_t raw_heading = (data[0] << 8) | data[1];
      float heading = raw_heading * 0.1f;
      publish_state(heading);
    }
  }
};

class SeaTalkTemperatureSensor : public SeaTalkSensor {
 public:
  SeaTalkTemperatureSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x23 && data.size() >= 2) {
      uint16_t raw_temp = (data[0] << 8) | data[1];
      float temp_celsius = (raw_temp * 0.1f) - 10.0f;
      publish_state(temp_celsius);
    }
  }
};

class SeaTalkLogSensor : public SeaTalkSensor {
 public:
  SeaTalkLogSensor(SeaTalkComponent *parent) : SeaTalkSensor(parent) {}
  
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
