
const $ = (id) => document.getElementById(id);

const history = {
    cpu: [],
    ram: [],
    gpu: [],
    network: [],
    vram: [],
    disk: []
};

const MAX_HISTORY = 80;
let latestMetrics = {};
let backendConnected = false;

const chartConfigs = {
    "cpu-chart": {
        key: "cpu",
        color: "#12c9ff",
        max: 100,
        labels: ["100%", "50%", "0%"]
    },
    "ram-chart": {
        key: "ram",
        color: "#e64cff",
        max: 100,
        labels: ["100%", "50%", "0%"],
        fill: true
    },
    "gpu-chart": {
        key: "gpu",
        color: "#32f36d",
        max: 100,
        labels: ["100%", "50%", "0%"],
        fill: true
    },
    "network-chart": {
        key: "network",
        color: "#32f36d",
        max: 200,
        labels: ["200 MB/s", "100 MB/s", "0 MB/s"]
    },
    "vram-chart": {
        key: "vram",
        color: "#32f36d",
        max: 100,
        labels: ["100%", "50%", "0%"],
        fill: true
    },
    "disk-chart": {
        key: "disk",
        color: "#ffc928",
        max: 100,
        labels: ["100%", "50%", "0%"],
        fill: true
    }
};

function setGauge(id, value) {
    const gauge = $(id);
    if (!gauge || !Number.isFinite(Number(value))) return;

    value = Math.max(0, Math.min(100, Number(value)));
    gauge.style.setProperty("--value", value);

    const label = gauge.querySelector("strong");
    if (label) label.textContent = `${Math.round(value)}%`;
}

function setText(id, value) {
    const element = $(id);
    if (element) element.textContent = value;
}

function setBar(id, percentage) {
    const element = $(id);
    if (!element || !Number.isFinite(Number(percentage))) return;

    const value = Math.max(0, Math.min(100, Number(percentage)));
    element.style.width = `${value}%`;
}

function pushHistory(key, value) {
    if (!Number.isFinite(Number(value))) return;

    history[key].push(Number(value));

    if (history[key].length > MAX_HISTORY) {
        history[key].shift();
    }
}

function createBars(id, values, color) {
    const container = $(id);
    if (!container) return;

    container.replaceChildren();

    values.forEach((value) => {
        const bar = document.createElement("span");
        bar.style.height = `${Math.max(2, Math.min(100, value))}%`;
        if (color) bar.style.background = color;
        container.appendChild(bar);
    });
}

function drawChart(id, values, options = {}) {
    const canvas = $(id);
    if (!canvas) return;

    const rect = canvas.getBoundingClientRect();
    if (rect.width <= 0 || rect.height <= 0) return;

    const dpr = window.devicePixelRatio || 1;

    const pixelWidth = Math.round(rect.width * dpr);
    const pixelHeight = Math.round(rect.height * dpr);

    if (canvas.width !== pixelWidth || canvas.height !== pixelHeight) {
        canvas.width = pixelWidth;
        canvas.height = pixelHeight;
    }

    const ctx = canvas.getContext("2d");
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

    const w = rect.width;
    const h = rect.height;
    const left = 40;
    const right = 5;
    const top = 8;
    const bottom = 17;
    const plotW = w - left - right;
    const plotH = h - top - bottom;

    const color = options.color || "#12c9ff";
    const max = options.max || 100;
    const labels = options.labels || ["100%", "50%", "0%"];

    ctx.clearRect(0, 0, w, h);
    ctx.font = "11px monospace";
    ctx.lineWidth = 1;

    labels.forEach((label, i) => {
        const y = top + (plotH * i) / Math.max(1, labels.length - 1);

        ctx.strokeStyle = "#1a2b39";
        ctx.beginPath();
        ctx.moveTo(left, y);
        ctx.lineTo(w - right, y);
        ctx.stroke();

        ctx.fillStyle = "#dbe7f4";
        ctx.textAlign = "right";
        ctx.fillText(label, left - 6, y + 4);
    });

    for (let i = 0; i <= 8; i++) {
        const x = left + (plotW * i) / 8;

        ctx.strokeStyle = "#142431";
        ctx.beginPath();
        ctx.moveTo(x, top);
        ctx.lineTo(x, h - bottom);
        ctx.stroke();
    }

    if (!values.length) return;

    ctx.beginPath();

    values.forEach((value, i) => {
        const x = left + (i / Math.max(1, values.length - 1)) * plotW;
        const y = top + (1 - Math.max(0, Math.min(max, value)) / max) * plotH;

        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
    });

    if (options.fill) {
        ctx.lineTo(w - right, h - bottom);
        ctx.lineTo(left, h - bottom);
        ctx.closePath();

        const gradient = ctx.createLinearGradient(0, top, 0, h - bottom);
        gradient.addColorStop(0, color + "66");
        gradient.addColorStop(1, color + "05");

        ctx.fillStyle = gradient;
        ctx.fill();

        ctx.beginPath();

        values.forEach((value, i) => {
            const x = left + (i / Math.max(1, values.length - 1)) * plotW;
            const y = top + (1 - Math.max(0, Math.min(max, value)) / max) * plotH;

            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        });
    }

    ctx.strokeStyle = color;
    ctx.lineWidth = 1.3;
    ctx.stroke();
}

