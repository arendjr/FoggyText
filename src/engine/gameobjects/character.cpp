#include "character.h"

#include <QDateTime>
#include <QDebug>

#include "exit.h"
#include "group.h"
#include "logutil.h"
#include "movementevent.h"
#include "player.h"
#include "portal.h"
#include "race.h"
#include "realm.h"
#include "room.h"
#include "shield.h"
#include "speechevent.h"
#include "util.h"
#include "weapon.h"


#define NO_STUN \
    if (m_secondsStunned > 0) { \
        send(QString("Please wait %1 seconds.").arg(m_secondsStunned), Olive); \
        return; \
    }


#define super StatsItem

Character::Character(Realm *realm, uint id, Options options) :
    Character(realm, GameObjectType::Character, id, options) {

    if (~options & Copy) {
        m_regenerationIntervalId = realm->startInterval(this, 45000);
    }
}

Character::Character(Realm *realm, GameObjectType objectType, uint id, Options options) :
    super(realm, objectType, id, options),
    m_height(0),
    m_direction(0, 0, 0),
    m_gender("male"),
    m_respawnTime(0),
    m_respawnTimeVariation(0),
    m_respawnTimerId(0),
    m_hp(1),
    m_maxHp(1),
    m_mp(0),
    m_maxMp(0),
    m_gold(0.0),
    m_effectsTimerId(0),
    m_secondsStunned(0),
    m_stunTimerId(0),
    m_leaveOnActive(false),
    m_regenerationIntervalId(0) {

    setAutoDelete(false);
}

Character::~Character() {

    if (m_regenerationIntervalId) {
        realm()->stopInterval(m_regenerationIntervalId);
    }
}

CharacterStats Character::totalStats() const {

    try {
        CharacterStats totalStats = super::totalStats();
        if (!m_weapon.isNull()) {
            totalStats += m_weapon.cast<Weapon *>()->totalStats();
        }
        if (!m_secondaryWeapon.isNull()) {
            totalStats += m_secondaryWeapon.cast<Weapon *>()->totalStats();
        }
        if (!m_shield.isNull()) {
            totalStats += m_shield.cast<Shield *>()->totalStats();
        }

        totalStats.dexterity = qMax(totalStats.dexterity - (inventoryWeight() / 5), 0);

        return totalStats;
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::totalStats(): " << exception.what();
        return CharacterStats(0, 0, 0, 0, 0, 0);
    }
}

void Character::setCurrentRoom(const GameObjectPtr &currentRoom) {

    if (m_currentRoom != currentRoom) {
        m_currentRoom = currentRoom;

        setModified();
    }
}

void Character::setDirection(const Vector3D &direction) {

    if (m_direction != direction) {
        m_direction = direction;

        setModified();
    }
}

void Character::addInventoryItem(const GameObjectPtr &item) {

    if (!m_inventory.contains(item)) {
        m_inventory << item;

        setModified();
    }
}

void Character::removeInventoryItem(const GameObjectPtr &item) {

    if (m_inventory.removeOne(item)) {
        setModified();
    }
}

void Character::setInventory(const GameObjectPtrList &inventory) {

    if (m_inventory != inventory) {
        m_inventory = inventory;

        setModified();
    }
}

void Character::addSellableItem(const GameObjectPtr &item) {

    if (!m_sellableItems.contains(item)) {
        m_sellableItems << item;

        setModified();
    }
}

void Character::removeSellableItem(const GameObjectPtr &item) {

    if (m_sellableItems.removeOne(item)) {
        setModified();
    }
}

void Character::setSellableItems(const GameObjectPtrList &items) {

    if (m_sellableItems != items) {
        m_sellableItems = items;

        setModified();
    }
}

void Character::setRace(const GameObjectPtr &race) {

    if (m_race != race) {
        m_race = race;

        setModified();
    }
}

void Character::setClass(const GameObjectPtr &characterClass) {

    if (m_class != characterClass) {
        m_class = characterClass;

        setModified();
    }
}

void Character::setGender(const QString &gender) {

    if (m_gender != gender) {
        m_gender = gender;

        setModified();
    }
}

