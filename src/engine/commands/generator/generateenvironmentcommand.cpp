#include "generateenvironmentcommand.h"

#include <cmath>

#include "area.h"
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

void GenerateEnvironmentCommand::execute(Character *character, const QString &command) {

    super::prepareExecute(character, command);

    generateScraperFloor(21, 10, Point3D(0, 0, 450));

    send("Environment generated");
}

QList<Room *> GenerateEnvironmentCommand::generateScraperFloor(int district, int level,
                                                               const Point3D &center) {

    static const int roomNums[] = { 8, 8, 12, 12, 16, 20, 28, 36, 48, 64 };
    static const double radiuses[] = { 24.0, 28.0,  34.0,  42.0,  52.0,
                                       66.0, 86.0, 116.0, 156.0, 206.0 };

    int numRooms = roomNums[level - 1];
    double radius = radiuses[level - 1];

    QString areaName = "District " + QString::number(district);
    Area *area = nullptr;
    for (const GameObjectPtr &existingArea : realm()->areas()) {
        if (existingArea->name() == areaName) {
            area = existingArea.cast<Area *>();
            break;
        }
    }
    if (area == nullptr) {
        area = new Area(realm());
        area->setName(areaName);
    }

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
        area->addRoom(room);

        if (i > 0) {
            connectRooms(rooms[i], rooms[i - 1]);
        }
    }

    connectRooms(rooms[numRooms - 1], rooms[0]);

    if (level > 1) {
        auto nextFloorRooms = generateScraperFloor(district, level - 1, center - Vector3D(0, 0, 8));

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

    Portal *portal = new Portal(realm());
    portal->setRoom(roomA);
    portal->setRoom2(roomB);
    portal->setName(Util::directionForVector(roomB->position() - roomA->position()));
    portal->setName2(Util::directionForVector(roomA->position() - roomB->position()));
    portal->setFlags(PortalFlags::CanSeeThrough | PortalFlags::CanHearThrough |
                     PortalFlags::CanShootThrough | PortalFlags::CanPassThrough);

    roomA->addPortal(portal);
    roomB->addPortal(portal);
}
