import QtQuick 2.12

import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

import OpenPanorama 1.0

ApplicationWindow {
    id: mainWindow

    title: "OpenPanorama"

    width: 800
    height: 600

    visible: true

    header: ToolBar {
        RowLayout {
            ToolButton {
                text: qsTr("New project")

                onClicked: {
                    if(imageModel.rowCount()) {
                        resetDialog.open()
                    }
                }
            }
            ToolButton {
                text: qsTr("Add new image")

                onClicked: {
                    fileDialog.open()
                }
            }
        }
    }

    RowLayout {
        id: mainRow

        anchors.fill: parent

        spacing: 0

        Rectangle {
            id: mainPane

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "black"

            Flickable {
                anchors.fill: parent
                contentHeight: pane.implicitHeight
                contentWidth: pane.implicitWidth

                flickableDirection: Flickable.HorizontalAndVerticalFlick

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                }

                ScrollBar.horizontal: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                }

                interactive: false

                PanoramaPane {
                    id: pane

                    anchors.fill: parent
                    model: imageModel
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 30

            color: "#e0e0e0"

            Button {
                anchors.centerIn: parent

                width: parent.width
                height: 20

                text: "..."

                onClicked: mainRow.state = mainRow.state == "mainPaneOnly" ? "" : "mainPaneOnly"               
            }
        }

        Item {
            id: rightPane

            Layout.fillHeight: true
            Layout.preferredWidth: 150

            Behavior on Layout.preferredWidth  {
              NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
            }

            Rectangle {
                anchors.fill: parent
                color: "#e0e0e0"
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 10

                Text {
                    text: qsTr("method:")
                }

                ComboBox {
                    Layout.fillWidth: true
                    height: 30

                    model: ListModel {
                        ListElement { text: "Banana" }
                        ListElement { text: "Apple" }
                        ListElement { text: "Coconut"}
                    }            
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    Layout.fillWidth: true
                    height: 30

                    text: qsTr("Apply")
                }
            }       
        }

        states: [
            State {
                name: "mainPaneOnly"
                PropertyChanges { target: rightPane; Layout.preferredWidth: 0;}
            }
        ]
    }

    FileDialog {
        id: fileDialog
        title: "Please choose an image"
        folder: shortcuts.home
        onAccepted: {
            imageModel.AddImageFromFile(fileDialog.fileUrl)
        }
    }

    Dialog {
        id: resetDialog
        title: "Reset project"
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            imageModel.Reset()
        }
    }
}