/*
 * Xenomai Lab
 * Copyright (C) 2013  Jorge Azevedo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "superline.h"
#include "superblock.h"
#include "macros.h"
#include "settings.h"
#include <QStringList>
#include <QFile>

class FileSettings
{
public:
    FileSettings();
    FileSettings(const QString& fileName);
    ~FileSettings();

    void open(const QString& fileName);
    void commit();

    void iterOver(const QString&, settings_section_enum_func func, const void *obj=NULL);
    int getInt(const QString& section, const QString& key) const;
    void storeInt(const QString& section, const QString& key, int value);
    double getDouble(const QString& section, const QString& key) const;
    void storeDouble(const QString& section, const QString& key, double value);
    QString getString(const QString& section, const QString& key) const;
    void storeString(const QString& section, const QString& key, const QString& value);
    SuperBlock getBlock(const QString& section) const;
    void storeBlock(const QString& section, const SuperBlock& block);
    SuperLine getLine(const QString& section) const;
    void storeLine(const QString& section, const SuperLine& block);

    bool isEmpty() const;

private:
    void setSetting(const QString& section, const QString& key);
    char* getSetting(const QString& section, const QString& key) const;

    Settings* d_settings;
    QString d_fileName;
};

#endif // FILESETTINGS_H
