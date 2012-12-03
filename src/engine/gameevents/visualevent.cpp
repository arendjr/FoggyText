#include "visualevent.h"

#include "portal.h"
#include "room.h"
#include "vector3d.h"


#define super GameEvent

VisualEvent::VisualEvent(Room *origin, double strength) :
    super(GameEventType::VisualEvent, origin, strength) {
}

VisualEvent::~VisualEvent() {
}

void VisualEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultipliers()[GameEventType::VisualEvent];

    if (strength >= 0.1) {
        QString message = descriptionForStrengthInRoom(strength, room);

        for (const GameObjectPtr &character : room->characters()) {
            if (excludedCharacters().contains(character)) {
                continue;
            }

            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onvisual", message);
            }
            addAffectedCharacter(character);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.unsafeCast<Portal *>();
            Room *room1 = portal->room().unsafeCast<Room *>();
            Room *room2 = portal->room2().unsafeCast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom)) {
                continue;
            }

            if (!isWithinSight(oppositeRoom, oppositeRoom)) {
                continue;
            }

            double multiplier;
            if (portal->isOpen()) {
                if (!portal->canSeeThroughIfOpen()) {
                    continue;
                }
                multiplier = 1.0;
            } else {
                if (!portal->canSeeThrough()) {
                    continue;
                }
                multiplier = portal->eventMultipliers()[GameEventType::VisualEvent];
            }

            Vector3D vector = oppositeRoom->position() - room->position();
            multiplier *= qMax(1.0 - 0.0005 * vector.length(), 0.0);

            double propagatedStrength = strength * multiplier;
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}

bool VisualEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

    if (sourceRoom == originRoom()) {
        return true;
    }

    Vector3D vector = targetRoom->position() - sourceRoom->position();

    Vector3D sourceVector = (targetRoom->position() - originRoom()->position()).normalized();
    Vector3D targetVector = vector.normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (targetRoom->flags() & RoomFlags::NoCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (targetRoom->flags() & RoomFlags::NoFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    return false;
}
