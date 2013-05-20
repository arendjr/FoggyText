#ifndef PORTAL_H
#define PORTAL_H

#include "gameeventmultipliermap.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "metatyperegistry.h"
#include "point3d.h"


PT_DEFINE_FLAGS(PortalFlags,
    IsHiddenFromSide1,
    IsHiddenFromSide2,
    CanOpenFromSide1,
    CanOpenFromSide2,
    OmitFromDescriptionFromSide1,
    OmitFromDescriptionFromSide2,
    CanSeeThrough,
    CanHearThrough,
    CanShootThrough,
    CanPassThrough,
    CanSeeThroughIfOpen,
    CanHearThroughIfOpen,
    CanShootThroughIfOpen,
    CanPassThroughIfOpen,
    IsOpen
)


class Portal : public GameObject {

    Q_OBJECT

    public:
        Portal(Realm *realm, uint id = 0, Options options = NoOptions);
        virtual ~Portal();

        const QString &name2() const { return m_name2; }
        void setName2(const QString &name2);
        Q_PROPERTY(QString name2 READ name2 WRITE setName2)

        const QString &description2() const { return m_description2; }
        void setDescription2(const QString &description2);
        Q_PROPERTY(QString description2 READ description2 WRITE setDescription2)

        const QString &destination() const { return m_destination; }
        void setDestination(const QString &destination);
        Q_PROPERTY(QString destination READ destination WRITE setDestination)

        const QString &destination2() const { return m_destination2; }
        void setDestination2(const QString &destination2);
        Q_PROPERTY(QString destination2 READ destination2 WRITE setDestination2)

        const GameObjectPtr &room() const { return m_room; }
        void setRoom(const GameObjectPtr &room);
        Q_PROPERTY(GameObjectPtr room READ room WRITE setRoom)

        const GameObjectPtr &room2() const { return m_room2; }
        void setRoom2(const GameObjectPtr &room2);
        Q_PROPERTY(GameObjectPtr room2 READ room2 WRITE setRoom2)

        Point3D position() const;
        Q_PROPERTY(Point3D position READ position STORED false)

        PortalFlags flags() const { return m_flags; }
        void setFlags(PortalFlags flags);
        Q_PROPERTY(PortalFlags flags READ flags WRITE setFlags)

        const GameEventMultiplierMap &eventMultipliers() const { return m_eventMultipliers; }
        void setEventMultipliers(const GameEventMultiplierMap &multipliers);
        Q_PROPERTY(GameEventMultiplierMap eventMultipliers READ eventMultipliers
                                                           WRITE setEventMultipliers)

        Q_INVOKABLE double eventMultiplier(GameEventType eventType) const;

        Q_INVOKABLE GameObjectPtr oppositeOf(const GameObjectPtr &room) const;
        Q_INVOKABLE QString nameFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE QString descriptionFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE QString destinationFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE bool isHiddenFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE bool canOpenFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE bool omitFromDescriptionFromRoom(const GameObjectPtr &room) const;
        Q_INVOKABLE bool canOpen() const;
        Q_INVOKABLE bool canSeeThrough() const;
        Q_INVOKABLE bool canHearThrough() const;
        Q_INVOKABLE bool canShootThrough() const;
        Q_INVOKABLE bool canPassThrough() const;

        bool isOpen() const { return m_flags & PortalFlags::IsOpen; }
        void setOpen(bool open);
        Q_PROPERTY(bool open READ isOpen WRITE setOpen STORED false)

    private:
        QString m_name2;

        QString m_description2;

        QString m_destination;
        QString m_destination2;

        GameObjectPtr m_room;
        GameObjectPtr m_room2;

        PortalFlags m_flags;

        GameEventMultiplierMap m_eventMultipliers;
};

#endif // PORTAL_H
