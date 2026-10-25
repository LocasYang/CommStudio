pragma Singleton

import QtQuick
import CommStudio.Backend 1.0

QtObject {
    id: root

    readonly property bool chinese: Studio.language === "zh-CN"

    // Use an English source phrase and a Chinese phrase at each UI call site.
    // Reading Studio.language here makes every bound text property update as
    // soon as the user changes the setting.
    function t(english, chineseText) {
        return chinese ? chineseText : english
    }

    function count(value, englishUnit, chineseUnit) {
        return chinese ? value + chineseUnit : value + " " + englishUnit
    }
}
