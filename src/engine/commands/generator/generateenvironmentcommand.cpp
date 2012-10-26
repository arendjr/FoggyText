#include "generateenvironmentcommand.h"

#include <cmath>

#include "realm.h"
#include "room.h"


#define PI 3.14159265


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

    generateScraperFloor(1, Point(0, 0, 450));
}

void GenerateEnvironmentCommand::generateScraperFloor(int level, const Point &center) {

    double radius = 200;
    for (int i = 0; i < 16; i++) {
        double angle = i * 360.0 / 16.0;
        int x = (int) (center.x + radius * sin(angle * PI / 180.0));
        int y = (int) (center.y + radius * cos(angle * PI / 180.0));
        createRoomAt(Point(x, y, center.z));
    }
}

Room *GenerateEnvironmentCommand::createRoomAt(const Point &position) {

    Room *room = GameObject::createByObjectType<Room *>(realm(), "room");
    room->setPosition(position);
    return room;
}
