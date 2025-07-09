const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>R.O.V.E.R Control</title>
  <style>
    body {
      font-family: sans-serif;
      margin: 0;
      display: flex;
      height: 100vh;
    }
    #menu {
      background-color: #222;
      color: white;
      width: 220px;
      padding: 20px;
    }
    #menu h2 {
      font-size: 1.2em;
      margin-bottom: 10px;
    }
    #menu button {
      display: block;
      width: 100%;
      padding: 10px;
      margin: 5px 0;
      background: #444;
      color: white;
      border: none;
      cursor: pointer;
    }
    #menu button:hover {
      background: #666;
    }
    #main {
      flex: 1;
      padding: 20px;
    }
    #statusLight {
      width: 140px;
      padding: 10px;
      background: gray;
      color: white;
      text-align: center;
      margin-bottom: 10px;
    }
  </style>
</head>
<body>
  <div id="menu">
    <h2>Commands</h2>
    <button onclick="sendCmd('areyou')">Ping</button>
    <button onclick="sendCmd('estop')">E-Stop</button>
    <button onclick="sendCmd('override_on')">Override ON</button>
    <button onclick="sendCmd('override_off')">Override OFF</button>
    <button onclick="sendCmd('sensordata')">Sensor Data</button>
  </div>

  <div id="main">
    <h1>R.O.V.E.R. Manual Override</h1>

    <div id="statusLight">Idle</div>

    <input type="text" id="cmdBox" placeholder="Send custom command" />
    <button onclick="sendCustom()">Send</button>

    <h2>LoRa Command</h2>
    <form onsubmit="sendLoRa(event)">
      <input type="text" id="loraMsg" placeholder="Enter LoRa message" />
      <button type="submit">Send LoRa</button>
    </form>

    <pre id="log"></pre>
  </div>

  <script>
    const STATUS_RESET_TIME_MS = 5000;
    let statusResetTimer = null;

    function getTimestamp() {
      return new Date().toLocaleTimeString();
    }

    function log(msg, ack = false, err = false) {
      const logBox = document.getElementById("log");
      logBox.textContent += `\n[${getTimestamp()}] ${msg}`;

      const status = document.getElementById("statusLight");
      if (err) {
        status.style.background = "red";
        status.textContent = "ERROR";
      } else if (ack) {
        status.style.background = "green";
        status.textContent = "ACK";
      } else {
        status.style.background = "orange";
        status.textContent = "SENDING...";
      }

      clearTimeout(statusResetTimer);
      statusResetTimer = setTimeout(() => {
        status.style.background = "gray";
        status.textContent = "Idle";
      }, STATUS_RESET_TIME_MS);
    }

    function sendCmd(x) {
      log("Sending: " + x);
      fetch('/cmd?x=' + x)
        .then(r => r.text())
        .then(t => log("Response: " + t, true))
        .catch(err => log("Error: " + err, false, true));
    }

    function sendCustom() {
      const x = document.getElementById("cmdBox").value;
      sendCmd(x);
    }

    function sendLoRa(e) {
      e.preventDefault();
      const msg = document.getElementById('loraMsg').value;
      log("Sending LoRa: " + msg);
      fetch('/cmd', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: 'lora=' + encodeURIComponent(msg)
      })
      .then(res => res.text())
      .then(txt => log("LoRa: " + txt, true))
      .catch(err => log("LoRa Error: " + err, false, true));
    }
  </script>
</body>
</html>
)rawliteral";
