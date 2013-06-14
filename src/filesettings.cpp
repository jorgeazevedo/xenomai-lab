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

#include "filesettings.h"

FileSettings::FileSettings()
{

}

FileSettings::FileSettings(const QString& fileName)
{
        open(fileName);
}

FileSettings::~FileSettings()
{
    settings_delete(d_settings);
    //TODO:
    //This is absolutely ridiculous.
    //FileSettings should be constructed
    //with a parameter that specifies this behavior.
    if(d_fileName.contains("blocks.conf"))
        return;
    if(!QFile::remove(d_fileName)){
        QStringList err;
        err << Q_FUNC_INFO << "Could not erase "+d_fileName+" ! This is a sign of workspace corruption."
               "Does "+d_fileName+" exist? Do we have permissions?";
        throw(err);
    }
}

void FileSettings::open(const QString &fileName)
{
    FILE *f;

    f=fopen(fileName.toStdString().c_str(), "r");
    if (f==NULL){
        f=fopen(fileName.toStdString().c_str(), "w+");
        if (f==NULL){
            throw("Failed to open"+fileName+" for read/write!");
        }
    }

    d_settings = settings_open(f);
    fclose(f);
    if (d_settings==NULL)
        throw("Failed to read settings!");

    d_fileName=fileName;
}

void FileSettings::commit()
{
    FILE *f;

    f = fopen(d_fileName.toStdString().c_str(), "w");
    if (f == NULL) {
        throw("Failed to open "+d_fileName+" for write!");
    }

    if(!settings_save(d_settings, f)){
        throw("Did not save settings!");
    }

    fclose(f);

}

void FileSettings::iterOver(const QString& section, settings_section_enum_func func, const void *obj)
{
    settings_section_enum(d_settings,section.toStdString().c_str() , func, obj);
}

int FileSettings::getInt(const QString& section, const QString& key) const
{

    char* string=getSetting(section,key);
    int ret=atoi(string);
    delete string;
    return ret;

}

void FileSettings::storeInt(const QString& section, const QString& key, int value){
    char buf[100];
    strcpy(buf,QString::number(value).toStdString().c_str());

    settings_set(d_settings,section.toStdString().c_str(),key.toStdString().c_str(),buf);
}

double FileSettings::getDouble(const QString& section, const QString& key) const
{

    char* string=getSetting(section,key);
    double ret=atof(string);
    delete string;
    return ret;

}

void FileSettings::storeDouble(const QString& section, const QString& key, double value){
    char buf[100];
    strcpy(buf,QString::number(value).toStdString().c_str());

    settings_set(d_settings,section.toStdString().c_str(),key.toStdString().c_str(),buf);
}

QString FileSettings::getString(const QString &section, const QString &key) const
{

    char* string=getSetting(section,key);
    QString ret(string);
    delete string;
    return ret;

}

void FileSettings::storeString(const QString& section, const QString& key, const QString& value){
    char buf[100];
    strcpy(buf,value.toStdString().c_str());

    settings_set(d_settings,section.toStdString().c_str(),key.toStdString().c_str(),buf);
}

SuperBlock FileSettings::getBlock(const QString &section) const
{
    return SuperBlock(getString(section,"Type"),
                     getString(section,"Name"),
                     getDouble(section,"X"),
                     getDouble(section,"Y"));
}

void FileSettings::storeBlock(const QString &section, const SuperBlock &block)
{
    storeString(section,"Type",block.type());
    storeString(section,"Name",block.name());
    storeDouble(section,"X",block.X());
    storeDouble(section,"Y",block.Y());

}

SuperLine FileSettings::getLine(const QString &section) const
{
    return SuperLine(getString(section,"Origin"),
                     getString(section,"Destiny"),
                     getString(section,"Name"),
                     getString(section,"Value"));
}

void FileSettings::storeLine(const QString &section, const SuperLine &line)
{
    storeString(section,"Origin",line.origin());
    storeString(section,"Destiny",line.destiny());
    storeString(section,"Name",line.name());
    storeString(section,"Value",line.value());

}

bool FileSettings::isEmpty() const
{
    if(d_fileName==""){
        QStringList err;
        err << Q_FUNC_INFO << "No registered file for open!";
        throw(err);
   }

    QFile file(d_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QStringList err;
        err << Q_FUNC_INFO << "Failed to file" + d_fileName + " for read.";
        throw(err);
    }


    if(file.atEnd())
        return true;

    return false;
}

void FileSettings::setSetting(const QString &section, const QString &key)
{
    //settings_set(d_settings,section.toStdString().c_str(),key.toStdString().c_str(),buf,sizeof(buf));
}

char* FileSettings::getSetting(const QString &section, const QString &key) const
{
    char buf[100];
    char* ret= new char[100];
    if(settings_get(d_settings,section.toStdString().c_str(),
                         key.toStdString().c_str(),buf,sizeof(buf))){
        strcpy(ret,buf);
        return ret;
    }
    else
        throw(section+" does not have a "+key+" in "+d_fileName);

}
