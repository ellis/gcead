import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Rectangle {
    id: page
    width: 320; height: 480
    color: "lightgray"

    ListModel {
        id: waveList
        ListElement {
            isChecked: true
            rec: 1
            name: "FID 1"
        }
        ListElement {
            isChecked: true
            rec: 1
            name: "EAD 1"
        }
        ListElement {
            isChecked: true
            rec: 1
            name: "DIG 1"
        }
        ListElement {
            isChecked: true
            rec: 2
            name: "FID 2"
        }
        ListElement {
            isChecked: true
            rec: 2
            name: "EAD 2"
        }
        ListElement {
            isChecked: true
            rec: 2
            name: "DIG 2"
        }
    }

    Component {
        id: waveDelegate
        CheckBox {
            checked: isChecked
            text: name
        }
    }

    ColumnLayout {
        id: columnLayout1
        anchors.fill: parent

        Text { text: "Please select which waves you wish to import:" }

        ScrollView {
            Layout.fillHeight: true
            frameVisible: true
            highlightOnFocus: true
            ListView {
                anchors.fill: parent
                model: waveList
                delegate: waveDelegate
                focus: true
            }
        }

        RowLayout {
            id: rowLayout1
            x: 29
            y: 285
            width: 249
            height: 100
            spacing: 2

            Item { Layout.fillWidth: true }
            Button {
                text: "Import"
            }
            Button {
                id: button1
                text: "Cancel"
            }
        }
    }
}
