#ifndef SCREENCONFIG_H
#define SCREENCONFIG_H

#include <QDBusArgument>
#include <QString>

struct ScreenConfig
{
    QString screenId;
    QString screenModeId;
    int screenPosX;
    int screenPosY;
    bool primary;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const ScreenConfig &screenStruct)
    {
        argument.beginStructure();
        argument << screenStruct.screenId  << screenStruct.screenModeId << screenStruct.screenPosX
                 << screenStruct.screenPosY << screenStruct.primary;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ScreenConfig &screenStruct)
    {
        argument.beginStructure();
        argument >> screenStruct.screenId >> screenStruct.screenModeId
                 >> screenStruct.screenPosX >> screenStruct.screenPosY >> screenStruct.primary;
        argument.endStructure();
        return argument;
    }
};
Q_DECLARE_METATYPE(ScreenConfig)

#endif // SCREENCONFIG_H
