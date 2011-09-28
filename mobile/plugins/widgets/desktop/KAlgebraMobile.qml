import QtQuick 1.0
import QtDesktop 0.1
import org.kde.analitza 1.0

Rectangle
{
	SystemPalette { id:syspal }
	color: syspal.window
	height: 400
	width: 300
	
	ToolBar{
		id: toolbar
		width: parent.width
		height: 40
		
		Row {
			spacing: 2
			anchors.verticalCenter: parent.verticalCenter
			
			ToolButton{
// 				iconSource: "images/folder_new.png"
				anchors.verticalCenter: parent.verticalCenter
				
				text: "Go!"
				
				onClicked: {
					var idx = pluginsView.currentIndex
					var toOpen = plugins.pluginPath(idx)
					
					try {
						var component = Qt.createComponent(toOpen)
						
						if (component.status == Component.Ready)
							frame.addTab(component)
						else
							console.log("Error loading component:", component.errorString());
					} catch (e) {
						console.log("error: "+e)
					}
					
				}
			}
		}
	}
	
	TabFrame
	{
		id: frame
		width: parent.width
		anchors.top: toolbar.bottom
		anchors.bottom: parent.bottom
		
		Tab {
			title: "hola"
			TableView {
				id: pluginsView
				anchors.fill: parent
				headerVisible: false
				
				TableColumn {
					role: "display"
					caption: "Title"
					width: 120
				}
				
		// 		itemDelegate: Row { /*Image { source: 'image://desktoptheme/'+decoration-name } */Text { text: display } }
		// 		highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
				
				model: PluginsModel { id: plugins }
			}
		}
	}
}
