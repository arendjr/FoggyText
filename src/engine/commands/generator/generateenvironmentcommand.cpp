#include "generateenvironmentcommand.h"

#include <cmath>

#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"


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

    //generateScraperFloor(10, Point3D(0, 0, 450));
    //generateScraperFloor(10, Point3D(500, 0, 450));
    //generateScraperFloor(10, Point3D(1000, 0, 450));
    //generateScraperFloor(10, Point3D(1500, 0, 450));
    //generateScraperFloor(10, Point3D(2000, 0, 450));
    //generateScraperFloor(10, Point3D(2500, 0, 450));
    //generateScraperFloor(10, Point3D(3000, 0, 450));

    //generateScraperFloor(10, Point3D(250, 400, 450));
    //generateScraperFloor(10, Point3D(750, 400, 450));
    //generateScraperFloor(10, Point3D(1250, 400, 450));
    //generateScraperFloor(10, Point3D(1750, 400, 450));
    //generateScraperFloor(10, Point3D(2250, 400, 450));
    //generateScraperFloor(10, Point3D(2750, 400, 450));
    //generateScraperFloor(10, Point3D(3250, 400, 450));

    //generateScraperFloor(10, Point3D(500, 800, 450));
    //generateScraperFloor(10, Point3D(1000, 800, 450));
    //generateScraperFloor(10, Point3D(1500, 800, 450));
    //generateScraperFloor(10, Point3D(2000, 800, 450));
    //generateScraperFloor(10, Point3D(2500, 800, 450));
    generateScraperFloor(10, Point3D(3000, 800, 450));
    generateScraperFloor(10, Point3D(3500, 800, 450));

    //generateScraperFloor(10, Point3D(2250, 1200, 450));
    //generateScraperFloor(10, Point3D(2750, 1200, 450));

    generateRoof(Point3D(-250, -250, 470), Point3D(3250, -250, 470),
                 Point3D(250, 1050, 470), Point3D(3750, 1050, 470));
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

QList<Room *> GenerateEnvironmentCommand::generateRoof(const Point3D &topLeft,
                                                       const Point3D &topRight,
                                                       const Point3D &bottomLeft,
                                                       const Point3D &bottomRight) {

    Q_UNUSED(bottomRight)

    QList<Room *> rooms;

    const int gridSize = 50;
    int numRows = (bottomLeft.y - topLeft.y) / gridSize + 1;
    int numColumns = (topRight.x - topLeft.x) / gridSize + 1;
    for (int i = 0; i < numRows; i++) {
        int y = topLeft.y + i * gridSize;
        int left = topLeft.x + i * (bottomLeft.x - topLeft.x) / numRows;
        for (int j = 0; j < numColumns; j++) {
            int x = left + j * gridSize;

            Room *room = createRoomAt(Point3D(x, y, topLeft.z));
            if (i == 0) {
                if (j == 0) {
                    room->setName("Northwest Corner of the Roof");
                } else if (j < numColumns - 1) {
                    room->setName("North Side of the Roof");
                } else {
                    room->setName("Northeast Corner of the Roof");
                }
            } else if (i < numRows - 1) {
                if (j == 0) {
                    room->setName("West Side of the Roof");
                } else if (j < numColumns - 1) {
                    room->setName("Roof");
                } else {
                    room->setName("East Side of the Roof");
                }
            } else {
                if (j == 0) {
                    room->setName("Southwest Corner of the Roof");
                } else if (j < numColumns - 1) {
                    room->setName("South Side of the Roof");
                } else {
                    room->setName("Southeast Corner of the Roof");
                }
            }

            if (i > 0) {
                int previousRow = i - 1;
                connectRooms(rooms[previousRow * numColumns + j], room);
                if (j > 0) {
                    connectRooms(rooms[previousRow * numColumns + j - 1], room);
                }
                if (j < numColumns - 1) {
                    connectRooms(rooms[previousRow * numColumns + j + 1], room);
                }
            }
            if (j > 0) {
                connectRooms(rooms[i * numColumns + j - 1], room);
            }
            rooms.append(room);
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

    Portal *portal = new Portal(realm());
    portal->setRoom(roomA);
    portal->setRoom2(roomB);
    portal->setName(Util::directionForVector(roomB->position() - roomA->position()));
    portal->setName2(Util::directionForVector(roomA->position() - roomB->position()));

    roomA->addPortal(portal);
    roomB->addPortal(portal);
}
