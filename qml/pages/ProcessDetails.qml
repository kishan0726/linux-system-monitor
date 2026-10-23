
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var monitor
    property string searchText: ""
    property string sortBy: "pid"
    property bool descending: true
    property int selectedPid: -1
    property string selectedName: ""

    color: "#0B1118"

    function sortedProcesses() {
        if (!root.monitor)
            return []

        let result = Array.from(root.monitor.processes || [])
        const query = root.searchText.trim().toLowerCase()

        if (query.length > 0) {
            result = result.filter(function(p) {
                return String(p.name || "").toLowerCase().includes(query)
                    || String(p.pid || "").includes(query)
                    || String(p.uid || "").includes(query)
            })
        }

        result.sort(function(a, b) {
            let av
            let bv

            if (root.sortBy === "pid") {
                av = Number(a.pid || 0)
                bv = Number(b.pid || 0)
            } else if (root.sortBy === "name") {
                av = String(a.name || "").toLowerCase()
                bv = String(b.name || "").toLowerCase()
            } else if (root.sortBy === "memory") {
                av = Number(a.memoryMB || 0)
                bv = Number(b.memoryMB || 0)
            } else if (root.sortBy === "state") {
                av = String(a.state || "")
                bv = String(b.state || "")
            } else if (root.sortBy === "uid") {
                av = Number(a.uid || 0)
                bv = Number(b.uid || 0)
            } else {
                av = Number(a.cpu || 0)
                bv = Number(b.cpu || 0)
            }

            let comparison = 0

            if (typeof av === "string")
                comparison = av.localeCompare(bv)
            else
                comparison = av < bv ? -1 : av > bv ? 1 : 0

            return root.descending ? -comparison : comparison
        })

        return result
    }

    function requestSort(column) {
        if (root.sortBy === column)
            root.descending = !root.descending
        else {
            root.sortBy = column
            root.descending = true
        }

        processList.model = root.sortedProcesses()
    }

    function formatMemory(value) {
        return Number(value || 0).toFixed(1) + " MiB"
    }

    function formatCpu(value) {
        return Number(value || 0).toFixed(1) + "%"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 22
        spacing: 14

        // Page heading
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: "PROCESSES DETAILS"
                color: "#E6EDF3"
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }

            Text {
                text: "PROCESS MANAGER"
                color: "#A9BAC8"
                font.pixelSize: 15
                font.bold: true
                Layout.fillWidth: true
            }
        }

        // Summary cards
        RowLayout {
            Layout.fillWidth: true
            spacing: 14

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 76
                radius: 9
                color: "#111C26"
                border.color: "#263746"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 11
                    spacing: 5

                    Text {
                        text: "TOTAL PROCESSES"
                        color: "#9BAFBE"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    Text {
                        text: root.monitor ? root.monitor.processCount : 0
                        color: "#83B5FF"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 76
                radius: 9
                color: "#111C26"
                border.color: "#263746"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 11
                    spacing: 5

                    Text {
                        text: "VISIBLE RESULTS"
                        color: "#9BAFBE"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    Text {
                        text: processList.count
                        color: "#6EE7D1"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }
            }
        }

        // Search
        TextField {
            id: searchField

            Layout.fillWidth: true
            Layout.preferredHeight: 44

            placeholderText: "Search by process name, PID, or UID..."
            placeholderTextColor: "#8395A5"
            color: "#E6EDF3"
            font.pixelSize: 16
            selectByMouse: true
            leftPadding: 14
            rightPadding: 14

            background: Rectangle {
                radius: 8
                color: "#101922"
                border.width: searchField.activeFocus ? 2 : 1
                border.color: searchField.activeFocus
                              ? "#55D6C2" : "#304554"
            }

            onTextChanged: {
                root.searchText = text
                processList.model = root.sortedProcesses()
            }
        }

        // Sorting controls
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                text: "SORT BY"
                color: "#A9BAC8"
                font.pixelSize: 14
                font.bold: true
            }

            ComboBox {
                id: sortCombo

                Layout.preferredWidth: 175
                Layout.preferredHeight: 38

                model: [
                    { text: "PID", value: "pid" },
                    { text: "Process name", value: "name" },
                    { text: "CPU usage", value: "cpu" },
                    { text: "Memory usage", value: "memory" },
                    { text: "Process state", value: "state" },
                    { text: "User ID", value: "uid" }
                ]

                textRole: "text"
                font.pixelSize: 14

                background: Rectangle {
                    radius: 7
                    color: "#111C26"
                    border.color: "#304554"
                }

                contentItem: Text {
                    leftPadding: 10
                    rightPadding: 28
                    text: sortCombo.displayText
                    color: "#E6EDF3"
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                delegate: ItemDelegate {
                    width: sortCombo.width
                    height: 38
                    text: modelData.text
                    font.pixelSize: 14

                    background: Rectangle {
                        color: highlighted ? "#203746" : "#111C26"
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#E6EDF3"
                        font.pixelSize: 14
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: 8
                    }
                }

                onActivated: function(index) {
                    root.sortBy = model[index].value
                    processList.model = root.sortedProcesses()
                }
            }

            Button {
                Layout.preferredHeight: 38
                Layout.preferredWidth: 125

                text: root.descending ? "Descending ↓" : "Ascending ↑"
                font.pixelSize: 13

                background: Rectangle {
                    radius: 7
                    color: "#152630"
                    border.color: "#31505D"
                }

                contentItem: Text {
                    text: parent.text
                    color: "#6EE7D1"
                    font.pixelSize: 13
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    root.descending = !root.descending
                    processList.model = root.sortedProcesses()
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }

        // Table header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            radius: 7
            color: "#1A2834"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Text {
                    text: "PID"
                    Layout.preferredWidth: 82
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("pid")
                    }
                }

                Text {
                    text: "PROCESS"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 100
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("name")
                    }
                }

                Text {
                    text: "STATE"
                    Layout.preferredWidth: 54
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("state")
                    }
                }

                Text {
                    text: "CPU %"
                    Layout.preferredWidth: 72
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("cpu")
                    }
                }

                Text {
                    text: "MEMORY"
                    Layout.preferredWidth: 94
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("memory")
                    }
                }

                Text {
                    text: "UID"
                    Layout.preferredWidth: 54
                    color: "#B8C7D3"
                    font.pixelSize: 13
                    font.bold: true
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.requestSort("uid")
                    }
                }
            }
        }

        // Process list
        ListView {
            id: processList

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 220

            clip: true
            spacing: 5
            model: root.sortedProcesses()

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: Rectangle {
                id: processRow

                required property var modelData
                required property int index

                width: processList.width
                height: 46
                radius: 7

                color: rowMouse.containsMouse
                       ? "#203342"
                       : (index % 2 === 0 ? "#111C26" : "#14212C")

                border.color: "#263746"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 8

                    Text {
                        text: processRow.modelData.pid
                        Layout.preferredWidth: 82
                        color: "#91BAFF"
                        font.pixelSize: 14
                        font.family: "monospace"
                        elide: Text.ElideRight
                    }

                    Text {
                        text: processRow.modelData.name || "Unknown"
                        Layout.fillWidth: true
                        Layout.minimumWidth: 100
                        color: "#E6EDF3"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                    }

                    Text {
                        text: processRow.modelData.state || "-"
                        Layout.preferredWidth: 54
                        color: processRow.modelData.state === "R"
                               ? "#6EE7D1" : "#A9BAC8"
                        font.pixelSize: 14
                    }

                    Text {
                        text: root.formatCpu(processRow.modelData.cpu)
                        Layout.preferredWidth: 72
                        color: "#C6A6FF"
                        font.pixelSize: 14
                        font.family: "monospace"
                    }

                    Text {
                        text: root.formatMemory(processRow.modelData.memoryMB)
                        Layout.preferredWidth: 94
                        color: "#F6C177"
                        font.pixelSize: 14
                        font.family: "monospace"
                    }

                    Text {
                        text: processRow.modelData.uid !== undefined
                              ? processRow.modelData.uid : "-"
                        Layout.preferredWidth: 54
                        color: "#A9BAC8"
                        font.pixelSize: 14
                    }
                }

                MouseArea {
                    id: rowMouse
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    hoverEnabled: true

                    onClicked: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            root.selectedPid = Number(processRow.modelData.pid)
                            root.selectedName = String(
                                        processRow.modelData.name || "Unknown")
                            terminateDialog.open()
                        }
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: processList.count === 0
                text: "No processes found"
                color: "#A9BAC8"
                font.pixelSize: 17
            }
        }

        Text {
            Layout.fillWidth: true
            text: "Right-click a process to request termination. Access depends on your user permissions."
            color: "#91A4B3"
            font.pixelSize: 13
            wrapMode: Text.WordWrap
        }

        // Footer
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    text: "Total processes"
                    color: "#9BAFBE"
                    font.pixelSize: 13
                }

                Text {
                    text: "Shown in dashboard"
                    color: "#9BAFBE"
                    font.pixelSize: 13
                }
            }

            ColumnLayout {
                spacing: 4

                Text {
                    text: root.monitor ? root.monitor.processCount : 0
                    color: "#6EE7D1"
                    font.pixelSize: 14
                    font.bold: true
                }

                Text {
                    text: processList.count
                    color: "#6EE7D1"
                    font.pixelSize: 14
                    font.bold: true
                }
            }
        }
    }

    // Termination confirmation
    Dialog {
        id: terminateDialog

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 420
        modal: true

        title: "Terminate process?"
        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: {
            if (root.monitor && root.selectedPid > 1) {
                const ok = root.monitor.terminateProcess(root.selectedPid)
                resultText.text = ok
                    ? "Termination signal sent to PID " + root.selectedPid
                    : "Could not terminate PID " + root.selectedPid
                resultDialog.open()
            }
        }

        contentItem: Text {
            width: 380
            text: "Send a termination request to \"" + root.selectedName
                  + "\" (PID " + root.selectedPid + ")?\n\n"
                  + "Unsaved work in this process may be lost."
            color: "#E6EDF3"
            font.pixelSize: 15
            wrapMode: Text.WordWrap
        }

        background: Rectangle {
            color: "#14212C"
            radius: 10
            border.color: "#38505F"
        }
    }

    Dialog {
        id: resultDialog

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 400
        modal: true
        title: "Process manager"
        standardButtons: Dialog.Ok

        contentItem: Text {
            id: resultText
            text: ""
            color: "#E6EDF3"
            font.pixelSize: 15
            wrapMode: Text.WordWrap
        }

        background: Rectangle {
            color: "#14212C"
            radius: 10
            border.color: "#38505F"
        }
    }
}