QString Character::subjectPronoun() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
           "he" : "she" : "it";
}

QString Character::objectPronoun() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
           "him" : "her" : "it";
}

QString Character::possessiveAdjective() const {

    return m_race.isNull() || m_race->name() != "animal" ?
           m_gender == "male" ?
           "his" : "her" : "its";
}

void Character::setHeight(int height) {

    if (m_height != height) {
        m_height = height;

        setModified();
    }
}

void Character::setRespawnTime(int respawnTime) {

    if (m_respawnTime != respawnTime) {
        m_respawnTime = qMax(respawnTime, 0);

        setModified();
    }
}

void Character::setRespawnTimeVariation(int respawnTimeVariation) {

    if (m_respawnTimeVariation != respawnTimeVariation) {
        m_respawnTimeVariation = qMax(respawnTimeVariation, 0);

        setModified();
    }
}

void Character::adjustHp(int delta) {

    setHp(m_hp + delta);
}

void Character::setHp(int hp) {

    if (m_hp != hp) {
        if (mayReferenceOtherProperties()) {
            m_hp = qBound(0, hp, maxHp());
        } else {
            m_hp = hp;
        }

        setModified();
    }
}

void Character::setMaxHp(int maxHp) {

    if (m_maxHp != maxHp) {
        m_maxHp = qMax(maxHp, 0);

        setModified();
    }
}

void Character::adjustMp(int delta) {

    setMp(m_mp + delta);
}

void Character::setMp(int mp) {

    if (m_mp != mp) {
        if (mayReferenceOtherProperties()) {
            m_mp = qBound(0, mp, maxMp());
        } else {
            m_mp = mp;
        }

        setModified();
    }
}

void Character::setMaxMp(int maxMp) {

    if (m_maxMp != maxMp) {
        m_maxMp = qMax(maxMp, 0);

        setModified();
    }
}

void Character::adjustGold(double delta) {

    setGold(m_gold + delta);
}

void Character::setGold(double gold) {

    if (m_gold != gold) {
        m_gold = qMax(gold, 0.0);

        setModified();
    }
}

void Character::setWeapon(const GameObjectPtr &weapon) {

    if (m_weapon != weapon) {
        m_weapon = weapon;

        setModified();
    }
}

void Character::setSecondaryWeapon(const GameObjectPtr &secondaryWeapon) {

    if (m_secondaryWeapon != secondaryWeapon) {
        m_secondaryWeapon = secondaryWeapon;

        setModified();
    }
}

void Character::setShield(const GameObjectPtr &shield) {

    if (m_shield != shield) {
        m_shield = shield;

        setModified();
    }
}

int Character::inventoryWeight() const {

    int inventoryWeight = 0;
    for (const GameObjectPtr &item : m_inventory) {
        inventoryWeight += item.cast<Item *>()->weight();
    }
    if (!m_weapon.isNull()) {
        inventoryWeight += m_weapon.cast<Item *>()->weight();
    }
    if (!m_secondaryWeapon.isNull()) {
        inventoryWeight += m_secondaryWeapon.cast<Item *>()->weight();
    }
    if (!m_shield.isNull()) {
        inventoryWeight += m_shield.cast<Item *>()->weight();
    }
    return inventoryWeight;
}

int Character::maxInventoryWeight() const {

    return 20 + ((stats().strength + stats().endurance) / 2);
}

int Character::totalWeight() const {

    return weight() + inventoryWeight();
}

void Character::addEffect(const Effect &effect) {

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int nextTimeout = updateEffects(now);

    if (nextTimeout == -1 || effect.delay < nextTimeout) {
        if (m_effectsTimerId) {
            realm()->stopTimer(m_effectsTimerId);
        }
        m_effectsTimerId = realm()->startTimer(this, effect.delay);
    }

    m_effects.append(effect);
    m_effects.last().started = now;
}

void Character::clearEffects() {

    m_effects.clear();
    if (m_effectsTimerId) {
        realm()->stopTimer(m_effectsTimerId);
        m_effectsTimerId = 0;
    }
}

