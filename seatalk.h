#include "esphome.h"
#include <vector>

namespace esphome {
namespace seatalk {

class SeaTalkListener {
 public:
  virtual void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) = 0;
};

class SeaTalkComponent : public PollingComponent, public uart::UARTDevice {
 public:
  SeaTalkComponent(UARTComponent *parent) : UARTDevice(parent), PollingComponent(1000) {
    // 1 second polling is just for housekeeping, SeaTalk messages are received asynchronously
  }

  void setup() override {
    // Clear any pending data
    while (this->available()) {
      this->read();
    }
    ESP_LOGI("seatalk", "SeaTalk component initialized");
  }

  void loop() override {
    // Process incoming SeaTalk messages in real-time
    while (this->available()) {
      uint8_t byte = this->read();
      process_byte(byte);
    }
  }

  void update() override {
    // This runs every second, can be used for polling sensors if needed
    // Most SeaTalk data is push-based, so we don't need to do much here
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

  void process_byte(uint8_t byte) {
    switch (state_) {
      case ParseState::WAIT_FOR_START:
        // SeaTalk messages start with a command byte that's never 0x00
        if (byte != 0x00) {
          command_ = byte;
          expected_length_ = get_expected_length(command_);
          data_.clear();
          state_ = ParseState::IN_MESSAGE;
          ESP_LOGV("seatalk", "Started message: cmd=0x%02X, len=%d", command_, expected_length_);
        }
        break;

      case ParseState::IN_MESSAGE:
        data_.push_back(byte);
        
        // Check if we have all data for this message type
        if (data_.size() == expected_length_) {
          // Complete message received
          handle_complete_message();
          state_ = ParseState::WAIT_FOR_START;
        }
        break;
    }
  }

  uint8_t get_expected_length(uint8_t command) {
    // SeaTalk message lengths based on command byte
    switch (command) {
      case 0x00: return 0;  // Should never happen
      case 0x01: return 4;  // Depth
      case 0x20: return 3;  // Apparent wind angle
      case 0x21: return 2;  // Apparent wind speed
      case 0x22: return 2;  // Speed through water
      case 0x23: return 3;  // Water temperature
      case 0x25: return 3;  // Trip log
      case 0x26: return 3;  // Total log
      case 0x27: return 4;  // Position (lat/lon)
      case 0x30: return 2;  // Compass heading
      case 0x31: return 3;  // Compass heading with variation
      case 0x36: return 3;  // GPS speed
      case 0x38: return 3;  // GPS time/date
      case 0x50: return 2;  // Set depth alarm
      case 0x51: return 2;  // Depth alarm status
      case 0x52: return 2;  // Anchor alarm
      case 0x53: return 2;  // Navigation data
      case 0x54: return 2;  // Waypoint name/next
      case 0x56: return 3;  // Cross track error
      case 0x58: return 4;  // Waypoint lat/lon
      case 0x5A: return 4;  // Waypoint arrival
      case 0x65: return 2;  // Display control
      case 0x66: return 2;  // Keypad data
      default:
        // For unknown commands, try to determine length from first data byte
        ESP_LOGW("seatalk", "Unknown command 0x%02X, guessing length", command);
        return 2;  // Most SeaTalk messages are 2-4 bytes
    }
  }

  void handle_complete_message() {
    ESP_LOGD("seatalk", "SeaTalk message: cmd=0x%02X, data=%s", command_, 
             format_hex(data_).c_str());

    // Notify all listeners
    for (auto *listener : listeners_) {
      listener->on_seatalk_message(command_, data_);
    }
  }

  std::string format_hex(const std::vector<uint8_t> &bytes) {
    char buf[10];
    std::string result;
    for (size_t i = 0; i < bytes.size(); i++) {
      if (i > 0) result += " ";
      snprintf(buf, sizeof(buf), "%02X", bytes[i]);
      result += buf;
    }
    return result;
  }
};

// ==================== Sensor Implementations ====================

class SeaTalkDepthSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x01 && data.size() >= 4) {
      // Depth message format: 0x01 XX YY ZZ
      // Depth in feet = ((XX << 8) | YY) * 0.1
      uint16_t raw_depth = (data[0] << 8) | data[1];
      float depth_feet = raw_depth * 0.1f;
      float depth_meters = depth_feet * 0.3048f;
      
