#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <QObject>
#include <QScriptValue>
#include <QString>

#include "gameobjectptr.h"
#include "metatyperegistry.h"


class Room;
class QScriptEngine;


PT_DEFINE_ENUM(GameEventType,
    Unknown,
    SoundEvent,
    VisualEvent
)


class GameEvent : public QObject {

    Q_OBJECT

    public:
        GameEvent(GameEventType eventType, Room *origin, double strength);
        virtual ~GameEvent();

        GameEventType eventType() const { return m_eventType; }
        Q_PROPERTY(GameEventType eventType READ eventType STORED false)

        Q_INVOKABLE bool isSoundEvent() const;
        Q_INVOKABLE bool isVisualEvent() const;

        GameObjectPtr origin() const;
        Q_PROPERTY(GameObjectPtr origin READ origin STORED false)

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);
        Q_PROPERTY(QString description READ description WRITE setDescription)

        const GameObjectPtrList &excludedCharacters() const { return m_excludedCharacters; }
        void addExcludedCharacter(const GameObjectPtr &excludedCharacter);
        void setExcludedCharacters(const GameObjectPtrList &excludedCharacters);
        Q_PROPERTY(GameObjectPtrList excludedCharacters READ excludedCharacters
                                                        WRITE setExcludedCharacters)

        const GameObjectPtrList &affectedCharacters() const { return m_affectedCharacters; }
        void addAffectedCharacter(const GameObjectPtr &affectedCharacter);
        void setAffectedCharacters(const GameObjectPtrList &affectedCharacters);
        Q_PROPERTY(GameObjectPtrList affectedCharacters READ affectedCharacters
                                                        WRITE setAffectedCharacters)

        const QString &distantDescription() const { return m_distantDescription; }
        void setDistantDescription(const QString &distantDescription);
        Q_PROPERTY(QString distantDescription READ distantDescription WRITE setDistantDescription)

        const QString &veryDistantDescription() const { return m_veryDistantDescription; }
        void setVeryDistantDescription(const QString &veryDistantDescription);
        Q_PROPERTY(QString veryDistantDescription READ veryDistantDescription
                                                  WRITE setVeryDistantDescription)

        void fire();

        static QScriptValue toScriptValue(QScriptEngine *engine, GameEvent *const &event);
        static void fromScriptValue(const QScriptValue &object, GameEvent *&event);

        QVector<QMetaProperty> storedMetaProperties() const;

    protected:
        virtual void visitRoom(Room *room, double strength) = 0;

        void addVisit(Room *room, double strength);
        bool hasBeenVisited(Room *room) const;

    private:
        class Visit {
            public:
                Room *room;
                double strength;

                Visit() = default;
                Visit(Room *room, double strength) :
                    room(room),
                    strength(strength) {
                }
        };

        GameEventType m_eventType;

        Room *m_origin;

        QVector<Visit> m_visits;
        int m_nextVisitIndex;

        QString m_description;
        QString m_distantDescription;
        QString m_veryDistantDescription;

        GameObjectPtrList m_excludedCharacters;
        GameObjectPtrList m_affectedCharacters;
};

PT_DECLARE_METATYPE(GameEvent *)

#endif // GAMEEVENT_H
