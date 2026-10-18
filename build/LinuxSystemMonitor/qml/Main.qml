
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: window

    visible: true
    width: 1440
    height: 900
    minimumWidth: 950
    minimumHeight: 680
    title: "Linux System Monitor"
    color: "#0b0f14"

    property color panel: "#111820"
    property color panelAlt: "#0e151d"
    property color border: "#283640"
    property color foreground: "#e6edf3"
    property color muted: "#83939f"
    property color cyan: "#7de2d1"
    property color blue: "#9bb8ff"
    property color purple: "#c5a6ff"
    property color amber: "#e7bd87"
    property color green: "#9be3a5"

    // Selected hardware section shown in the details dialog.
    property string selectedSection: ""

    function openDetails(section) {
        selectedSection = section
        detailsDialog.open()
    }

    function detailsForSection(section) {
        switch (section) {
        case "CPU":
            return [
                { label: "CPU usage", value: cpuMonitor.usage.toFixed(1) + "%" },
                { label: "Temperature", value: cpuMonitor.temperature > 0 ? cpuMonitor.temperature.toFixed(1) + " °C" : "N/A" },
                { label: "Frequency", value: cpuMonitor.frequency > 0 ? cpuMonitor.frequency.toFixed(0) + " MHz" : "N/A" },
                { label: "Logical cores", value: String(cpuMonitor.coreCount) }
            ]
        case "GPU":
            return [
                { label: "GPU", value: gpuMonitor.available ? gpuMonitor.name : "Unavailable" },
                { label: "Utilization", value: gpuMonitor.available ? gpuMonitor.usage.toFixed(1) + "%" : "N/A" },
                { label: "Temperature", value: gpuMonitor.available ? gpuMonitor.temperature.toFixed(1) + " °C" : "N/A" },
                { label: "VRAM used", value: gpuMonitor.available ? (gpuMonitor.usedVRAM / 1024).toFixed(2) + " GiB" : "N/A" },
                { label: "Total VRAM", value: gpuMonitor.available ? (gpuMonitor.totalVRAM / 1024).toFixed(2) + " GiB" : "N/A" }
            ]
        case "MEMORY":
            return [
                { label: "Usage", value: memoryMonitor.usage.toFixed(1) + "%" },
                { label: "Total RAM", value: memoryMonitor.totalGB.toFixed(2) + " GB" },
                { label: "Used RAM", value: memoryMonitor.usedGB.toFixed(2) + " GB" },
                { label: "Available RAM", value: memoryMonitor.availableGB.toFixed(2) + " GB" }
            ]
        case "DISK":
            return [
                { label: "Usage", value: diskMonitor.usage.toFixed(1) + "%" },
                { label: "Total capacity", value: diskMonitor.totalGB.toFixed(2) + " GB" },
                { label: "Used space", value: diskMonitor.usedGB.toFixed(2) + " GB" },
                { label: "Free space", value: diskMonitor.freeGB.toFixed(2) + " GB" }
            ]
        case "NETWORK":
            return [
                { label: "Interface", value: networkMonitor.interfaceName },
                { label: "Download speed", value: networkMonitor.available ? networkMonitor.downloadMBps.toFixed(2) + " MB/s" : "Unavailable" },
                { label: "Upload speed", value: networkMonitor.available ? networkMonitor.uploadMBps.toFixed(2) + " MB/s" : "Unavailable" }
            ]
        case "PROCESSES":
            return [
                { label: "Total processes", value: String(processMonitor.processCount) },
                { label: "Shown in dashboard", value: String(processMonitor.processes.length) }
            ]
        default:
            return []
        }
    }

    ScrollView {
        id: pageScroll
        anchors.fill: parent
        anchors.margins: 18
        clip: true
        contentWidth: availableWidth

        ColumnLayout {
            width: pageScroll.availableWidth
            spacing: 16

            // HEADER
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 76
                color: window.panel
                radius: 12
                border.color: window.border

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12


                    ColumnLayout {
                        spacing: 3

                        Text {
                            text: "LINUX SYSTEM MONITOR"
                            color: window.foreground
                            font.pixelSize: 19
                            font.bold: true
                        }

                        Text {
                            text: "PERFORMANCE OVERVIEW"
                            color: window.muted
                            font.pixelSize: 10
                            font.letterSpacing: 1.3
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ColumnLayout {
                        spacing: 4

                        Text {
                            text: "SYSTEM STATUS"
                            color: window.muted
                            font.pixelSize: 9
                            horizontalAlignment: Text.AlignRight
                            Layout.alignment: Qt.AlignRight
                        }

                        RowLayout {
                            spacing: 6
                            Layout.alignment: Qt.AlignRight

                            Rectangle {
                                width: 7
                                height: 7
                                radius: 4
                                color: window.cyan
                            }

                            Text {
                                text: "Active"
                                color: window.cyan
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }
                }
            }

            // SECTION TITLE
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "Resource overview"
                    color: window.foreground
                    font.pixelSize: 17
                    font.bold: true
                }

                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: "LIVE DATA: CPU / MEMORY"
                    color: window.muted
                    font.pixelSize: 9
                    font.letterSpacing: 1
                }
            }

            // SIX RESOURCE CARDS
            GridLayout {
                Layout.fillWidth: true
                columns: pageScroll.availableWidth < 650 ? 2 : 3
                columnSpacing: 12
                rowSpacing: 12

                MetricCard {
                    Layout.fillWidth: true
                    title: "CPU"
                    value: cpuMonitor.usage.toFixed(1) + "%"
                    detail: cpuMonitor.coreCount + " logical cores"
                    symbol: "CPU"
                    accent: window.cyan
                }

                MetricCard {
                    Layout.fillWidth: true
                    title: "MEMORY"
                    value: memoryMonitor.usedGB.toFixed(1) + " GB"
                    detail: memoryMonitor.usage.toFixed(1) + "% RAM in use"
                    symbol: "RAM"
                    accent: window.blue
                }
                
                MetricCard {
                    Layout.fillWidth: true
                    title: "GPU"
                    value: gpuMonitor.available
                           ? gpuMonitor.usage.toFixed(0) + "%"
                           : "-- %"
                    detail: gpuMonitor.available
                            ? gpuMonitor.name
                            : "GPU unavailable"
                    symbol: "GPU"
                    accent: window.purple
                }

                MetricCard {
                    Layout.fillWidth: true
                    title: "DISK"
                    value: diskMonitor.usage.toFixed(1) + "%"
                    detail: diskMonitor.usedGB.toFixed(1)
                            + " / "
                            + diskMonitor.totalGB.toFixed(1)
                            + " GB used"
                    symbol: "SSD"
                    accent: window.amber
                }

                MetricCard {
                    Layout.fillWidth: true
                    title: "NETWORK"
                    value: networkMonitor.available
                           ? networkMonitor.downloadMBps.toFixed(2) + " MB/s"
                           : "-- MB/s"
                    detail: networkMonitor.available
                            ? "↓ " + networkMonitor.downloadMBps.toFixed(2)
                              + " / ↑ " + networkMonitor.uploadMBps.toFixed(2)
                              + " MB/s"
                            : "Network unavailable"
                    symbol: "NET"
                    accent: window.green
                }

                MetricCard {
                    Layout.fillWidth: true
                    title: "PROCESSES"
                    value: String(processMonitor.processCount)
                    detail: "Active processes"
                    symbol: "PID"
                    accent: window.blue
                }
            }

            // CPU AND MEMORY CHARTS
            GridLayout {
                Layout.fillWidth: true
                columns: pageScroll.availableWidth < 700 ? 1 : 2
                columnSpacing: 12
                rowSpacing: 12

                ChartPanel {
                    Layout.fillWidth: true
                    title: "CPU utilization"
                    subtitle: "Processor activity over time"
                    accent: window.cyan
                    samples: cpuMonitor.history
                    emptyMessage: "Collecting CPU samples"
                }

                ChartPanel {
                    Layout.fillWidth: true
                    title: "Memory utilization"
                    subtitle: "RAM pressure over time"
                    accent: window.purple
                    samples: memoryMonitor.history
                    emptyMessage: "Collecting memory samples"
                }
            }

            // SYSTEM DETAILS AND PROCESS TABLE
            GridLayout {
                Layout.fillWidth: true
                columns: pageScroll.availableWidth < 850 ? 1 : 2
                columnSpacing: 12
                rowSpacing: 12

                // SYSTEM DETAILS
                Panel {
                    Layout.fillWidth: true
                    implicitHeight: 260

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text {
                            text: "SYSTEM DETAILS"
                            color: window.foreground
                            font.pixelSize: 14
                            font.bold: true
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: window.border
                        }

                        DetailRow {
                            label: "CPU temperature"
                            value: cpuMonitor.temperature > 0
                                   ? cpuMonitor.temperature.toFixed(1) + " °C"
                                   : "N/A"
                            accent: window.amber
                        }

                        DetailRow {
                            label: "CPU frequency"
                            value: cpuMonitor.frequency > 0
                                   ? cpuMonitor.frequency.toFixed(0) + " MHz"
                                   : "N/A"
                            accent: window.cyan
                        }

                        DetailRow {
                            label: "Total RAM"
                            value: memoryMonitor.totalGB.toFixed(1) + " GB"
                            accent: window.blue
                        }

                        DetailRow {
                            label: "Used RAM"
                            value: memoryMonitor.usedGB.toFixed(1) + " GB"
                            accent: window.purple
                        }

                        DetailRow {
                            label: "Available RAM"
                            value: memoryMonitor.availableGB.toFixed(1) + " GB"
                            accent: window.blue
                        }
  
                        DetailRow {
                            label: "GPU temperature"
                            value: gpuMonitor.available
                                   ? gpuMonitor.temperature.toFixed(1) + " °C"
                                   : "N/A"
                            accent: window.purple
                        }
              
                        DetailRow {
                            label: "GPU memory"
                            value: gpuMonitor.available
                                   ? (gpuMonitor.usedVRAM / 1024).toFixed(2) + " / "
                                     + (gpuMonitor.totalVRAM / 1024).toFixed(2) + " GiB"
                                   : "N/A"
                            accent: window.purple
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                // PROCESS TABLE
                Panel {
                    Layout.fillWidth: true
                    implicitHeight: 260

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true

                            Text {
                                text: "TOP PROCESSES (" + processMonitor.processCount + ")"
                                color: window.foreground
                                font.pixelSize: 14
                                font.bold: true
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            Text {
                                text: "CPU / RAM"
                                color: window.muted
                                font.pixelSize: 12
                            }
                        }

                        
                        Rectangle {
                            Layout.fillWidth: true
                            height: 32
                            radius: 6
                            color: "#19232d"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                spacing: 8

                                Text {
                                    Layout.fillWidth: true
                                    Layout.minimumWidth: 0
                                    text: "PROCESS"
                                    color: window.muted
                                    font.pixelSize: 12
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Text {
                                    Layout.preferredWidth: 60
                                    Layout.minimumWidth: 60
                                    text: "PID"
                                    color: window.muted
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Text {
                                    Layout.preferredWidth: 60
                                    Layout.minimumWidth: 60
                                    text: "CPU"
                                    color: window.muted
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Text {
                                    Layout.preferredWidth: 60
                                    Layout.minimumWidth: 60
                                    text: "RAM"
                                    color: window.muted
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 8

                            Repeater {
                                model: processMonitor.processes

                                delegate: RowLayout {
                                    required property var modelData
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Text {
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 0
                                        text: modelData.name
                                        color: window.foreground
                                        font.pixelSize: 13
                                        elide: Text.ElideRight
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    Text {
                                        Layout.preferredWidth: 60
                                        Layout.minimumWidth: 60
                                        text: String(modelData.pid)
                                        color: window.muted
                                        font.pixelSize: 12
                                        horizontalAlignment: Text.AlignRight
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    Text {
                                        Layout.preferredWidth: 60
                                        Layout.minimumWidth: 60
                                        text: Number(modelData.cpu).toFixed(1) + "%"
                                        color: window.cyan
                                        font.pixelSize: 12
                                        horizontalAlignment: Text.AlignRight
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    Text {
                                        Layout.preferredWidth: 60
                                        Layout.minimumWidth: 60
                                        text: Number(modelData.memoryMB).toFixed(1) + " MB"
                                        color: window.blue
                                        font.pixelSize: 12
                                        horizontalAlignment: Text.AlignRight
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.topMargin: 10
                Layout.bottomMargin: 10
                spacing: 8

                Item {
                    Layout.fillWidth: true
                    implicitHeight: shortcuts.implicitHeight

                    GridLayout {
                        id: shortcuts
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        columns: 6
                        rowSpacing: 8
                        columnSpacing: 20

                        Repeater {
                            model: [
                                { key: "Ctrl + C", label: "CPU" },
                                { key: "Ctrl + G", label: "GPU" },
                                { key: "Ctrl + M", label: "MEMORY" },
                                { key: "Ctrl + D", label: "DISK" },
                                { key: "Ctrl + N", label: "NETWORK" },
                                { key: "Ctrl + P", label: "PROCESS" }
                            ]

                            delegate: RowLayout {
                                required property var modelData
                                spacing: 6

                                Rectangle {
                                    Layout.preferredWidth: 66
                                    Layout.preferredHeight: 24
                                    radius: 4
                                    color: "#19232d"
                                    border.color: "#344451"

                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.key
                                        color: "#7dd3fc"
                                        font.pixelSize: 12
                                        font.bold: true
                                    }
                                }

                                Text {
                                    text: modelData.label
                                    color: "#8495a3"
                                    font.pixelSize: 12
                                    font.letterSpacing: 0.5
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Hardware keyboard shortcuts. These are window-level shortcuts.
    Shortcut {
        sequence: "Ctrl+C"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("CPU")
    }
    Shortcut {
        sequence: "Ctrl+G"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("GPU")
    }
    Shortcut {
        sequence: "Ctrl+M"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("MEMORY")
    }
    Shortcut {
        sequence: "Ctrl+D"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("DISK")
    }
    Shortcut {
        sequence: "Ctrl+N"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("NETWORK")
    }
    Shortcut {
        sequence: "Ctrl+P"
        context: Qt.WindowShortcut
        onActivated: window.openDetails("PROCESSES")
    }

    Dialog {
        id: detailsDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(window.width - 40, 480)
        modal: true
        title: window.selectedSection + " DETAILS"
        standardButtons: Dialog.Close

        background: Rectangle {
            color: window.panel
            radius: 12
            border.color: window.border
        }

        contentItem: ColumnLayout {
            spacing: 12
            implicitWidth: 400

            Repeater {
                model: window.detailsForSection(window.selectedSection)

                delegate: RowLayout {
                    required property var modelData
                    Layout.fillWidth: true
                    spacing: 16

                    Text {
                        Layout.fillWidth: true
                        text: modelData.label
                        color: window.muted
                        font.pixelSize: 13
                    }

                    Text {
                        text: modelData.value
                        color: window.cyan
                        font.pixelSize: 13
                        font.bold: true
                        horizontalAlignment: Text.AlignRight
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                visible: window.selectedSection === "PROCESSES"
                text: "Use the process table on the dashboard to inspect the current top processes."
                color: window.muted
                wrapMode: Text.WordWrap
                font.pixelSize: 11
            }
        }
    }

    // REUSABLE PANEL
    component Panel: Rectangle {
        color: window.panel
        radius: 12
        border.color: window.border
        border.width: 1
    }

    // REUSABLE RESOURCE CARD
    component MetricCard: Rectangle {
        property string title: ""
        property string value: "--"
        property string detail: ""
        property string symbol: ""
        property color accent: "#7de2d1"

        Layout.minimumWidth: 0
        implicitHeight: 124

        color: window.panel
        radius: 12
        border.color: window.border
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 7

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: title
                    color: window.muted
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1
                }

                Item {
                    Layout.fillWidth: true
                }

                Rectangle {
                    width: 30
                    height: 25
                    radius: 6
                    color: "#1a272e"

                    Text {
                        anchors.centerIn: parent
                        text: symbol
                        color: accent
                        font.pixelSize: 9
                        font.bold: true
                    }
                }
            }

            Text {
                text: value
                color: accent
                font.pixelSize: 25
                font.bold: true
            }

            Text {
                Layout.fillWidth: true
                text: detail
                color: window.muted
                font.pixelSize: 10
                elide: Text.ElideRight
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    // REUSABLE DETAIL ROW
    component DetailRow: RowLayout {
        property string label: ""
        property string value: ""
        property color accent: "#7de2d1"

        Layout.fillWidth: true
        spacing: 8

        Text {
            Layout.fillWidth: true
            text: label
            color: window.muted
            font.pixelSize: 11
        }

        Text {
            text: value
            color: accent
            font.pixelSize: 11
            font.bold: true
        }
    }

    // REUSABLE CHART PANEL
    component ChartPanel: Rectangle {
        property string title: ""
        property string subtitle: ""
        property color accent: "#7de2d1"
        property var samples: []
        property string emptyMessage: "Collecting samples"

        Layout.minimumWidth: 0
        implicitHeight: 220

        color: window.panel
        radius: 12
        border.color: window.border
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 8

            Text {
                text: title
                color: window.foreground
                font.pixelSize: 14
                font.bold: true
            }

            Text {
                text: subtitle
                color: window.muted
                font.pixelSize: 10
            }

            ResourceChart {
                Layout.fillWidth: true
                Layout.fillHeight: true

                lineColor: parent.parent.accent
                samples: parent.parent.samples
                emptyMessage: parent.parent.emptyMessage
            }
        }
    }
}