      ESP_LOGD("seatalk_depth", "Depth: %.1f ft (%.1f m)", depth_feet, depth_meters);
      publish_state(depth_meters);
    }
  }
};

class SeaTalkSpeedSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x22 && data.size() >= 2) {
      // Speed through water: 0x22 XX YY
      // Speed in knots = ((XX & 0x7F) << 8 | YY) * 0.1
      uint16_t raw_speed = ((data[0] & 0x7F) << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      
      ESP_LOGD("seatalk_speed", "Speed: %.1f knots", speed_knots);
      publish_state(speed_knots);
    }
  }
};

class SeaTalkWindAngleSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x20 && data.size() >= 3) {
      // Apparent wind angle: 0x20 XX YY ZZ
      // Angle in degrees = (XX * 2) - 180
      int16_t angle = (data[0] * 2) - 180;
      
      ESP_LOGD("seatalk_wind_angle", "Wind angle: %d°", angle);
      publish_state(angle);
    }
  }
};

class SeaTalkWindSpeedSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x21 && data.size() >= 2) {
      // Apparent wind speed: 0x21 XX YY
      // Speed in knots = (XX << 8 | YY) * 0.1
      uint16_t raw_speed = (data[0] << 8) | data[1];
      float speed_knots = raw_speed * 0.1f;
      
      ESP_LOGD("seatalk_wind_speed", "Wind speed: %.1f knots", speed_knots);
      publish_state(speed_knots);
    }
  }
};

class SeaTalkHeadingSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x30 && data.size() >= 2) {
      // Compass heading: 0x30 XX YY
      // Heading in degrees = (XX << 8 | YY) * 0.1
      uint16_t raw_heading = (data[0] << 8) | data[1];
      float heading = raw_heading * 0.1f;
      
      ESP_LOGD("seatalk_heading", "Heading: %.1f°", heading);
      publish_state(heading);
    }
  }
};

class SeaTalkTemperatureSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x23 && data.size() >= 3) {
      // Water temperature: 0x23 XX YY ZZ
      // Temperature in °C = ((XX << 8 | YY) * 0.1) - 10
      uint16_t raw_temp = (data[0] << 8) | data[1];
      float temp_celsius = (raw_temp * 0.1f) - 10.0f;
      
      ESP_LOGD("seatalk_temp", "Temperature: %.1f°C", temp_celsius);
      publish_state(temp_celsius);
    }
  }
};

class SeaTalkLogSensor : public sensor::Sensor, public SeaTalkListener {
 public:
  SeaTalkLogSensor() : Sensor() {}

  void on_seatalk_message(uint8_t command, const std::vector<uint8_t> &data) override {
    if (command == 0x26 && data.size() >= 3) {
      // Total log: 0x26 XX YY ZZ
      // Distance in nm = ((XX << 16) | (YY << 8) | ZZ) * 0.1
      uint32_t raw_log = (data[0] << 16) | (data[1] << 8) | data[2];
      float distance_nm = raw_log * 0.1f;
      
      ESP_LOGD("seatalk_log", "Total log: %.1f nm", distance_nm);
      publish_state(distance_nm);
    }
  }
};

class SeaTalkComponent : public PollingComponent, public uart::UARTDevice {
 public:
  // ... existing code ...

  void send_command(uint8_t command, const std::vector<uint8_t> &data) {
    // SeaTalk requires bus arbitration - wait for bus idle
    delay(10);  // Simple wait, could be improved with bus sensing
    
    // Send the message
    this->write_byte(command);
    for (uint8_t byte : data) {
      this->write_byte(byte);
    }
    
    ESP_LOGD("seatalk", "Sent command: 0x%02X", command);
  }

  // Convenience methods for common commands
  void request_depth() {
    send_command(0x01, {0x00, 0x00, 0x00});
  }

  void request_wind() {
    send_command(0x20, {0x00, 0x00});
  }
};

}  // namespace seatalk
}  // namespace esphome
