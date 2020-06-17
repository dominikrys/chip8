const statusElement = document.getElementById("status");
const progressElement = document.getElementById("progress");
const spinnerElement = document.getElementById("spinner");

Module = {
  noInitialRun: true,
  canvas: (function () {
    const canvas = document.getElementById("canvas");
    canvas.addEventListener(
      "webglcontextlost",
      function (e) {
        alert("WebGL context lost. You will need to reload the page.");
        e.preventDefault();
      },
      false
    );

    return canvas;
  })(),
  setStatus: function (text) {
    if (!Module.setStatus.last)
      Module.setStatus.last = { time: Date.now(), text: "" };
    if (text === Module.setStatus.last.text) return;
    const m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    const now = Date.now();
    if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    Module.setStatus.last.time = now;
    Module.setStatus.last.text = text;
    if (m) {
      text = m[1];
      progressElement.value = parseInt(m[2]) * 100;
      progressElement.max = parseInt(m[4]) * 100;
      progressElement.hidden = false;
      spinnerElement.hidden = false;
    } else {
      progressElement.value = null;
      progressElement.max = null;
      progressElement.hidden = true;
      if (!text) spinnerElement.hidden = true;
    }
    statusElement.innerHTML = text;
  },
  running: false,
  selectedRomUInt8Array: [],
  ticksPerSec: 10,
  loadCurrentRom: function () {
    this.ccall(
      "loadRom",
      "null",
      ["array", "number"],
      [this.selectedRomUInt8Array, this.ticksPerSec]
    );
  },
};

Module.setStatus("Downloading...");
window.onerror = function () {
  Module.setStatus("Exception thrown, see JavaScript console");
  spinnerElement.style.display = "none";
};

const startStopButton = document.getElementById("start-stop-button");

function getRomOptionsFromDropdown(optionText) {
  if (optionText === "SELECT ROM") {
    return;
  }

  let romOptions = JSON.parse(optionText);

  const romName = romOptions["name"];
  const romPath = "bin/roms/revival/" + romName + "\0";
  Module.selectedRomUInt8Array = new TextEncoder().encode(romPath);

  Module.ticksPerSec = romOptions["ticksPerSec"];

  startStopButton.disabled = false;

  Module.loadCurrentRom();
}

Module["onRuntimeInitialized"] = function () {
  getRomOptionsFromDropdown(document.querySelector("#rom-dropdown").value);

  document.querySelector("#rom-dropdown").onchange = function (event) {
    getRomOptionsFromDropdown(event.target.value);
  };

  startStopButton.addEventListener("click", () => {
    if (Module.running) {
      Module.ccall("stop", null, null, null);
      startStopButton.innerHTML = "START";
      Module.running = false;
    } else {
      Module.loadCurrentRom();
      Module.ccall("main", null, null, null);
      startStopButton.innerHTML = "STOP";
      Module.running = true;
    }
  });
};
