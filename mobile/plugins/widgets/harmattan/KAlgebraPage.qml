import com.nokia.meego 1.0

Page
{
    tools: ToolBarLayout {
        id: pageStackWindowTools
        visible: true
        ToolIcon { iconId: "toolbar-back"; onClicked: pageStack.pop(); }
    }
}