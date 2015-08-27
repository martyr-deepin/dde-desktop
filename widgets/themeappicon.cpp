#include "themeappicon.h"
#include <QFile>
#include <QPainter>
#include <QSvgRenderer>
#include <QPixmap>

#undef signals
extern "C" {
  #include <gtk/gtk.h>
}
#define signals public

ThemeAppIcon::ThemeAppIcon(QObject *parent) : QObject(parent)
{

}

void ThemeAppIcon::gtkInit(){
    gtk_init(NULL, NULL);
}

QPixmap ThemeAppIcon::getIconPixmap(QString iconPath, int width, int height){
    QPixmap pixmap(width, height);
    // iconPath is an absolute path of the system.
    if (QFile::exists(iconPath)) {
        pixmap = QPixmap(iconPath);
    } else if (iconPath.startsWith("data:image/")){
        // iconPath is a string representing an inline image.
        QStringList strs = iconPath.split("base64,");
        if (strs.length() == 2) {
            QByteArray data = QByteArray::fromBase64(strs.at(1).toLatin1());
            pixmap.loadFromData(data);
        }
    } else {
        // try to read the iconPath as a icon name.
        QString path = getThemeIconPath(iconPath);
        if (path.endsWith("svg")) {
            QSvgRenderer renderer(path);
            pixmap.fill(Qt::transparent);

            QPainter painter;
            painter.begin(&pixmap);

            renderer.render(&painter);

            painter.end();
        } else {
            pixmap.load(path);
        }
    }

    return pixmap;
}

QString ThemeAppIcon::getThemeIconPath(QString iconName)
{
    QByteArray bytes = iconName.toUtf8();
    const char *name = bytes.constData();

    GtkIconTheme* theme = gtk_icon_theme_get_default();

    GtkIconInfo* info = gtk_icon_theme_lookup_icon(theme, name, 48, GTK_ICON_LOOKUP_GENERIC_FALLBACK);

    if (info) {
        char* path = g_strdup(gtk_icon_info_get_filename(info));
#if GTK_MAJOR_VERSION >= 3
        g_object_unref(info);
#elif GTK_MAJOR_VERSION == 2
        gtk_icon_info_free(info);
#endif
        return QString(path);
    } else {
        return "";
    }
}

ThemeAppIcon::~ThemeAppIcon()
{

}

