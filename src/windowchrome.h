#pragma once

#include <QColor>
#include <QObject>

class WindowChrome : public QObject
{
    Q_OBJECT

public:
    explicit WindowChrome(QObject *parent = nullptr);

    Q_INVOKABLE bool apply(QObject *windowObject,
                           bool dark,
                           const QColor &captionColor,
                           const QColor &textColor,
                           const QColor &borderColor);
};
