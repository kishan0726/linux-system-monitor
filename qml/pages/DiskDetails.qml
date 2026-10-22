
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
            text: "DISK OVERVIEW"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        // Capacity summary cards
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "TOTAL CAPACITY",
                        value: root.monitor
                            ? Number(root.monitor.totalGB).toFixed(2) + " GiB"
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "USED SPACE",
                        value: root.monitor
                            ? Number(root.monitor.usedGB).toFixed(2) + " GiB"
                            : "N/A",
                        color: "#7de2d1"
                    },
                    {
                        label: "FREE SPACE",
                        value: root.monitor
                            ? Number(root.monitor.freeGB).toFixed(2) + " GiB"
                            : "N/A",
                        color: "#9be3a5"
                    },
                    {
                        label: "DISK USAGE",
                        value: root.monitor
                            ? Number(root.monitor.usage).toFixed(1) + "%"
                            : "N/A",
                        color: "#c5a6ff"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
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
                            font.pixelSize: 16
                            font.bold: true
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        // Used versus available capacity
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

                Text {
                    text: "CAPACITY BREAKDOWN"
                    color: "#e6edf3"
                    font.pixelSize: 12
                    font.bold: true
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredWidth: root.monitor
                            ? Math.max(0.001, Number(root.monitor.usedGB))
                            : 1
                        Layout.preferredHeight: 18
                        radius: 4
                        color: "#7de2d1"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredWidth: root.monitor
                            ? Math.max(0.001, Number(root.monitor.freeGB))
                            : 1
                        Layout.preferredHeight: 18
                        radius: 4
                        color: "#283640"
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "● Used"
                        color: "#7de2d1"
                        font.pixelSize: 10
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "● Available"
                        color: "#9be3a5"
                        font.pixelSize: 10
                    }
                }
            }
        }

        // Filesystem usage history
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
                        text: "DISK USAGE HISTORY"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

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
                        ? Array.from(root.monitor.history).map(Number)
                        : []

                    lineColor: "#7de2d1"
                    emptyMessage: "Collecting disk samples..."
                }
            }
        }

        // Live I/O throughput history
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 250

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
                        text: "READ / WRITE THROUGHPUT"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "● Read"
                        color: "#9bb8ff"
                        font.pixelSize: 10
                    }

                    Text {
                        text: "● Write"
                        color: "#e7bd87"
                        font.pixelSize: 10
                    }
                }

                DiskActivityChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    readSamples: root.monitor
                        ? Array.from(root.monitor.readHistory)
                        : []

                    writeSamples: root.monitor
                        ? Array.from(root.monitor.writeHistory)
                        : []
                }
            }
        }

        // Current read/write rates
        Text {
            text: "CURRENT DISK ACTIVITY"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "READ SPEED",
                        color: "#9bb8ff",
                        value: root.monitor
                            ? Number(root.monitor.readMBps).toFixed(2) + " MiB/s"
                            : "N/A"
                    },
                    {
                        label: "WRITE SPEED",
                        color: "#e7bd87",
                        value: root.monitor
                            ? Number(root.monitor.writeMBps).toFixed(2) + " MiB/s"
                            : "N/A"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredHeight: 82

                    radius: 8
                    color: "#111820"
                    border.color: "#283640"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 7

                        Text {
                            text: modelData.label
                            color: "#83939f"
                            font.pixelSize: 10
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

        Item {
            Layout.preferredHeight: 4
        }
    }
}
