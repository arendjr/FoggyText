#include "generateenvironmentcommand.h"

#include <cmath>

#include "exit.h"
#include "realm.h"
#include "room.h"


#define TAU 6.2831853071


#define super AdminCommand

GenerateEnvironmentCommand::GenerateEnvironmentCommand(QObject *parent) :
    super(parent) {

    setDescription("Generate new environment.\n"
                   "\n"
                   "Usage: generate-environment\n");
}

GenerateEnvironmentCommand::~GenerateEnvironmentCommand() {
}

void GenerateEnvironmentCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    generateScraperFloor(10, Point3D(0, 0, 450));
    generateScraperFloor(10, Point3D(500, 0, 450));
    generateScraperFloor(10, Point3D(1000, 0, 450));
    generateScraperFloor(10, Point3D(1500, 0, 450));
    generateScraperFloor(10, Point3D(2000, 0, 450));
    generateScraperFloor(10, Point3D(2500, 0, 450));
    generateScraperFloor(10, Point3D(3000, 0, 450));

    generateScraperFloor(10, Point3D(250, 400, 450));
    generateScraperFloor(10, Point3D(750, 400, 450));
    generateScraperFloor(10, Point3D(1250, 400, 450));
    generateScraperFloor(10, Point3D(1750, 400, 450));
    generateScraperFloor(10, Point3D(2250, 400, 450));
    generateScraperFloor(10, Point3D(2750, 400, 450));
    generateScraperFloor(10, Point3D(3250, 400, 450));

    generateScraperFloor(10, Point3D(500, 800, 450));
    generateScraperFloor(10, Point3D(1000, 800, 450));
    generateScraperFloor(10, Point3D(1500, 800, 450));
    generateScraperFloor(10, Point3D(2000, 800, 450));
    generateScraperFloor(10, Point3D(2500, 800, 450));
    generateScraperFloor(10, Point3D(3000, 800, 450));
    generateScraperFloor(10, Point3D(3500, 800, 450));

    generateScraperFloor(10, Point3D(2250, 1200, 450));
    generateScraperFloor(10, Point3D(2750, 1200, 450));
}

QList<Room *> GenerateEnvironmentCommand::generateScraperFloor(int level, const Point3D &center) {

    static const int roomNums[] = { 4, 4, 4, 8, 8, 12, 12, 16, 24, 32 };
    static const double radiuses[] = { 24.0, 28.0,  34.0,  42.0,  52.0,
                                       66.0, 86.0, 116.0, 156.0, 206.0 };

    int numRooms = roomNums[level - 1];
    double radius = radiuses[level - 1];

    QList<Room *> rooms;

    for (int i = 0; i < numRooms; i++) {
        double angle = i / (double) numRooms;
        int x = (int) (center.x + radius * sin(angle * TAU));
        int y = (int) (center.y + radius * cos(angle * TAU));
        Room *room = createRoomAt(Point3D(x, y, center.z));
        if (abs(x) > abs(y)) {
            if (x > 0) {
                room->setName("East Side of the Corridor");
            } else {
                room->setName("West Side of the Corridor");
            }
        } else {
            if (y > 0) {
                room->setName("South Side of the Corridor");
            } else {
                room->setName("North Side of the Corridor");
            }
        }
        rooms.append(room);

        if (i > 0) {
            connectRooms(rooms[i], rooms[i - 1]);
        }
    }

    connectRooms(rooms[numRooms - 1], rooms[0]);

    if (level > 1) {
        auto nextFloorRooms = generateScraperFloor(level - 1, center - Vector3D(0, 0, 8));

        for (int i = 0; i < 4; i++) {
            int index1 = i * rooms.length() / 4;
            int index2 = i * nextFloorRooms.length() / 4;
            connectRooms(rooms[index1], nextFloorRooms[index2]);
        }
    }

    return rooms;
}

Room *GenerateEnvironmentCommand::createRoomAt(const Point3D &position) {

    Room *room = new Room(realm());
    room->setPosition(position);
    return room;
}

void GenerateEnvironmentCommand::connectRooms(Room *roomA, Room *roomB) {

    Exit *aToB = new Exit(realm());
    Exit *bToA = new Exit(realm());

    roomA->addExit(aToB);
    roomB->addExit(bToA);

    aToB->setDestination(roomB);
    bToA->setDestination(roomA);

    aToB->setName(directionForVector(roomA->position() - roomB->position()));
    bToA->setName(directionForVector(roomB->position() - roomA->position()));

    aToB->setOppositeExit(bToA);
    bToA->setOppositeExit(aToB);
}

QString GenerateEnvironmentCommand::directionForVector(const Vector3D &vector) {

    if (10 * abs(vector.z) > sqrt(pow(vector.x, 2) + pow(vector.y, 2))) {
        if (vector.z > 0) {
            return "down";
        } else {
            return "up";
        }
    } else {
        double degrees = atan2(vector.y, vector.x) * 360.0 / TAU + 180.0;
        if (degrees < 22.5 || degrees > 337.5) {
            return "east";
        } else if (degrees >= 22.5 && degrees < 67.5) {
            return "southeast";
        } else if (degrees >= 67.5 && degrees < 112.5) {
            return "south";
        } else if (degrees >= 112.5 && degrees < 157.5) {
            return "southwest";
        } else if (degrees >= 157.5 && degrees < 202.5) {
            return "west";
        } else if (degrees >= 202.5 && degrees < 247.5) {
            return "northwest";
        } else if (degrees >= 247.5 && degrees < 292.5) {
            return "north";
        } else {
            return "northeast";
        }
    }
}
