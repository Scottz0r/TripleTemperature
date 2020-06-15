# Triple Temperature

Triple temperature sensor with agreement voting. This project uses three  MCP9808 temperature sensors with a voting engine to report temperatures. At least two sensors have to agree in order for a temperature to be reported successfully.

## Specs

Hardware:
- ATmega328P (16 MHz, 2KB SRAM, 32KB Flash)
- 3x MCP 9808 temperature sensors (connected to I2C bus).

Timings:
- Messages will be sent within 60ms after receiving the final request byte. This is enforced by the watchdog timer. Typical response is less than 10ms.
- All bytes in request messages must be sent within 10ms, otherwise the request will be ignored.

## Wiring

Wiring to match the `prj_config.h` header:
- All MCP 9808 SDA are connected together.
- All MCP 9808 SCL are connected together.
- MCP 9808 #1 has pin A0 connected to 5V.
- MCP 9808 #2 has pin A1 connected to 5V.
- Uno SDA and SCL are connect to the MCP 9808 SDA and SCL respectively.

## Tests

Code is tested with `boost.test` (header only version).  Tests are located in the `tests` directory. A Visual Studio 2019 project exists for building and running tests. Tests can also run with Clang.

### Code Coverage 

Clang 9.0.1 and `grcov` are used for test code coverage. The test build script `build_test_clang.ps1` is available for windows. This requires `$env:BOOST_PATH` to be set to the boost root directory before running.

HTML code coverage reported created by grcov will be in `/debug/coverage/` after running the test script.

## Serial Tester

A Windows serial tester project is the `serial_tester_windows` directory. This uses Windows COM APIs to send and receive messages to the Triple Temperature project.

## Messages

### 1. Temperature

|Byte(s)    |Description                |
|-----------|---------------------------|
|0          |Message Identifier         |
|1          |Status Code                |
|2-3        |Temperature 0              |
|4-5        |Temperature 1              |
|6-7        |Temperature 2              |
|8          |Temperature Agreement Bits |
|9-10       |Average Temperature        |
|11         |Checksum                   |

### 2. System Status

|Byte(s)    |Description                |
|-----------|---------------------------|
|0          |Message Identifier         |
|1          |System Status              |
|2          |Sensor Status Bits         |
|3          |Checksum                   |

### 3. Error

|Byte(s)    |Description                |
|-----------|---------------------------|
|0          |Message Identifier         |
|1          |Error Code                 |
|2          |Checksum                   |

Possible error code values

0. Bad Request
1. System Failure (Requires reboot)

### 4. Request Message

|Byte(s)    |Description                |
|-----------|---------------------------|
|0          |Message Identifier         |
|1          |Request Type               |
|2          |Checksum                   |

Possible request type values are:

0. Temperature
1. System Status
