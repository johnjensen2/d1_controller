#ifndef ROVER_CODES_H
#define ROVER_CODES_H

// ----------------------------------------
// Core Command IDs (shared across CAN, UART, ESP-NOW)
// ----------------------------------------

// General system control
#define CMD_ARE_YOU_THERE         0x05
#define CMD_EMERGENCY_STOP        0x06
#define CMD_RESET_ALL             0x07

// Mode control
#define CMD_OVERRIDE_ON           0x10
#define CMD_OVERRIDE_OFF          0x11
#define CMD_ENTER_SLEEP           0x12
#define CMD_WAKE_UP               0x13

// Time sync
#define CMD_TIME_SYNC_REQUEST     0x20
#define CMD_TIME_SYNC_BROADCAST   0x21

// Sensor and telemetry
#define CMD_SEND_SENSOR_DATA      0x30
#define CMD_SEND_GPS              0x31
#define CMD_SEND_IMU              0x32
#define CMD_SEND_BATTERY          0x33

// Motor control
#define CMD_MOTOR_STOP_ALL        0x40
#define CMD_MOTOR_SET_SPEED       0x41
#define CMD_MOTOR_BRAKE           0x42

// Steering control
#define CMD_STEER_FRONT_SET       0x50
#define CMD_STEER_REAR_SET        0x51

// Self-reporting
#define CMD_SELF_REPORT_REQUEST   0x60
#define CMD_SELF_REPORT_RESPONSE  0x61

// LoRa passthrough (text)
#define CMD_LORA_STRING           0xA0

// Debug/Diagnostics
#define CMD_DEBUG_PRINT           0xF0
#define CMD_HEARTBEAT             0xF1

// ----------------------------------------
// Role identifiers (used in SelfReportPayload.role)
// ----------------------------------------

#define ROLE_MAIN_CONTROLLER      0x01
#define ROLE_MOTOR_POD            0x02
#define ROLE_SENSOR_NODE          0x03
#define ROLE_POWER_MANAGER        0x04
#define ROLE_OVERRIDE_UNIT        0x05
#define ROLE_CAMERA_NODE          0x06

// ----------------------------------------
// Command Strings (for web/console UI triggers)
// ----------------------------------------

static const char* CMD_STR_ARE_YOU_THERE    = "areyou";
static const char* CMD_STR_EMERGENCY_STOP   = "estop";
static const char* CMD_STR_OVERRIDE_ON      = "override_on";
static const char* CMD_STR_OVERRIDE_OFF     = "override_off";
static const char* CMD_STR_SEND_SENSOR_DATA = "sensordata";
static const char* CMD_STR_LORA_PREFIX      = "lora";

// ----------------------------------------
// Data Structures
// ----------------------------------------

// Self-report response structure (example: 8 bytes, compact)
struct SelfReportPayload {
  uint8_t role;               // Device role (e.g. ROLE_MOTOR_POD)
  uint8_t status;             // 0 = OK, 1 = WARN, 2 = FAIL
  uint8_t active_components;  // Number of subsystems it manages
  uint8_t flags;              // Bitmask (sensor OK, motor OK, etc.)
  uint32_t uptime_sec;        // Time since boot (for diagnostics)
} __attribute__((packed));    // Required for clean CAN/uC byte layout

#endif // ROVER_CODES_H
