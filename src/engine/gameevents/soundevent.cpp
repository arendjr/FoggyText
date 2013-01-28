#include "soundevent.h"

#include "character.h"
#include "portal.h"
#include "room.h"


#define super GameEvent

SoundEvent::SoundEvent(Room *origin, double strength) :
    SoundEvent(GameEventType::Sound, origin, strength) {
}

SoundEvent::SoundEvent(GameEventType eventType, Room *origin, double strength) :
    super(eventType, origin, strength) {
}

SoundEvent::~SoundEvent() {
}

void SoundEvent::visitRoom(Room *room, double strength) {

    strength *= room->eventMultipliers()[GameEventType::Sound];

    if (strength >= 0.1) {
        for (const GameObjectPtr &characterPtr : room->characters()) {
            if (excludedCharacters().contains(characterPtr)) {
                continue;
            }

            Character *character = characterPtr.cast<Character *>();
            QString message = descriptionForStrengthAndCharacterInRoom(strength, character, room);
            if (character->isPlayer()) {
                character->send(message);
            } else {
                character->invokeTrigger("onsound", message);
            }

            addAffectedCharacter(characterPtr);
        }

        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            Room *room1 = portal->room().cast<Room *>();
            Room *room2 = portal->room2().cast<Room *>();
            Room *oppositeRoom = (room == room1 ? room2 : room1);
            if (hasBeenVisited(oppositeRoom)) {
                continue;
            }

            double multiplier;
            if (portal->isOpen()) {
                if (!portal->canHearThroughIfOpen()) {
                    continue;
                }
                multiplier = 1.0;
            } else {
                if (!portal->canHearThrough()) {
                    continue;
                }
                multiplier = portal->eventMultipliers()[GameEventType::Sound];
            }

            Vector3D vector = room2->position() - room1->position();
            multiplier *= qMax(1.0 - 0.05 * vector.length(), 0.0);

            double propagatedStrength = strength * multiplier;
            if (propagatedStrength >= 0.1) {
                addVisit(oppositeRoom, propagatedStrength);
            }
        }
    }
}
