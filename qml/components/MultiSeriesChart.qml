import QtQuick

Item {
    id: root
    property var series: []
    property string emptyMessage: "Collecting samples"

    Canvas {
        id: canvas
        anchors.fill: parent
        renderTarget: Canvas.FramebufferObject
        antialiasing: true

        onPaint: {
            const ctx = getContext("2d")
            const w = width
            const h = height
            if (w <= 0 || h <= 0) return
            ctx.clearRect(0, 0, w, h)

            ctx.strokeStyle = "#283640"
            ctx.lineWidth = 1
            for (let i = 0; i < 5; ++i) {
                const y = i * h / 4
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(w, y)
                ctx.stroke()
            }

            const items = Array.from(root.series || []).filter(s => s && s.samples && s.samples.length)
            let count = 0
            for (let i = 0; i < items.length; ++i)
                count = Math.max(count, items[i].samples.length)
            if (count < 2) {
                ctx.fillStyle = "#83939f"
                ctx.font = "12px sans-serif"
                ctx.fillText(root.emptyMessage, 10, 20)
                return
            }

            const plotH = Math.max(1, h - 10)
            const stepX = w / Math.max(1, count - 1)
            for (let j = 0; j < items.length; ++j) {
                const item = items[j]
                const values = Array.from(item.samples).slice(-count).map(v => Math.max(0, Math.min(100, Number(v) || 0)))
                if (values.length < 2) continue

                ctx.beginPath()
                for (let i = 0; i < values.length; ++i) {
                    const x = i * stepX
                    const y = h - (values[i] / 100) * plotH - 5
                    if (i === 0) {
                        ctx.moveTo(x, y)
                    } else {
                        const prevX = (i - 1) * stepX
                        const prevY = h - (values[i - 1] / 100) * plotH - 5
                        const midX = (prevX + x) / 2
                        const midY = (prevY + y) / 2
                        ctx.quadraticCurveTo(prevX, prevY, midX, midY)
                        if (i === values.length - 1) ctx.lineTo(x, y)
                    }
                }
                ctx.strokeStyle = item.color || "#7de2d1"
                ctx.lineWidth = 1.6
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.stroke()
            }
        }
    }

    onSeriesChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
