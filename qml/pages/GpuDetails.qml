
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

ScrollView {
    id: root

    property var monitor

    clip: true
    contentWidth: availableWidth

    ColumnLayout {
        width: root.availableWidth
        spacing: 14

        Text {
            text: "GPU OVERVIEW"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        // GPU name and status
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 76

            radius: 8
            color: "#111820"
            border.color: "#283640"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 12

                Rectangle {
                    width: 40
                    height: 40
                    radius: 8
                    color: "#302746"

                    Text {
                        anchors.centerIn: parent
                        text: "GPU"
                        color: "#c5a6ff"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        Layout.fillWidth: true
                        text: root.monitor
                            ? root.monitor.name
                            : "GPU unavailable"
                        color: "#e6edf3"
                        font.pixelSize: 15
                        font.bold: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: "NVIDIA GPU monitoring"
                        color: "#83939f"
                        font.pixelSize: 10
                    }
                }

                Rectangle {
                    radius: 10
                    color: root.monitor && root.monitor.available
                        ? "#17352f" : "#3a2428"
                    implicitWidth: gpuStatus.implicitWidth + 18
                    implicitHeight: 28

                    Text {
                        id: gpuStatus
                        anchors.centerIn: parent
                        text: root.monitor && root.monitor.available
                            ? "AVAILABLE" : "UNAVAILABLE"
                        color: root.monitor && root.monitor.available
                            ? "#7de2d1" : "#ff8f9c"
                        font.pixelSize: 9
                        font.bold: true
                    }
                }
            }
        }

        // GPU metrics
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "GPU UTILIZATION",
                        value: root.monitor && root.monitor.available
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A",
                        color: "#c5a6ff"
                    },
                    {
                        label: "TEMPERATURE",
                        value: root.monitor && root.monitor.available
                            ? Number(root.monitor.temperature).toFixed(1) + " °C"
                            : "N/A",
                        color: "#e7bd87"
                    },
                    {
                        label: "VRAM USED",
                        value: root.monitor && root.monitor.available
                            ? (Number(root.monitor.usedVRAM) / 1024).toFixed(2) + " GiB"
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "TOTAL VRAM",
                        value: root.monitor && root.monitor.available
                            ? (Number(root.monitor.totalVRAM) / 1024).toFixed(2) + " GiB"
                            : "N/A",
                        color: "#7de2d1"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredHeight: 78

                    radius: 8
                    color: "#111820"
                    border.color: "#283640"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 6

                        Text {
                            text: modelData.label
                            color: "#83939f"
                            font.pixelSize: 9
                            font.bold: true
                        }

                        Text {
                            text: modelData.value
                            color: modelData.color
                            font.pixelSize: 16
                            font.bold: true
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        // VRAM usage breakdown
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 112

            radius: 10
            color: "#111820"
            border.color: "#283640"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "VIDEO MEMORY BREAKDOWN"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: root.monitor && root.monitor.available
                            ? Number(root.monitor.totalVRAM > 0
                                ? root.monitor.usedVRAM / root.monitor.totalVRAM * 100
                                : 0).toFixed(1) + "%"
                            : "N/A"
                        color: "#c5a6ff"
                        font.pixelSize: 13
                        font.bold: true
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 18
                    radius: 4
                    color: "#283640"
                    clip: true

                    Rectangle {
                        width: parent.width * (
                            root.monitor && root.monitor.available
                            && root.monitor.totalVRAM > 0
                            ? Math.max(0, Math.min(1,
                                root.monitor.usedVRAM / root.monitor.totalVRAM))
                            : 0)
                        height: parent.height
                        radius: 4
                        color: "#c5a6ff"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "● Used VRAM"
                        color: "#c5a6ff"
                        font.pixelSize: 10
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "● Free VRAM"
                        color: "#83939f"
                        font.pixelSize: 10
                    }
                }
            }
        }

        // GPU utilization history
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 225

            radius: 10
            color: "#111820"
            border.color: "#283640"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "GPU UTILIZATION HISTORY"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: root.monitor && root.monitor.available
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A"
                        color: "#c5a6ff"
                        font.pixelSize: 13
                        font.bold: true
                    }
                }

                ResourceChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    samples: root.monitor
                        ? Array.from(root.monitor.history).map(Number)
                        : []

                    lineColor: "#c5a6ff"
                    emptyMessage: "Collecting GPU samples..."
                }
            }
        }

        // VRAM usage history
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 225

            radius: 10
            color: "#111820"
            border.color: "#283640"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "VRAM USAGE HISTORY"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: root.monitor && root.monitor.available
                            && root.monitor.totalVRAM > 0
                            ? Number(root.monitor.usedVRAM
                                / root.monitor.totalVRAM * 100).toFixed(1) + "%"
                            : "N/A"
                        color: "#9bb8ff"
                        font.pixelSize: 13
                        font.bold: true
                    }
                }

                ResourceChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    samples: root.monitor && root.monitor.available
                        && root.monitor.totalVRAM > 0
                        ? Array.from(root.monitor.vramHistory)
                            .map(value => Number(value)
                                / Number(root.monitor.totalVRAM) * 100)
                        : []

                    lineColor: "#9bb8ff"
                    emptyMessage: "Collecting VRAM samples..."
                }
            }
        }

        Item {
            Layout.preferredHeight: 4
        }
    }
}
