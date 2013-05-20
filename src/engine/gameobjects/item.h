#ifndef ITEM_H
#define ITEM_H

#include "gameobject.h"
#include "gameobjectptr.h"
#include "point3d.h"


PT_DEFINE_FLAGS(ItemFlags,
    AttachedToCeiling,
    AttachedToWall,
    Hidden,
    Portable,
    AlwaysUseDefiniteArticle,
    ImpliedPlural
)


class Item : public GameObject {

    Q_OBJECT

    public:
        Item(Realm *realm, uint id = 0, Options options = NoOptions);
        Item(Realm *realm, GameObjectType objectType, uint id, Options options = NoOptions);
        virtual ~Item();

        const Point3D &position() const { return m_position; }
        void setPosition(const Point3D &position);
        Q_PROPERTY(Point3D position READ position WRITE setPosition)

        const QString &presenceVerb() const { return m_presenceVerb; }
        void setPresenceVerb(const QString &presenceVerb);
        Q_PROPERTY(QString presenceVerb READ presenceVerb WRITE setPresenceVerb)

        double weight() const { return m_weight; }
        void setWeight(double weight);
        Q_PROPERTY(double weight READ weight WRITE setWeight)

        double cost() const { return m_cost; }
        void setCost(double cost);
        Q_PROPERTY(double cost READ cost WRITE setCost)

        ItemFlags flags() const { return m_flags; }
        void setFlags(ItemFlags flags);
        Q_PROPERTY(ItemFlags flags READ flags WRITE setFlags)

        bool isHidden() const { return m_flags & ItemFlags::Hidden; }
        Q_PROPERTY(bool hidden READ isHidden STORED false)

        bool isPortable() const { return m_flags & ItemFlags::Portable; }
        Q_PROPERTY(bool portable READ isPortable STORED false)

    private:
        Point3D m_position;

        QString m_presenceVerb;

        double m_weight;

        double m_cost;

        ItemFlags m_flags;
};

#endif // ITEM_H
