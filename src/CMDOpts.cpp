/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "CMDOpts.h"

CMDOpts::CMDOpts(QApplication& app) :
    m_resetSettings(false),
    m_doNotSaveSettings(false)
{
    // Parsing the command line argument.
    QCommandLineParser parser;
    parser.setApplicationDescription(QApplication::translate(
        "cmd parser",
        "A little program to make list of wishing games."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("itemList",
        QCoreApplication::translate("cmd parser", "Loading an item list file (game, ...)"));
    
    QCommandLineOption resetSettings(
        "reset-settings",
        QCoreApplication::translate("cmd parser", "Reset to default settings"));
    parser.addOption(resetSettings);

    QCommandLineOption doNotSaveSettings(
        "do-not-save-settings",
        QCoreApplication::translate("cmd parser", "Do not save the settings"));
    parser.addOption(doNotSaveSettings);
    
    parser.process(app);

    if (parser.positionalArguments().size() > 0)
        m_itemListFile = parser.positionalArguments().first();
    m_resetSettings = parser.isSet(resetSettings);
    m_doNotSaveSettings = parser.isSet(doNotSaveSettings);
}

const QString& CMDOpts::itemListFile() const
{
    return m_itemListFile;
}

bool CMDOpts::resetSettings() const
{
    return m_resetSettings;
}

bool CMDOpts::doNotSaveSettings() const
{
    return m_doNotSaveSettings;
}