/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#pragma once

#include <QSettings>

class Config: public QSettings
{
    Q_OBJECT
public:

    static Config &instance()
    {
        static Config cfg;
        return cfg;
    }
private:
    Q_DISABLE_COPY(Config)

    explicit Config();
};
