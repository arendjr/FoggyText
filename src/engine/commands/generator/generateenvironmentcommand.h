#ifndef GENERATECOMMAND_H
#define GENERATECOMMAND_H

#include "admincommand.h"


class Point3D;
class Room;
class Vector3D;


class GenerateEnvironmentCommand : public AdminCommand {

    Q_OBJECT

    public:
        GenerateEnvironmentCommand(QObject *parent = 0);
        virtual ~GenerateEnvironmentCommand();

        virtual void execute(Player *player, const QString &command);

    private:
        QList<Room *> generateScraperFloor(int level, const Point3D &center);

        Room *createRoomAt(const Point3D &position);

        void connectRooms(Room *roomA, Room *roomB);

        QString directionForVector(const Vector3D &vector);
};

#endif // GENERATECOMMAND_H
