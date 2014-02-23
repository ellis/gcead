import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: page
    width: 320; height: 480
    color: "lightgray"

	Button {
		text: "Push me"
		anchors.centerIn: parent
	}

	ListModel {
		id: waveList
		ListElement {
			checked: false
			rec: 1
			name: "FID 1"
		}
		ListElement {
			checked: false
			rec: 1
			name: "EAD 1"
		}
		ListElement {
			checked: false
			rec: 1
			name: "DIG 1"
		}
	}

	Component {
		id: waveDelegate
			CheckBox {
				text: name
			}
	}

	ListView {
		y: 20
		width: 200
		x: 20
		height: 200
		model: waveList
		delegate: waveDelegate
	}
/*
    Text {
        id: helloText
        text: "Hello world!"
        y: 30
        anchors.horizontalCenter: page.horizontalCenter
        font.pointSize: 24; font.bold: true
    }
*/
}
