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

#ifndef GAMESORTING_SETTINGS_H_
#define GAMESORTING_SETTINGS_H_

class Settings
{
    Settings(const Settings&) = delete;
    Settings();
public:
    ~Settings();

    inline bool isLegacyUtilEditor() const;
    void setLegacyUtilEditor(bool value);

    /*
    Only one instance of the class can be created.
    This method return an existing instance of Settings class.
    */
    static inline Settings& instance();

private:
    static Settings m_instance;

    bool m_isLegacyUtilEditor;
};

inline bool Settings::isLegacyUtilEditor() const
{
    return m_isLegacyUtilEditor;
}

inline Settings& Settings::instance()
{
    return m_instance;
}

#endif // GAMESORTING_SETTINGS_H_