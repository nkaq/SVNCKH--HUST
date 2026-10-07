# ESP32-S3 Firmware

Modules:
```text
drivers/
acquisition/
logging/
dsp/
oled/
tests/
```

Week 3 target:
- interrupt / DATA_READY where suitable
- FIFO / ring buffer
- timestamp
- error counters
- state machine
- logging
- OLED status
- stress test

Không dùng `Serial.print()` từng sample làm architecture acquisition chính.
