#include "sectorgetcommand.h"

#include "gameobject.h"
#include "point3d.h"
#include "realm.h"
#include "room.h"
#include "util.h"


#define super ApiCommand

SectorGetCommand::SectorGetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-sector-get <request-id> <min-x> <min-y> <max-x> <max-y>");
}

SectorGetCommand::~SectorGetCommand() {
}

void SectorGetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    int minX = takeWord().toInt();
    int minY = takeWord().toInt();
    int maxX = takeWord().toInt();
    int maxY = takeWord().toInt();
    if (minX >= maxX || minY >= maxY) {
        sendError(400, "Invalid coordinates given");
        return;
    }

    QVariantMap rooms;
    QVariantMap portals;
    for (const GameObjectPtr &roomPtr : realm()->rooms()) {
        Room *room = roomPtr.unsafeCast<Room *>();
        Point3D position = room->position();
        if (position.x >= minX && position.x <= maxX && position.y >= minY && position.y <= maxY) {
            rooms[QString::number(room->id())] = room->toJsonString();
            for (const GameObjectPtr &portal : room->portals()) {
                QString portalId = QString::number(portal->id());
                if (!portals.contains(portalId)) {
                    portals[portalId] = portal->toJsonString();
                }
            }
        }
    }

    QVariantMap data;
    data["rooms"] = rooms;
    data["portals"] = portals;
    sendReply(data);
}