void Character::clearNegativeEffects() {

    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        if (effect.hpDelta < 0 || effect.mpDelta < 0) {
            m_effects.removeAt(i);
            i--;
            continue;
        }
    }

    if (m_effects.length() == 0) {
        clearEffects();
    }
}

void Character::open(const GameObjectPtr &exitPtr) {

    NO_STUN

    try {
        Exit *exit = exitPtr.cast<Exit *>();

        if (!exit->isDoor()) {
            send("Exit cannot be opened.");
            return;
        }

        if (exit->isOpen()) {
            send(QString("The %1 is already open.").arg(exit->name()));
        } else {
            if (!exit->invokeTrigger("onopen", this)) {
                return;
            }

            exit->setOpen(true);

            send(QString("You open the %1.").arg(exit->name()));

            GameObjectPtrList others = currentRoom().cast<Room *>()->characters();
            others.removeOne(this);
            others.send(QString("%1 opens the %2.").arg(name(), exit->name()));
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::open(): " << exception.what();
    }
}

void Character::close(const GameObjectPtr &exitPtr) {

    NO_STUN

    try {
        Exit *exit = exitPtr.cast<Exit *>();

        if (!exit->isDoor()) {
            send("Exit cannot be closed.");
            return;
        }

        if (exit->isOpen()) {
            if (!exit->invokeTrigger("onclose", this)) {
                return;
            }

            exit->setOpen(false);
            send(QString("You close the %1.").arg(exit->name()));

            GameObjectPtrList others = currentRoom().cast<Room *>()->characters();
            others.removeOne(this);
            others.send(QString("%1 closes the %2.").arg(name(), exit->name()));
        } else {
            send(QString("The %1 is already closed.").arg(exit->name()));
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::close(): " << exception.what();
    }
}

void Character::go(const GameObjectPtr &pointer) {

    NO_STUN

    try {
        Room *room = currentRoom().cast<Room *>();
        QString exitName;
        GameObjectPtr destination;
        GameObjectPtr portalOrExit;

        if (pointer->isExit()) {
            Exit *exit = pointer.unsafeCast<Exit *>();

            exitName = exit->name();
            if (exit->isDoor() && !exit->isOpen()) {
                send(QString("The %1 is closed.").arg(exitName));
                return;
            }

            destination = exit->destination();
            portalOrExit = pointer;
        } else if (pointer->isPortal()) {
            Portal *portal = pointer.unsafeCast<Portal *>();

            exitName = portal->nameFromRoom(currentRoom());
            if (portal->canOpenFromRoom(currentRoom()) && !portal->isOpen()) {
                send(QString("The %1 is closed.").arg(exitName));
                return;
            }

            destination = portal->oppositeOf(currentRoom());
            portalOrExit = pointer;
        } else if (pointer->isRoom()) {
            for (const GameObjectPtr &portalPtr : room->portals()) {
                Portal *portal = portalPtr.cast<Portal *>();
                if (portal->oppositeOf(currentRoom()) == pointer) {
                    exitName = portal->nameFromRoom(currentRoom());
                    if (portal->canOpenFromRoom(currentRoom()) && !portal->isOpen()) {
                        send(QString("The %1 is closed.").arg(exitName));
                        return;
                    }
                }
            }

            destination = pointer;
        } else {
            qDebug() << "Character::go(): Expected a pointer to an exit, portal or room";
            return;
        }

        for (const GameObjectPtr &character : room->characters()) {
            if (!character->invokeTrigger("oncharacterexit", this, exitName)) {
                return;
            }
        }

        if (!portalOrExit.isNull() && !portalOrExit->invokeTrigger("onenter", this)) {
            return;
        }

        GameObjectPtrList followers;
        if (!m_group.isNull()) {
            Group *group = m_group.cast<Group *>();
            if (group->leader() == this) {
                for (const GameObjectPtr &member : group->members()) {
                    if (member.cast<Character *>()->currentRoom() != room) {
                        continue;
                    }

                    bool blocked = false;
                    for (const GameObjectPtr &character : room->characters()) {
                        if (!character->invokeTrigger("oncharacterexit", member, exitName)) {
                            blocked = true;
                            break;
                        }
                    }
                    if (blocked) {
                        continue;
                    }

                    if (!portalOrExit.isNull() && !portalOrExit->invokeTrigger("onenter", member)) {
                        continue;
                    }

                    followers.append(member);
                }
            }
        }

        Room *sourceRoom = currentRoom().cast<Room *>();
        sourceRoom->removeCharacter(this);
        for (const GameObjectPtr &follower : followers) {
            sourceRoom->removeCharacter(follower);
        }

        Room *destinationRoom = destination.cast<Room *>();
        destinationRoom->addCharacter(this);
        for (const GameObjectPtr &follower : followers) {
            destinationRoom->addCharacter(follower);
        }

        Vector3D movement = destinationRoom->position() - sourceRoom->position();
        Vector3D direction = movement.normalized();

        setCurrentRoom(destination);
        setDirection(direction);
        enteredRoom();

        for (const GameObjectPtr &followerPtr : followers) {
            Character *follower = followerPtr.cast<Character *>();
            follower->setCurrentRoom(destination);
            follower->setDirection(direction);
            follower->send(QString("You follow %1.").arg(name()));
            follower->enteredRoom();
        }

        MovementEvent *event = new MovementEvent(this, sourceRoom, 1.0);
        event->setDestination(destinationRoom);
        event->setExcludedCharacters(followers);
        event->addExcludedCharacter(this);
        event->setMovement(movement);
        event->setDirection(direction);
        event->fire();

        for (const GameObjectPtr &character : destinationRoom->characters()) {
            character->invokeTrigger("oncharacterentered", this);

            for (const GameObjectPtr &follower : followers) {
                character->invokeTrigger("oncharacterentered", follower);
            }
        }

        if (isPlayer()) {
            LogUtil::countRoomVisit(destination.toString(), 1 + followers.length());
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::go(): " << exception.what();
    }
}

void Character::enter(const GameObjectPtr &roomPtr, const GameObjectPtrList &followers) {

    try {
        setCurrentRoom(roomPtr);

        for (const GameObjectPtr &follower : followers) {
            follower.cast<Character *>()->setCurrentRoom(roomPtr);
        }

        Room *room = roomPtr.cast<Room *>();
        GameObjectPtrList characters = room->characters();

        room->addCharacter(this);

        for (const GameObjectPtr &follower : followers) {
            room->addCharacter(follower);
        }

        if (isPlayer()) {
            LogUtil::countRoomVisit(roomPtr.toString(), 1 + followers.length());
        }

        if (!characters.isEmpty()) {
            GameObjectPtrList arrivals;
            arrivals.append(this);
            arrivals.append(followers);
            characters.send(QString("%1 arrived.").arg(arrivals.joinFancy(Capitalized)));
        }

        enteredRoom();

        for (const GameObjectPtr &follower : followers) {
            follower->send(QString("You follow %1.").arg(name()));
            follower.cast<Character *>()->enteredRoom();
        }

        for (const GameObjectPtr &character : characters) {
            character->invokeTrigger("oncharacterentered", this);

            for (const GameObjectPtr &follower : followers) {
                character->invokeTrigger("oncharacterentered", follower);
            }
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::enter(): " << exception.what();
    }
}

void Character::leave(const GameObjectPtr &roomPtr, const QString &exitName,
                      const GameObjectPtrList &followers) {

    try {
        Room *room = roomPtr.cast<Room *>();

        room->removeCharacter(this);

        for (const GameObjectPtr &follower : followers) {
            room->removeCharacter(follower);
        }

        GameObjectPtrList characters = room->characters();
        if (!characters.isEmpty()) {
            QString departuresName;
            if (followers.isEmpty()) {
                bool unique = true;
                for (const GameObjectPtr &other : characters) {
                    if (other->name() == name()) {
                        unique = false;
                        break;
                    }
                }
                if (unique) {
                    departuresName = definiteName(characters, Capitalized);
                } else {
                    departuresName = indefiniteName(Capitalized);
                }
            } else {
                GameObjectPtrList departures;
                departures << this;
                departures << followers;
                departuresName = departures.joinFancy(Capitalized);
            }

            characters.send(exitName.isEmpty() ?
                            QString("%1 left.").arg(departuresName) :
                            QString("%1 left %2%3.").arg(departuresName,
                                                         Util::isDirection(exitName) ? "" :
                                                                                       "to the ",
                                                         exitName));
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::leave(): " << exception.what();
    }
}

void Character::say(const QString &message) {

    try {
        Room *room = currentRoom().cast<Room *>();
        SpeechEvent *event = new SpeechEvent(this, message, room, 1.0);
        event->fire();
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::say(): " << exception.what();
    }
}

void Character::shout(const QString &message) {

    try {
        Room *room = currentRoom().cast<Room *>();
        SpeechEvent *event = new SpeechEvent(this, message, room, 5.0);
        event->fire();

        for (const GameObjectPtr &character : event->affectedCharacters()) {
            character->invokeTrigger("onshout", this, message);
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::shout(): " << exception.what();
    }
}

void Character::talk(const GameObjectPtr &character, const QString &message) {

    try {
        Room *room = currentRoom().cast<Room *>();
        SpeechEvent *event = new SpeechEvent(this, message, room, 1.0, character);
        event->fire();

        if (!character->isPlayer()) {
            LogUtil::logNpcTalk(character->name(), message);
            character->invokeTrigger("ontalk", this, message);
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::talk(): " << exception.what();
    }
}

void Character::tell(const GameObjectPtr &playerPtr, const QString &message) {

    try {
        Player *player = playerPtr.cast<Player *>();

        if (message.isEmpty()) {
            send(QString("Tell %1 what?").arg(player->name()));
            return;
        }

        player->send(QString("%1 tells you, \"%2\".").arg(name(), message));
        send(QString("You tell %1, \"%2\".").arg(player->name(), message));
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::tell(): " << exception.what();
    }
}

void Character::take(const GameObjectPtrList &items) {

    try {
        Room *room = currentRoom().cast<Room *>();

        GameObjectPtrList takenItems;
        for (const GameObjectPtr &itemPtr : items) {
            Item *item = itemPtr.cast<Item *>();
            if (item->isPortable()) {
                if (inventoryWeight() + item->weight() <= maxInventoryWeight()) {
                    addInventoryItem(itemPtr);
                    room->removeItem(itemPtr);
                    takenItems << itemPtr;
                } else {
                    send(QString("You can't take %2, because it's too heavy.")
                         .arg(item->definiteName(room->items())));
                }
            } else {
                send(QString("You can't take %2.").arg(item->definiteName(room->items())));
            }
        }

        if (takenItems.length() > 0) {
            QString description = takenItems.joinFancy(DefiniteArticles);
            send(QString("You take %2.").arg(description));

            GameObjectPtrList others = room->characters();
            others.removeOne(this);
            others.send(QString("%1 takes %3.").arg(definiteName(room->characters(), Capitalized),
                                                    description));
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::take(): " << exception.what();
    }
}

void Character::wield(const GameObjectPtr &item) {

    try {
        if (!m_inventory.contains(item)) {
            return;
        }

        if (item->isWeapon()) {
            if (m_weapon.isNull()) {
                send(QString("You wield your %1.").arg(item->name()));
                m_weapon = item;
            } else {
                if (m_class->name() == "wanderer" && m_secondaryWeapon.isNull()) {
                    send(QString("You wield your %1 as secondary weapon.").arg(item->name()));
                    m_secondaryWeapon = item;
                } else if (m_weapon->name() == item->name()) {
                    send(QString("You swap your %1 for another %1.")
                         .arg(m_weapon->name(), item->name()));
                    m_weapon = item;
                } else {
                    send(QString("You remove your %1 and wield your %1.")
                         .arg(m_weapon->name(), item->name()));
                    m_weapon = item;
                }
            }
            m_inventory.removeOne(item);
        } else if (item->isShield()) {
            if (m_shield.isNull()) {
                send(QString("You wield your %1.").arg(item->name()));
                m_shield = item;
            } else {
                send(QString("You remove your %1 and wield your %1.")
                     .arg(m_shield->name(), item->name()));
                m_shield = item;
            }
            m_inventory.removeOne(item);
        } else {
            send("You cannot wield that.");
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::wield(): " << exception.what();
    }
}

void Character::remove(const GameObjectPtr &item) {

    try {
        if (m_weapon == item) {
            send(QString("You remove your %1.").arg(item->name()));
            m_inventory << item;
            m_weapon = GameObjectPtr();
        } else if (m_secondaryWeapon == item) {
            send(QString("You remove your %1.").arg(item->name()));
            m_inventory << item;
            m_secondaryWeapon = GameObjectPtr();
        } else if (m_shield == item) {
            send(QString("You remove your %1.").arg(item->name()));
            m_inventory << item;
            m_shield = GameObjectPtr();
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::remove(): " << exception.what();
    }
}

void Character::kill(const GameObjectPtr &characterPtr) {

    NO_STUN

    try {
        Character *character = characterPtr.cast<Character *>();

        if (!character->invokeTrigger("onattack", this)) {
            return;
        }

        Room *room = currentRoom().cast<Room *>();
        GameObjectPtrList others = room->characters();
        others.removeOne(this);
        others.removeOne(characterPtr);

        CharacterStats myStats = totalStats();
        CharacterStats enemyStats = character->totalStats();

        qreal hitChance = 100 * ((80 + myStats.dexterity) / 160.0) *
                                ((100 - enemyStats.dexterity) / 100.0);
        int damage = 0;
        if (qrand() % 100 < hitChance) {
            damage = qrand() % (int) (20.0 * (myStats.strength / 40.0) *
                                              ((80 - enemyStats.endurance) / 80.0)) + 1;

            character->adjustHp(-damage);
        }

        bool invoked = false;
        if (room->hasTrigger("oncombat")) {
            invoked = room->invokeTrigger("oncombat", this, characterPtr, others, damage);
        }
        if (!invoked) {
            realm()->invokeTrigger("oncombat", this, characterPtr, others, damage);
        }

        stun(4000 - (25 * myStats.dexterity));

        others = currentRoom().cast<Room *>()->characters();
        others.removeOne(this);
        others.removeOne(characterPtr);
        for (const GameObjectPtr &other : others) {
            other->invokeTrigger("oncharacterattacked", this, characterPtr);
        }

        if (damage > 0 && character->hp() == 0) {
            character->die(this);
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::kill(): " << exception.what();
    }
}

void Character::die(const GameObjectPtr &attacker) {

    try {
        if (!invokeTrigger("ondie", attacker)) {
            return;
        }

        send("You died.", Red);

        Room *room = currentRoom().cast<Room *>();

        GameObjectPtrList others = room->characters();
        others.removeOne(this);

        QString myName = definiteName(room->characters(), Capitalized);
        others.send(QString("%1 died.").arg(myName), Teal);

        if (inventory().length() > 0) {
            for (const GameObjectPtr &item : inventory()) {
                room->addItem(item);
            }

            QString droppedItemsDescription = inventory().joinFancy();

            setInventory(GameObjectPtrList());

            others.send(QString("%1 was carrying %2.").arg(myName, droppedItemsDescription), Teal);
        }

        for (const GameObjectPtr &other : others) {
            other->invokeTrigger("oncharacterdied", this, attacker);
        }

        killAllTimers();

        room->removeCharacter(this);

        if (isPlayer()) {
            LogUtil::countPlayerDeath(currentRoom().toString());

            enter(race().cast<Race *>()->startingRoom());

            setHp(1);
            stun(5000);
        } else {
            if (m_respawnTime) {
                setInventory(GameObjectPtrList());

                int respawnTime = m_respawnTime;
                if (m_respawnTimeVariation) {
                    respawnTime += qrand() % m_respawnTimeVariation;
                }
                m_respawnTimerId = realm()->startTimer(this, respawnTime);
            } else {
                setDeleted();
            }
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::die(): " << exception.what();
    }
}

void Character::follow(const GameObjectPtr &characterPtr) {

    try {
        Character *character = characterPtr.cast<Character *>();

        if (character == this) {
            send("You cannot follow yourself.");
            return;
        }
        if (isPlayer()) {
            if (!characterPtr->isPlayer()) {
                GameObjectPtrList characters = currentRoom().cast<Room *>()->characters();
                QString name = character->definiteName(characters, DefiniteArticles);
                send(QString("You cannot follow %1.").arg(name));
                return;
            }
        } else {
            if (characterPtr->isPlayer()) {
                return;
            }
        }

        if (!m_group.isNull()) {
            Group *group = m_group.cast<Group *>();
            if (group->leader() == characterPtr) {
                send(QString("You are already following %1.").arg(character->name()));
                return;
            }
            if (group->members().contains(characterPtr)) {
                send(QString("You are already in the same group as %1.").arg(character->name()));
                return;
            }

            lose();
        }

        Group *group;
        if (character->m_group.isNull()) {
            group = new Group(realm());
            group->setLeader(characterPtr);
            character->m_group = group;
        } else {
            group = character->m_group.cast<Group *>();
        }

        if (group->members().length() == 0) {
            send(QString("You started following %1.").arg(character->name()));
            character->send(QString("%1 started following you.").arg(name()));
        } else {
            send(QString("You joined the group of %1, led by %2.").arg(group->members().joinFancy(),
                                                                       group->leader()->name()));
            group->send(QString("%1 joined your group.").arg(name()));
        }

        group->addMember(this);
        m_group = group;
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::follow(): " << exception.what();
    }
}

void Character::lose(const GameObjectPtr &character) {

    try {
        if (m_group.isNull()) {
            send("You're not part of any group.");
            return;
        }

        Group *group = m_group.cast<Group *>();

        if (character.isNull()) {
            if (group->leader() == this) {
                disband();
            } else {
                if (group->members().length() > 1) {
                    group->removeMember(this);
                    group->send(QString("%1 left the group.").arg(name()));
                    send("You left the group.");
                } else {
                    Character *leader = group->leader().cast<Character *>();
                    leader->send(QString("%1 has stopped following you.").arg(name()));
                    leader->m_group = GameObjectPtr();
                    group->setDeleted();
                    send(QString("You stopped following %1.").arg(group->leader()->name()));
                }
            }
            m_group = GameObjectPtr();
        } else {
            if (character == this) {
                lose();
            } else {
                if (group->leader() == this) {
                    if (group->members().contains(character)) {
                        if (group->members().length() > 1) {
                            group->removeMember(character);
                            character->send(QString("%1 has removed you from the group.").arg(name()));
                            send(QString("You removed %1 from the group.").arg(character->name()));
                        } else {
                            disband();
                        }
                    } else {
                        send(QString("%1 is not a member of the group.").arg(character->name()));
                    }
                } else {
                    send("Only the group leader can lose people from the group.");
                }
            }
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::lose(): " << exception.what();
    }
}

void Character::disband() {

    try {
        if (m_group.isNull()) {
            send("You're not part of any group.");
            return;
        }

        Group *group = m_group.cast<Group *>();
        if (group->leader() != this) {
            send("Only the group leader can disband the group.");
            return;
        }

        for (const GameObjectPtr &memberPtr : group->members()) {
            Character *member = memberPtr.cast<Character *>();
            member->send(QString("%1 has disbanded the group.").arg(name()));
            member->m_group = GameObjectPtr();
        }

        group->setDeleted();
        send("You disbanded the group.");
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::disband(): " << exception.what();
    }
}

void Character::stun(int timeout) {

    if (m_stunTimerId) {
        realm()->stopTimer(m_stunTimerId);
    }

    int seconds = timeout / 1000;
    int firstTimeout = timeout % 1000;
    if (firstTimeout == 0) {
        firstTimeout = 1000;
    } else {
        seconds++;
    }

    m_secondsStunned = seconds;
    m_stunTimerId = realm()->startTimer(this, firstTimeout);
}

void Character::setLeaveOnActive(bool leaveOnActive) {

    m_leaveOnActive = leaveOnActive;
}

void Character::init() {

    try {
        // guarantee our current room lists us, otherwise we may end up in a
        // real limbo if we died and a server termination killed the respawn
        // timers. in addition, this allows Room::characters to become a
        // non-stored property, saving many disk writes when characters walk
        // around
        if (!isPlayer() && !currentRoom().isNull()) {
            Room *room = currentRoom().cast<Room *>();
            room->addCharacter(this);
        }

        if (hasTrigger("oninit")) {
            super::init();
        } else {
            invokeTrigger("onspawn");
        }
    } catch (GameException &exception) {
        qDebug() << "Exception in Character::init(): " << exception.what();
    }
}

GameObject *Character::copy() {

    Character *other = qobject_cast<Character *>(super::copy());
    if (!m_weapon.isNull()) {
        other->setWeapon(m_weapon->copy());
    }
    if (!m_secondaryWeapon.isNull()) {
        other->setSecondaryWeapon(m_secondaryWeapon->copy());
    }
    if (!m_shield.isNull()) {
        other->setShield(m_shield->copy());
    }
    other->setInventory(GameObjectPtrList());
    for (const GameObjectPtr &item : m_inventory) {
        other->addInventoryItem(item->copy());
    }
    return other;
}

void Character::invokeTimer(int timerId) {

    if (timerId == m_respawnTimerId) {
        m_respawnTimerId = 0;

        m_hp = m_maxHp;
        m_mp = m_maxMp;

        setModified();

        enter(currentRoom());

        invokeTrigger("onspawn");
    } else if (timerId == m_effectsTimerId) {
        int nextTimeout = updateEffects(QDateTime::currentMSecsSinceEpoch());
        m_effectsTimerId = (nextTimeout > -1 ? realm()->startTimer(this, nextTimeout) : 0);
    } else if (timerId == m_stunTimerId) {
        m_secondsStunned--;

        if (m_secondsStunned > 0) {
            m_stunTimerId = realm()->startTimer(this, 1000);
        } else {
            m_stunTimerId = 0;

            if (m_leaveOnActive) {
                leave(currentRoom());
                m_leaveOnActive = false;
            } else {
                invokeTrigger("onactive");
            }
        }
    } else if (timerId == m_regenerationIntervalId) {
        adjustHp(qMax(stats().vitality / 15, 1));
    } else {
        super::invokeTimer(timerId);
    }
}

void Character::killAllTimers() {

    super::killAllTimers();

    if (m_respawnTimerId) {
        realm()->stopTimer(m_respawnTimerId);
        m_respawnTimerId = 0;
    }

    clearEffects();

    m_secondsStunned = 0;
    if (m_stunTimerId) {
        realm()->stopTimer(m_stunTimerId);
        m_stunTimerId = 0;
    }
}

void Character::changeName(const QString &newName) {

    super::changeName(newName);

    if (newName.toLower() != newName) {
        setIndefiniteArticle("");
    }

    realm()->addReservedName(newName);
}

void Character::changeStats(const CharacterStats &newStats) {

    super::changeStats(newStats);

    setMaxHp(2 * newStats.vitality);
    setMaxMp(newStats.intelligence);
}

void Character::enteredRoom() {

    invokeTrigger("onentered");
}

int Character::updateEffects(qint64 now) {

    int nextTimeout = -1;
    for (int i = 0; i < m_effects.length(); i++) {
        Effect &effect = m_effects[i];

        int msecsLeft = (effect.started + effect.delay) - now;
        while (msecsLeft <= 0) {
            adjustHp(effect.hpDelta);
            adjustMp(effect.mpDelta);
            send(effect.message);

            effect.numOccurrences--;
            if (effect.numOccurrences > 0) {
                effect.started = now + msecsLeft;
                msecsLeft = effect.delay + msecsLeft;
            } else {
                m_effects.removeAt(i);
                i--;
                break;
            }
        }

        if (msecsLeft > 0 && (nextTimeout == -1 || msecsLeft < nextTimeout)) {
            nextTimeout = msecsLeft;
        }
    }

    return nextTimeout;
}
