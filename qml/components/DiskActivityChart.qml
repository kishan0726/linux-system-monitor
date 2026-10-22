
import QtQuick

Item {
    id: root

    property var readSamples: []
    property var writeSamples: []
    property color readColor: "#9bb8ff"
    property color writeColor: "#e7bd87"

    Canvas {
        id: canvas
        anchors.fill: parent
        renderTarget: Canvas.Image

        onPaint: {
            const ctx = getContext("2d")
            const w = width
            const h = height

            ctx.clearRect(0, 0, w, h)

            if (w <= 0 || h <= 0)
                return

            const reads = Array.from(root.readSamples || [])
                .map(Number)
                .map(v => Number.isFinite(v) ? Math.max(0, v) : 0)

            const writes = Array.from(root.writeSamples || [])
                .map(Number)
                .map(v => Number.isFinite(v) ? Math.max(0, v) : 0)

            const count = Math.min(reads.length, writes.length)

            ctx.strokeStyle = "#283640"
            ctx.lineWidth = 1

            for (let i = 0; i < 5; ++i) {
                const y = i * h / 4
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(w, y)
                ctx.stroke()
            }

            if (count < 2) {
                ctx.fillStyle = "#83939f"
                ctx.font = "12px sans-serif"
                ctx.fillText("Collecting disk activity samples...", 10, 20)
                return
            }

            let maxValue = 1

            for (let i = 0; i < count; ++i)
                maxValue = Math.max(maxValue, reads[i], writes[i])

            maxValue *= 1.15

            const stepX = w / (count - 1)

            function drawSeries(values, color) {
                ctx.beginPath()

                for (let i = 0; i < count; ++i) {
                    const x = i * stepX
                    const y = h - (values[i] / maxValue) * (h - 8) - 4

                    if (i === 0)
                        ctx.moveTo(x, y)
                    else
                        ctx.lineTo(x, y)
                }

                ctx.strokeStyle = color
                ctx.lineWidth = 2
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.stroke()
            }

            drawSeries(reads, root.readColor)
            drawSeries(writes, root.writeColor)

            ctx.fillStyle = "#83939f"
            ctx.font = "11px sans-serif"
            ctx.fillText("Scale: 0–" + maxValue.toFixed(2) + " MiB/s",
                         8, 14)
        }
    }

    onReadSamplesChanged: canvas.requestPaint()
    onWriteSamplesChanged: canvas.requestPaint()
    onReadColorChanged: canvas.requestPaint()
    onWriteColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Component.onCompleted: canvas.requestPaint()
}
