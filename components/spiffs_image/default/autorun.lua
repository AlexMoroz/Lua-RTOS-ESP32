-- Configure WiFi access point
net.wf.setup(
    net.wf.mode.AP,
    "esp32",
    "123qweasd",
    net.wf.powersave.NONE,
    4,
    false
)

-- Start wifi
net.wf.start()

-- Get current time from sntp server
net.service.sntp.start()
net.service.sntp.stop()

-- Start http server
net.service.http.start()

