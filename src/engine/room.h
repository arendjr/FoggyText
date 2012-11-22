#ifndef ROOM_H
#define ROOM_H

#include "gameeventmultipliermap.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "point3d.h"


class Room : public GameObject {

    Q_OBJECT

    public:
        Room(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Room();

        const Point3D &position() const { return m_position; }
        void setPosition(const Point3D &position);
        Q_PROPERTY(Point3D position READ position WRITE setPosition)

        const GameObjectPtrList &portals() const { return m_portals; }
        Q_INVOKABLE void addPortal(const GameObjectPtr &portal);
        Q_INVOKABLE void removePortal(const GameObjectPtr &portal);
        void setPortals(const GameObjectPtrList &portals);
        Q_PROPERTY(GameObjectPtrList portals READ portals WRITE setPortals)

        const GameObjectPtrList &exits();
        Q_PROPERTY(GameObjectPtrList exits READ exits STORED false)

        const GameObjectPtrList &characters() const { return m_characters; }
        Q_INVOKABLE void addCharacter(const GameObjectPtr &character);
        Q_INVOKABLE void removeCharacter(const GameObjectPtr &character);
        void setCharacters(const GameObjectPtrList &characters);
        Q_PROPERTY(GameObjectPtrList characters READ characters WRITE setCharacters STORED false)

        const GameObjectPtrList &items() const { return m_items; }
        Q_INVOKABLE void addItem(const GameObjectPtr &item);
        Q_INVOKABLE void removeItem(const GameObjectPtr &item);
        void setItems(const GameObjectPtrList &items);
        Q_PROPERTY(GameObjectPtrList items READ items WRITE setItems)

        const GameEventMultiplierMap &eventMultipliers() const { return m_eventMultipliers; }
        void setEventMultipliers(const GameEventMultiplierMap &multipliers);
        Q_PROPERTY(GameEventMultiplierMap eventMultipliers READ eventMultipliers
                                                           WRITE setEventMultipliers)

        GameObjectPtrList objects() { return exits() + characters() + m_items; }
        Q_PROPERTY(GameObjectPtrList objects READ objects STORED false)

    private:
        Point3D m_position;
        GameObjectPtrList m_portals;
        GameObjectPtrList m_exits;
        GameObjectPtrList m_characters;
        GameObjectPtrList m_items;
        GameEventMultiplierMap m_eventMultipliers;
};

#endif // ROOM_H
