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

        // ==================== RAM OVERVIEW ====================
        Text {
            text: "RAM OVERVIEW"
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
                model: [
                    {
                        label: "TOTAL RAM",
                        value: root.monitor
                            ? Number(root.monitor.totalGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "USED RAM",
                        value: root.monitor
                            ? Number(root.monitor.usedGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#7de2d1"
                    },
                    {
                        label: "AVAILABLE",
                        value: root.monitor
                            ? Number(root.monitor.availableGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#9be3a5"
                    },
                    {
                        label: "RAM USAGE",
                        value: root.monitor
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A",
                        color: "#c5a6ff"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 76
                    Layout.minimumWidth: 0

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
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                }
            }
        }

        // ==================== RAM HISTORY ====================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 245

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
                        text: "RAM USAGE HISTORY"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: root.monitor
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A"
                        color: "#7de2d1"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }

                ResourceChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    samples: root.monitor
                        ? Array.from(root.monitor.history)
                        : []

                    lineColor: "#7de2d1"
                    emptyMessage: "Collecting RAM samples..."
                }
            }
        }

        // ==================== MEMORY BREAKDOWN ====================
        Text {
            text: "MEMORY BREAKDOWN"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "FREE MEMORY",
                        value: root.monitor
                            ? Number(root.monitor.freeGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#9be3a5"
                    },
                    {
                        label: "CACHED",
                        value: root.monitor
                            ? Number(root.monitor.cachedGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "BUFFERS",
                        value: root.monitor
                            ? Number(root.monitor.buffersGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#e7bd87"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 72
                    Layout.minimumWidth: 0

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
                            font.pixelSize: 17
                            font.bold: true
                        }
                    }
                }
            }
        }

        // ==================== SWAP OVERVIEW ====================
        Text {
            text: "SWAP MEMORY"
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
                model: [
                    {
                        label: "TOTAL SWAP",
                        value: root.monitor
                            ? Number(root.monitor.swapTotalGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#c5a6ff"
                    },
                    {
                        label: "USED SWAP",
                        value: root.monitor
                            ? Number(root.monitor.swapUsedGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#ff8f9c"
                    },
                    {
                        label: "FREE SWAP",
                        value: root.monitor
                            ? Number(root.monitor.swapFreeGB).toFixed(2) + " GB"
                            : "N/A",
                        color: "#9be3a5"
                    },
                    {
                        label: "SWAP USAGE",
                        value: root.monitor
                            ? Number(root.monitor.swapUsage).toFixed(1) + "%"
                            : "N/A",
                        color: "#e7bd87"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.preferredHeight: 76
                    Layout.minimumWidth: 0

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
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                }
            }
        }

        // ==================== SWAP HISTORY ====================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 220

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
                        text: "SWAP USAGE HISTORY"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: root.monitor
                            ? Number(root.monitor.swapUsage).toFixed(1) + "%"
                            : "N/A"
                        color: "#ff8f9c"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }

                ResourceChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    samples: root.monitor
                        ? Array.from(root.monitor.swapHistory)
                        : []

                    lineColor: "#ff8f9c"
                    emptyMessage: "Collecting swap samples..."
                }
            }
        }
    }
}