#include "room.h"

#include <QDebug>

#include "exit.h"
#include "item.h"
#include "portal.h"


#define super GameObject

Room::Room(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Room, id, options),
    m_position(0, 0, 0),
    m_flags(RoomFlags::NoFlags) {

    setAutoDelete(false);
}

Room::~Room() {
}

void Room::setPosition(const Point3D &position) {

    if (m_position != position) {
        m_position = position;

        setModified();
    }
}

void Room::addPortal(const GameObjectPtr &portal) {

    if (!m_portals.contains(portal)) {
        m_portals << portal;

        m_exits.clear();
        setModified();
    }
}

void Room::removePortal(const GameObjectPtr &portal) {

    if (m_portals.removeOne(portal)) {
        m_exits.clear();
        setModified();
    }
}

void Room::setPortals(const GameObjectPtrList &portals) {

    if (m_portals != portals) {
        m_portals = portals;

        m_exits.clear();
        setModified();
    }
}

const GameObjectPtrList &Room::exits() {

    if (m_exits.isEmpty()) {
        for (const GameObjectPtr &portalPtr : m_portals) {
            Portal *portal = portalPtr.cast<Portal *>();
            if (portal->flags() & PortalFlags::CanPassThrough ||
                (portal->flags() & PortalFlags::CanPassThroughIfOpen &&
                 portal->canOpenFromRoom(const_cast<Room *>(this)))) {
                Exit *exit = new Exit(realm());
                if (this == portal->room().cast<Room *>()) {
                    exit->setName(portal->name());
                    exit->setDescription(portal->description());
                    exit->setDestination(portal->room2());
                    exit->setDoor(portal->flags() & PortalFlags::CanOpenFromSide1);
                    exit->setHidden(portal->flags() & PortalFlags::IsHiddenFromSide1);
                    exit->setTriggers(portal->triggers());
                } else {
                    exit->setName(portal->name2());
                    exit->setDescription(portal->description2());
                    exit->setDestination(portal->room());
                    exit->setDoor(portal->flags() & PortalFlags::CanOpenFromSide2);
                    exit->setHidden(portal->flags() & PortalFlags::IsHiddenFromSide2);
                    exit->setTriggers(portal->triggers());
                }
                m_exits.append(exit);
            }
        }
    }
    return m_exits;
}

void Room::addCharacter(const GameObjectPtr &character) {

    if (!m_characters.contains(character)) {
        m_characters << character;
    }
}

void Room::removeCharacter(const GameObjectPtr &character) {

    m_characters.removeOne(character);
}

void Room::setCharacters(const GameObjectPtrList &characters) {

    if (m_characters != characters) {
        m_characters = characters;
    }
}

void Room::addItem(const GameObjectPtr &item) {

    if (!m_items.contains(item)) {
        m_items << item;

        setModified();
    }
}

void Room::removeItem(const GameObjectPtr &item) {

    if (m_items.removeOne(item)) {
        setModified();
    }
}

void Room::setItems(const GameObjectPtrList &items) {

    if (m_items != items) {
        m_items = items;

        setModified();
    }
}

void Room::setFlags(RoomFlags flags) {

    if (m_flags != flags) {
        m_flags = flags;

        setModified();
    }
}

void Room::setEventMultipliers(const GameEventMultiplierMap &multipliers) {

    if (m_eventMultipliers != multipliers) {
        m_eventMultipliers = multipliers;

        setModified();
    }
}
