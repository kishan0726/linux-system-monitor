
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

ScrollView {
    id: root

    property var monitor

    clip: true
    contentWidth: availableWidth

    function formatBytes(bytes) {
        let value = Number(bytes)
        if (!Number.isFinite(value) || value < 0)
            value = 0

        if (value < 1024)
            return value.toFixed(0) + " B"

        value /= 1024
        if (value < 1024)
            return value.toFixed(2) + " KiB"

        value /= 1024
        if (value < 1024)
            return value.toFixed(2) + " MiB"

        value /= 1024
        return value.toFixed(2) + " GiB"
    }

    ColumnLayout {
        width: root.availableWidth
        spacing: 14

        Text {
            text: "NETWORK OVERVIEW"
            color: "#e6edf3"
            font.pixelSize: 12
            font.bold: true
        }

        // Interface and connection status
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
                    width: 38
                    height: 38
                    radius: 8
                    color: "#1a272e"

                    Text {
                        anchors.centerIn: parent
                        text: "NET"
                        color: "#9be3a5"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        text: root.monitor
                            ? root.monitor.interfaceName
                            : "Unknown interface"
                        color: "#e6edf3"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Text {
                        text: root.monitor && root.monitor.available
                            ? "Default network interface"
                            : "No active interface detected"
                        color: "#83939f"
                        font.pixelSize: 10
                    }
                }

                Rectangle {
                    radius: 10
                    color: root.monitor && root.monitor.available
                        ? "#17352f" : "#3a2428"
                    implicitWidth: statusText.implicitWidth + 18
                    implicitHeight: 28

                    Text {
                        id: statusText
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

        // Current transfer speeds
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 10
            rowSpacing: 10

            Repeater {
                model: [
                    {
                        label: "DOWNLOAD SPEED",
                        value: root.monitor
                            ? Number(root.monitor.downloadMBps).toFixed(2)
                                + " MiB/s"
                            : "N/A",
                        color: "#9bb8ff",
                        symbol: "↓"
                    },
                    {
                        label: "UPLOAD SPEED",
                        value: root.monitor
                            ? Number(root.monitor.uploadMBps).toFixed(2)
                                + " MiB/s"
                            : "N/A",
                        color: "#e7bd87",
                        symbol: "↑"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData

                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredHeight: 88

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
                            text: modelData.symbol + "  " + modelData.value
                            color: modelData.color
                            font.pixelSize: 19
                            font.bold: true
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        // Download/upload history
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

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "NETWORK THROUGHPUT"
                        color: "#e6edf3"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "● Download"
                        color: "#9bb8ff"
                        font.pixelSize: 10
                    }

                    Text {
                        text: "● Upload"
                        color: "#e7bd87"
                        font.pixelSize: 10
                    }
                }

                DiskActivityChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    readColor: "#9bb8ff"
                    writeColor: "#e7bd87"

                    readSamples: root.monitor
                        ? Array.from(root.monitor.downloadHistory)
                        : []

                    writeSamples: root.monitor
                        ? Array.from(root.monitor.uploadHistory)
                        : []
                }
            }
        }

        // Total traffic counters
        Text {
            text: "TOTAL TRAFFIC"
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
                        label: "TOTAL RECEIVED",
                        value: root.monitor
                            ? root.formatBytes(root.monitor.receivedBytes)
                            : "N/A",
                        color: "#9bb8ff"
                    },
                    {
                        label: "TOTAL SENT",
                        value: root.monitor
                            ? root.formatBytes(root.monitor.sentBytes)
                            : "N/A",
                        color: "#e7bd87"
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
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        Text {
            Layout.fillWidth: true
            text: "Traffic totals are read from Linux interface counters and may reset when the interface or system restarts."
            color: "#83939f"
            font.pixelSize: 10
            wrapMode: Text.WordWrap
        }

        Item {
            Layout.preferredHeight: 4
        }
    }
}
