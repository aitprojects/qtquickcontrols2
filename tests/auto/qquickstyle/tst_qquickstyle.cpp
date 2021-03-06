/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/qtest.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuickControls2/qquickstyle.h>
#include <QtQuickControls2/private/qquickstyle_p.h>
#include <QtGui/private/qguiapplication_p.h>

#include "../shared/util.h"

class tst_QQuickStyle : public QQmlDataTest
{
    Q_OBJECT

private slots:
    void cleanup();
    void lookup();
    void configurationFile_data();
    void configurationFile();
    void commandLineArgument();
    void environmentVariables();
    void availableStyles();

private:
    void loadControls();
    void unloadControls();
};

void tst_QQuickStyle::cleanup()
{
    unloadControls();

    QGuiApplicationPrivate::styleOverride.clear();
    qunsetenv("QT_QUICK_CONTROLS_STYLE");
    qunsetenv("QT_QUICK_CONTROLS_FALLBACK_STYLE");
    qunsetenv("QT_QUICK_CONTROLS_CONF");
}

void tst_QQuickStyle::loadControls()
{
    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0; import QtQuick.Controls 2.1; Control { }", QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY2(!object.isNull(), qPrintable(component.errorString()));
}

void tst_QQuickStyle::unloadControls()
{
    qmlClearTypeRegistrations();
}

void tst_QQuickStyle::lookup()
{
    QVERIFY(QQuickStyle::name().isEmpty());
    QVERIFY(!QQuickStyle::path().isEmpty());

    QQuickStyle::setStyle("material");
    QCOMPARE(QQuickStyle::name(), QString("Material"));
    QVERIFY(!QQuickStyle::path().isEmpty());

    loadControls();

    QCOMPARE(QQuickStyle::name(), QString("Material"));
    QVERIFY(!QQuickStyle::path().isEmpty());
}

void tst_QQuickStyle::configurationFile_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("expectedStyle");
    QTest::addColumn<QString>("expectedPath");

    QTest::newRow("Default") << "default.conf" << "Default" << "";
    QTest::newRow("Fusion") << "fusion.conf" << "Fusion" << "";
    QTest::newRow("Imagine") << "imagine.conf" << "Imagine" << "";
    QTest::newRow("Material") << "material.conf" << "Material" << "";
    QTest::newRow("Universal") << "universal.conf" << "Universal" << "";
    QTest::newRow("Custom") << "custom.conf" << "Custom" << ":/";
}

void tst_QQuickStyle::configurationFile()
{
    QFETCH(QString, fileName);
    QFETCH(QString, expectedStyle);
    QFETCH(QString, expectedPath);

    qputenv("QT_QUICK_CONTROLS_CONF", testFile(fileName).toLocal8Bit());

    loadControls();

    QCOMPARE(QQuickStyle::name(), expectedStyle);
    if (!expectedPath.isEmpty())
        QCOMPARE(QQuickStyle::path(), expectedPath);
}

void tst_QQuickStyle::commandLineArgument()
{
    QGuiApplicationPrivate::styleOverride = "CmdLineArgStyle";

    loadControls();

    QCOMPARE(QQuickStyle::name(), QString("CmdLineArgStyle"));
}

void tst_QQuickStyle::environmentVariables()
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "EnvVarStyle");
    qputenv("QT_QUICK_CONTROLS_FALLBACK_STYLE", "EnvVarFallbackStyle");
    QCOMPARE(QQuickStyle::name(), QString("EnvVarStyle"));
    QCOMPARE(QQuickStylePrivate::fallbackStyle(), QString("EnvVarFallbackStyle"));
}

void tst_QQuickStyle::availableStyles()
{
    QString path = QFINDTESTDATA("data");
    QVERIFY(!path.isEmpty());
    qputenv("QT_QUICK_CONTROLS_STYLE_PATH", path.toLocal8Bit());

    QStringList paths = QQuickStylePrivate::stylePaths();
    QVERIFY(paths.contains(path));

    const QStringList styles = QQuickStyle::availableStyles();
    QVERIFY(!styles.isEmpty());
    QCOMPARE(styles.first(), QString("Default"));
    QVERIFY(!styles.contains("designer"));

    // QTBUG-60973
    for (const QString &style : styles) {
        QVERIFY2(!style.endsWith(".dSYM"), qPrintable(style));
    }
}

QTEST_MAIN(tst_QQuickStyle)

#include "tst_qquickstyle.moc"
