import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

ScrollView {
    id: root

    property var monitor

    // Consistent color for each CPU core
    property var coreColors: [
        "#7de2d1", // cpu0 - Teal
        "#9bb8ff", // cpu1 - Blue
        "#c5a6ff", // cpu2 - Purple
        "#e7bd87", // cpu3 - Amber
        "#9be3a5", // cpu4 - Green
        "#ff8f9c", // cpu5 - Pink
        "#7dc8ff", // cpu6 - Sky blue
        "#d5e36b", // cpu7 - Lime
        "#ffb86c", // cpu8 - Orange
        "#bd93f9", // cpu9 - Violet
        "#8be9fd", // cpu10 - Cyan
        "#f1fa8c", // cpu11 - Yellow
        "#ff79c6", // cpu12 - Magenta
        "#50fa7b", // cpu13 - Bright green
        "#ff5555", // cpu14 - Red
        "#6272a4"  // cpu15 - Slate blue
    ]

    function colorForCore(name) {
        const index = parseInt(String(name).replace("cpu", ""))

        if (isNaN(index) || index < 0)
            return coreColors[0]

        return coreColors[index % coreColors.length]
    }

    clip: true
    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth
        spacing: 14

        // ==================== CPU SUMMARY ====================
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "TOTAL USAGE",
                        value: root.monitor
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A",
                        color: "#7de2d1"
                    },
                    {
                        label: "LOGICAL CORES",
                        value: root.monitor
                            ? String(root.monitor.coreCount)
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "TEMPERATURE",
                        value: root.monitor && root.monitor.temperature > 0
                            ? Number(root.monitor.temperature).toFixed(1) + " °C"
                            : "N/A",
                        color: "#e7bd87"
                    },
                    {
                        label: "FREQUENCY",
                        value: root.monitor && root.monitor.frequency > 0
                            ? Number(root.monitor.frequency).toFixed(0) + " MHz"
                            : "N/A",
                        color: "#c5a6ff"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 76

                    radius: 8
                    color: "#111820"
                    border.color: "#283640"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 5

                        Text {
                            text: modelData.label
                            color: "#83939f"
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Text {
                            text: modelData.value
                            color: modelData.color
                            font.pixelSize: 20
                            font.bold: true
                        }
                    }
                }
            }
        }

        // ==================== COMBINED CPU HISTORY ====================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 270

            radius: 10
            color: "#111820"
            border.color: "#283640"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text {
                    text: "ALL CORES · COMBINED HISTORY"
                    color: "#e6edf3"
                    font.pixelSize: 12
                    font.bold: true
                }

                MultiSeriesChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    series: {
                        const result = []
                        const data = root.monitor
                            ? root.monitor.coreHistory
                            : []

                        for (let i = 0; i < data.length; ++i) {
                            result.push({
                                name: data[i].name,
                                samples: Array.from(data[i].history || []),
                                color: root.colorForCore(data[i].name)
                            })
                        }

                        return result
                    }

                    emptyMessage: "Collecting per-core samples..."
                }

                // Color legend below the combined chart
                Flow {
                    Layout.fillWidth: true
                    spacing: 12

                    Repeater {
                        model: root.monitor
                            ? root.monitor.coreHistory
                            : []

                        delegate: Row {
                            required property var modelData

                            spacing: 5

                            Rectangle {
                                width: 9
                                height: 9
                                radius: 3
                                color: root.colorForCore(modelData.name)

                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: String(modelData.name).toUpperCase()
                                color: "#aab7c4"
                                font.pixelSize: 10

                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                }
            }
        }

        // ==================== INDIVIDUAL CORE USAGE ====================
        Text {
            text: "INDIVIDUAL CORE USAGE"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: root.monitor
                    ? root.monitor.coreHistory
                    : []

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    Layout.minimumWidth: 0

                    radius: 8
                    color: "#111820"
                    border.color: "#283640"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Text {
                                text: String(modelData.name).toUpperCase()
                                color: "#83939f"
                                font.pixelSize: 10
                                font.bold: true
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            Text {
                                text: Number(modelData.usage).toFixed(1) + "%"
                                color: root.colorForCore(modelData.name)
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }

                        ResourceChart {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            samples: Array.from(modelData.history || [])
                            lineColor: root.colorForCore(modelData.name)
                            emptyMessage: "Collecting samples"
                        }
                    }
                }
            }
        }
    }
}