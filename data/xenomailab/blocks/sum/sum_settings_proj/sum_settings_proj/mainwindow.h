#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "blockbase.h"
#include "sum_settings.h"

class MainWindow : public BlockBase
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& execName, const QString& name,QWidget *parent = 0);
    virtual ~MainWindow();

private:
    void setSettings();
};

#endif // MAINWINDOW_H
