import QtQuick
import QtQuick.Window
import CommStudio 1.0

Item {
    id: root
    property string kind: "grid"
    property color color: Theme.textSecondary
    implicitWidth: 20; implicitHeight: 20
    readonly property var paths: ({
        wave: '<path d="M2 13h4l3-8 4 15 3-10 2 3h4"/>',
        sidebar: '<rect x="3" y="4" width="18" height="16" rx="2"/><path d="M9 4v16m5-11 3 3-3 3"/>',
        port: '<path d="M3 4h18v6H3zM6 13h12M8 16h8M11 19h2v3h-2zM7 7h.1M11 7h.1M15 7h.1"/>',
        grid: '<rect x="3" y="3" width="7" height="7" rx="1"/><rect x="14" y="3" width="7" height="7" rx="1"/><rect x="3" y="14" width="7" height="7" rx="1"/><rect x="14" y="14" width="7" height="7" rx="1"/>',
        bluetooth: '<path d="m7 7 10 10-5 4V3l5 4L7 17"/>',
        network: '<rect x="9" y="3" width="6" height="5" rx="1"/><path d="M12 8v5M5 16v-3h14v3"/><rect x="2" y="16" width="6" height="5" rx="1"/><rect x="16" y="16" width="6" height="5" rx="1"/>',
        tools: '<path d="m14 6 4 4 3-3c1 5-3 8-7 6L6 21l-3-3 8-8c-2-4 1-8 6-7z"/>',
        log: '<rect x="5" y="3" width="14" height="18" rx="2"/><path d="M8 8h8M8 12h8M8 16h5"/>',
        gear: '<path d="m9 3-1 3-3 1 1 3-2 2 2 2-1 3 3 1 1 3h6l1-3 3-1-1-3 2-2-2-2 1-3-3-1-1-3z"/><circle cx="12" cy="12" r="3"/>',
        nodes: '<circle cx="5" cy="12" r="3"/><circle cx="18" cy="5" r="3"/><circle cx="18" cy="19" r="3"/><path d="m8 10 7-4M8 14l7 4"/>',
        plus: '<path d="M12 4v16M4 12h16"/>',
        more: '<circle cx="5" cy="12" r=".7"/><circle cx="12" cy="12" r=".7"/><circle cx="19" cy="12" r=".7"/>',
        close: '<path d="m6 6 12 12M18 6 6 18"/>',
        play: '<path d="m7 4 14 8-14 8z"/>',
        pause: '<path d="M8 5v14M16 5v14"/>',
        expand: '<path d="m9 5 7 7-7 7"/>',
        collapse: '<path d="m15 5-7 7 7 7"/>',
        down: '<path d="m5 9 7 7 7-7"/>',
        link: '<path d="m10 7 2-2a5 5 0 0 1 7 7l-2 2M14 17l-2 2a5 5 0 0 1-7-7l2-2M8 16l8-8"/>',
        unlink: '<path d="m10 7 2-2a5 5 0 0 1 7 7M14 17l-2 2a5 5 0 0 1-7-7M3 3l18 18"/>',
        search: '<circle cx="10" cy="10" r="6.5"/><path d="m15 15 6 6"/>',
        inspect: '<path d="M13 21H4V3h16v8M7 7h9M7 11h4"/><circle cx="17" cy="16" r="4"/><path d="m20 19 3 3"/>',
        copy: '<rect x="7" y="7" width="14" height="14" rx="1"/><path d="M17 3H3v14M11 11h6M11 15h6"/>',
        import: '<path d="M20 15v6H4V5h6M17 2v12m-4-4 4 4 4-4"/>',
        export: '<path d="M20 15v6H4V5h6M17 14V2m-4 4 4-4 4 4"/>',
        rows: '<path d="M4 5h10M4 12h16M4 19h10"/>',
        columns: '<path d="M5 4v16M12 4v16M19 4v16"/>',
        edit: '<path d="m4 15 12-12 5 5L9 20l-6 1zM13 6l5 5M14 20h7"/>',
        clear: '<path d="M5 7h14M9 3h6l1 4M7 7l1 14h8l1-14M10 11v6m4-6v6"/>',
        save: '<path d="M4 3h13l4 4v14H3V3zM7 3v6h10V3M7 21v-8h10v8"/>',
        maximize: '<path d="M9 3H3v6M15 3h6v6M3 15v6h6m6 0h6v-6"/>',
        sun: '<circle cx="12" cy="12" r="4"/><path d="M12 2v2m0 16v2M2 12h2m16 0h2M5 5l1 1m12 12 1 1M5 19l1-1M18 6l1-1"/>'
    })
    Image {
        anchors.fill: parent
        sourceSize.width: Math.ceil(width * Screen.devicePixelRatio)
        sourceSize.height: Math.ceil(height * Screen.devicePixelRatio)
        fillMode: Image.PreserveAspectFit
        opacity: root.color.a
        source: "data:image/svg+xml;utf8," + encodeURIComponent('<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="' + Qt.rgba(root.color.r, root.color.g, root.color.b, 1) + '" stroke-width="1.65" stroke-linecap="round" stroke-linejoin="round">' + (root.paths[root.kind] || root.paths.grid) + '</svg>')
    }
}
