
import QtQuick

Item {
    id: root

    property color lineColor: "#7de2d1"
    property var samples: []
    property string emptyMessage: "Waiting for samples"

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

            // Background grid
            ctx.strokeStyle = "#283640"
            ctx.lineWidth = 1

            for (let i = 0; i < 5; ++i) {
                const y = i * h / 4
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(w, y)
                ctx.stroke()
            }

            const values = Array.from(root.samples || [])
                .map(Number)
                .filter(v => Number.isFinite(v))

            if (values.length < 2) {
                ctx.fillStyle = "#83939f"
                ctx.font = "12px sans-serif"
                ctx.fillText(
                    root.emptyMessage + " (" + values.length + ")",
                    10, 20
                )
                return
            }

            const maxValue = 100
            const stepX = w / (values.length - 1)

            function pointY(value) {
                const v = Math.max(0, Math.min(maxValue, value))
                return h - (v / maxValue) * (h - 8) - 4
            }

            // Filled area
            ctx.beginPath()
            ctx.moveTo(0, h)

            for (let i = 0; i < values.length; ++i) {
                ctx.lineTo(i * stepX, pointY(values[i]))
            }

            ctx.lineTo(w, h)
            ctx.closePath()

            ctx.fillStyle = Qt.rgba(
                root.lineColor.r,
                root.lineColor.g,
                root.lineColor.b,
                0.14
            )
            ctx.fill()

            // CPU usage line
            ctx.beginPath()

            for (let i = 0; i < values.length; ++i) {
                const x = i * stepX
                const y = pointY(values[i])

                if (i === 0)
                    ctx.moveTo(x, y)
                else
                    ctx.lineTo(x, y)
            }

            ctx.strokeStyle = root.lineColor
            ctx.lineWidth = 2
            ctx.lineJoin = "round"
            ctx.lineCap = "round"
            ctx.stroke()

            // Latest value
            const lastValue = Math.max(
                0, Math.min(100, values[values.length - 1])
            )

            ctx.fillStyle = root.lineColor
            ctx.font = "bold 12px sans-serif"
            ctx.fillText(lastValue.toFixed(1) + "%", 8, 16)
        }
    }

    onSamplesChanged: canvas.requestPaint()
    onLineColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Component.onCompleted: canvas.requestPaint()
}