function updateClock() {
    const now = new Date();

    setText("date", now.toLocaleDateString("en-GB", {
        weekday: "short",
        day: "2-digit",
        month: "short",
        year: "numeric"
    }));

    setText("time", now.toLocaleTimeString("en-GB"));
}

function updateDashboard(data) {
    latestMetrics = { ...latestMetrics, ...data };

    // CPU utilization.
    if (Number.isFinite(Number(data.cpu))) {
        setGauge("cpu-gauge", data.cpu);
        pushHistory("cpu", data.cpu);
    }

    // RAM usage. Values are in GB.
    if (Number.isFinite(Number(data.ramTotal))) {
        setText("ram-used", `${Number(data.ramUsed).toFixed(1)} GB`);
        setText("ram-used-legend", `${Number(data.ramUsed).toFixed(1)} GB`);

        const total = Number(data.ramTotal);
        const used = Number(data.ramUsed || 0);
        const available = Math.max(0, total - used);

        const legend = document.querySelectorAll(".legend-list p b");

        if (legend.length >= 3) {
            legend[1].textContent = `${available.toFixed(1)} GB`;
            legend[2].textContent = `${total.toFixed(1)} GB`;
        }
    }

    if (Number.isFinite(Number(data.ramPercent))) {
        setBar("ram-bar", data.ramPercent);
        setText("ram-percent", Number(data.ramPercent).toFixed(1));
        pushHistory("ram", data.ramPercent);
    }

    // Swap utilization.
    if (Number.isFinite(Number(data.swapPercent))) {
        const swapCard = document.querySelector(".metric-card.swap");

        if (swapCard) {
            const value = Math.max(0, Math.min(100, Number(data.swapPercent)));
            const metricValue = swapCard.querySelector(".metric-value");

            if (metricValue) {
                metricValue.innerHTML =
                    `${value.toFixed(1)}<span>%</span>`;
            }

            const fill = swapCard.querySelector(".progress .bar-fill");

            if (fill) fill.style.width = `${value}%`;
        }
    }

    // GPU data will populate these when the C++ collector is connected.
    if (Number.isFinite(Number(data.gpu))) {
        setGauge("gpu-gauge", data.gpu);
        pushHistory("gpu", data.gpu);
    }

    if (Number.isFinite(Number(data.vramPercent))) {
        pushHistory("vram", data.vramPercent);
    }

    if (Number.isFinite(Number(data.diskPercent))) {
        pushHistory("disk", data.diskPercent);
    }

    if (Number.isFinite(Number(data.networkDownload))) {
        pushHistory("network", data.networkDownload);
    }

    // Draw only histories for which data has actually arrived.
    Object.entries(chartConfigs).forEach(([id, config]) => {
        const values = history[config.key];

        if (values.length) {
            drawChart(id, values, config);
        }
    });
}

function connectToBackend() {
    if (typeof qt === "undefined" || typeof QWebChannel === "undefined") {
        console.warn("Qt WebChannel is unavailable. Live data is disconnected.");
        return;
    }

    new QWebChannel(qt.webChannelTransport, function (channel) {
        const monitor = channel.objects.monitor;

        if (!monitor) {
            console.error("The C++ monitor object was not registered.");
            return;
        }

        monitor.metricsUpdated.connect(function (metrics) {
            backendConnected = true;
            updateDashboard(metrics);
        });

        console.log("Connected to C++ monitoring backend.");
    });
}

updateClock();
window.setInterval(updateClock, 1000);

window.addEventListener("resize", () => {
    Object.entries(chartConfigs).forEach(([id, config]) => {
        if (history[config.key].length) {
            drawChart(id, history[config.key], config);
        }
    });
});

connectToBackend();
