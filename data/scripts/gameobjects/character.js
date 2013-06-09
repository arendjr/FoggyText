
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Character") instead.
 */
function Character() {

    this.currentAction = "";
    this.currentActionTimerId = 0;
}

/**
 * This method is called whenever the stats of the character change.
 *
 * @param newStats Array containing the new stats.
 */
Character.prototype.changeStats = function(newStats) {

    this.maxHp = 2 * newStats[VITALITY];
};

/**
 * Closes a portal.
 *
 * @param portal The portal to close.
 */
Character.prototype.close = function(portal) {

    if (!portal.canOpenFromRoom(this.currentRoom)) {
        this.send("Exit cannot be closed.");
        return;
    }

    var name = portal.nameFromRoom(this.currentRoom);
    if (portal.open) {
        if (!portal.invokeTrigger("onclose", this)) {
            return;
        }

        portal.open = false;
        this.send("You close the %1.".arg(name));

        var others = this.currentRoom.characters;
        others.removeOne(this);
        others.send("%1 closes the %2.".arg(this.definiteName(this.currentRoom.characters), name));
    } else {
        this.send("The %1 is already closed.".arg(name));
    }
};

/**
 * Makes the character die.
 *
 * @param attacker The character who killed this character (optional).
 */
Character.prototype.die = function(attacker) {

    if (!this.invokeTrigger("ondie", attacker)) {
        return;
    }

    this.send("You died.", Color.Red);

    var room = this.currentRoom;
    var myName = this.definiteName(room.characters, Options.Capitalized);

    var others = room.characters;
    others.removeOne(this);
    others.send("%1 died.".arg(myName), Color.Teal);

    if (!this.inventory.isEmpty()) {
        this.inventory.forEach(function(item) {
            room.addItem(item);
        });

        var droppedItemsDescription = this.inventory.joinFancy();
        others.send("%1 was carrying %2.".arg(myName, droppedItemsDescription), Color.Teal);

        this.inventory = [];
    }

    for (var i = 0, length = others.length; i < length; i++) {
        others[i].invokeTrigger("oncharacterdied", this, attacker);
    };

    this.killAllTimers();

    this.setAction("");

    room.removeCharacter(this);

    if (this.isPlayer()) {
        LogUtil.countPlayerDeath(room.toString());

        this.enter(this.race.startingRoom);

        this.hp = 1;
        this.stun(5000);
    } else {
        if (this.respawnTime) {
            var respawnTime = this.respawnTime + randomInt(0, this.respawnTimeVariation);
            this.setTimeout(function() {
                this.hp = this.maxHp;

                this.enter(this.currentRoom);

                this.invokeTrigger("onspawn");
            }, respawnTime);
        } else {
            this.setDeleted();
        }
    }
};

/**
 * Makes the character travel to another room.
 *
 * @param object A portal or room. If a portal is given, it is assumed to be reachable from the
 *               current room. If a room is given, the portal to travel through is determined
 *               automatically. However, if the portal cannot be determined (for example, if the
 *               rooms aren't adjacent) the character will travel to the other room regardless).
 */
Character.prototype.go = function(object) {

    if (!object) {
        return;
    }

    if (this.secondsStunned() > 0) {
        this.send("Please wait %1 seconds.".arg(this.secondsStunned()), Color.Olive);
        return;
    }

    var i, length, exitName = "", destination = null, portal = null;

    if (object.isPortal()) {
        portal = object;
        exitName = portal.nameFromRoom(this.currentRoom);
        destination = portal.oppositeOf(this.currentRoom);
    } else if (object.isRoom()) {
        for (i = 0, length = this.currentRoom.portals.length; i < length; i++) {
            var potentialPortal = this.currentRoom.portals[i];
            if (potentialPortal.oppositeOf(this.currentRoom) === object) {
                portal = potentialPortal;
                exitName = portal.nameFromRoom(this.currentRoom);
                break;
            }
        }
        destination = object;
    } else {
        return;
    }

    var character, numCharacters = this.currentRoom.characters.length;
    for (i = 0; i < numCharacters; i++) {
        character = this.currentRoom.characters[i];
        if (character !== this) {
            if (!character.invokeTrigger("oncharacterexit", this, exitName)) {
                return;
            }
        }
    }

    if (portal) {
        if (portal.canOpen() && !portal.open) {
            this.send("The %1 is closed.".arg(exitName));
            return;
        }
        if (!portal.invokeTrigger("onenter", this)) {
            return;
        }
        if (!portal.canPassThrough()) {
            this.send("You cannot go there.");
            return;
        }
    }

    var action;
    if (this.currentAction === "walk" || this.currentAction === "run") {
        action = "run";
    } else {
        action = "walk";
    }
    this.setAction(action, { "duration": 4000 });

    var followers = [];
    if (this.group && this.group.leader === this) {
        for (i = 0, length = this.group.members.length; i < length; i++) {
            var member = this.group.members[i];
            if (member.currentRoom !== this.currentRoom) {
                continue;
            }

            var blocked = false;
            for (var j = 0; j < numCharacters; j++) {
                character = this.currentRoom.characters[j];
                if (!character.invokeTrigger("oncharacterexit", this, exitName)) {
                    blocked = true;
                    break;
                }
            }
            if (blocked) {
                continue;
            }

            if (portal && !portal.invokeTrigger("onenter", member)) {
                continue;
            }

            followers.append(member);
        }
    }

    var source = this.currentRoom;
    var movement = destination.position.minus(source.position);
    var direction = movement.normalized();

    this.leave(source);
    this.direction = direction;
    this.enter(destination);

    for (i = 0, length = followers.length; i < length; i++) {
        var follower = followers[i];
        follower.leave(source);
        follower.direction = direction;
        follower.enter(destination);
        follower.send("You follow %1.".arg(this.name));
        this.setAction(action, { "duration": 4000 });
    }

    var party = [this].concat(followers);

    var simplePresent = (followers.isEmpty() ? action + "s" : action);
    var continuous = (followers.isEmpty() ? "is" : "are") + " " +
                     (action === "walk" ? "walking" : "running");

    var visualEvent = Realm.createEvent("MovementVisual", source, 1.0);
    visualEvent.subject = (followers.isEmpty() ? this : this.group);
    visualEvent.destination = destination
    visualEvent.movement = movement;
    visualEvent.direction = direction;
    visualEvent.setVerb(simplePresent, continuous);
    visualEvent.excludedCharacters = party;
    visualEvent.fire();

    if (!this.race || this.race.name !== "animal") {
        var soundStrength = (action === "walk" ? 1.0 : 3.0);
        var soundDescription = "someone";

        if (!followers.isEmpty()) {
            soundDescription = "some people";
            for (i = 0, length = followers.length; i < length; i++) {
                soundStrength += Math.max(0.8 - 0.2 * i, 0.3);
            }
        }

        var soundEvent = Realm.createEvent("MovementSound", source, soundStrength);
        soundEvent.description = soundDescription;
        soundEvent.distantDescription = soundDescription;
        soundEvent.veryDistantDescription = soundDescription;
        soundEvent.destination = destination;
        soundEvent.movement = movement;
        soundEvent.direction = direction;
        soundEvent.setVerb(simplePresent, continuous);
        soundEvent.excludedCharacters = party.concat(visualEvent.affectedCharacters);
        soundEvent.fire();
    }

    if (this.isPlayer()) {
        LogUtil.countRoomVisit(destination.toString(), 1 + followers.length);
    }
};

/**
 * Makes the character stand guard. This does not actually do anything other than applying the
 * description when a player looks at the character.
 */
Character.prototype.guard = function(target) {

    this.setAction("guard", { "target": target });
};

/**
 * Returns the weight of all the inventory (including those currently wielded) carried by the
 * character.
 *
 * @return number
 */
Character.prototype.inventoryWeight = function() {

    var inventoryWeight = 0.0;
    this.inventory.forEach(function(item) {
        inventoryWeight += item.weight;
    });
    if (this.weapon) {
        inventoryWeight += this.weapon.weight;
    }
    if (this.secondaryWeapon) {
        inventoryWeight += this.secondaryWeapon.weight;
    }
    if (this.shield) {
        inventoryWeight += this.shield.weight;
    }
    return inventoryWeight;
};

/**
 * Attempts to kill another character.
 */
Character.prototype.kill = function(character) {

    if (this.secondsStunned() > 0) {
        this.send("Please wait %1 seconds.".arg(this.secondsStunned()), Color.Olive);
        return;
    }

    this.setAction("fight", { "target": character, "duration": 4000 });

    if (!character.invokeTrigger("onattack", this)) {
        return;
    }

    var room = this.currentRoom;

    var others = room.characters;
    others.removeOne(this);
    others.removeOne(character);

    var invoked = false;
    if (room.hasTrigger("oncombat")) {
        invoked = room.invokeTrigger("oncombat", this, character, others);
    }
    if (!invoked) {
        Realm.invokeTrigger("oncombat", this, character, others);
    }

    for (var i = 0, length = others.length; i < length; i++) {
        others[i].invokeTrigger("oncharacterattacked", this, character);
    }

    if (character.hp === 0) {
        character.die(this);
    }
};

/**
 * Looks in the direction the character is currently facing.
 */
Character.prototype.lookAhead = function() {

    var room = this.currentRoom;

    var strength = room.eventMultiplier("Visual");
    if (this.weapon && this.weapon.name === "binocular") {
        strength *= 4;
    }

    var itemGroups = VisualUtil.divideItemsIntoGroups(room.items, this.direction);
    var portalGroups = VisualUtil.dividePortalsAndCharactersIntoGroups(this, room, strength);

    var combinedItems = Util.combinePtrList(itemGroups["ahead"]);
    portalGroups["ahead"].forEach(function(portal) {
        combinedItems.append(portal.nameWithDestinationFromRoom(room));
    });

    if (!combinedItems.isEmpty()) {
        this.send("You see %1.".arg(Util.joinFancy(combinedItems)));
    }

    if (portalGroups.hasOwnProperty("characters")) {
        this.send(VisualUtil.describeCharactersRelativeTo(portalGroups["characters"], this));
    }
};

/**
 * Returns the description of the character as seen by another character.
 *
 * @param character The character looking at this character.
 *
 * @return string
 */
Character.prototype.lookAtBy = function(character) {

    var pool = this.currentRoom.characters;

    var text;
    if (character.id === this.id) {
        text = "You look at yourself.\n";
    } else if (this.isPlayer()) {
        text = "You look at %1, a %2 %3.\n"
               .arg(this.definiteName(pool), this.race.adjective, this.characterClass.name);
    } else {
        text = "You look at %1.\n".arg(this.definiteName(pool));
    }

    if (!this.description.isEmpty()) {
        text += this.description + "\n";
    }

    var wieldedItems = [];
    if (this.weapon) {
        wieldedItems.append(this.weapon);
    }
    if (this.secondaryWeapon) {
        wieldedItems.append(this.secondaryWeapon);
    }
    if (this.shield) {
        wieldedItems.append(this.shield);
    }
    if (!wieldedItems.isEmpty()) {
        if (character.id === this.id) {
            text += "You are holding %1.\n".arg(wieldedItems.joinFancy());
        } else {
            text += "%1 is holding %2.\n".arg(this.subjectPronoun.capitalized())
                                         .arg(wieldedItems.joinFancy());
        }
    }

    return text;
};

/**
 * Returns the maximum amount of inventory the character may carry.
 *
 * @return number
 */
Character.prototype.maxInventoryWeight = function() {

    return 20 + this.stats[STRENGTH] + Math.floor(this.stats[ENDURANCE] / 2);
};

/**
 * Returns the name of the character or a more fuzzy description, depending on the strength
 * (clarity) with which the character is being observed.
 *
 * @param strength The strength with which the character is being observed. 1.0 represents a full
 *                 clear view, while 0.0 means the character has become invisible.
 *
 * @return string
 */
Character.prototype.nameAtStrength = function(strength) {

    if (strength >= 0.9) {
        return this.flags.split("|").contains("AlwaysUseDefiniteArticle") ?
               "the " + this.name : this.indefiniteName();
    } else if (strength >= 0.8) {
        return this.gender === "male" ? "a man" : "a woman";
    } else {
        return "someone";
    }
};

/**
 * Opens a portal.
 *
 * @param portal The portal to open.
 */
Character.prototype.open = function(portal) {

    if (!portal.canOpenFromRoom(this.currentRoom)) {
        this.send("Exit cannot be opened.");
        return;
    }

    var name = portal.nameFromRoom(this.currentRoom);
    if (portal.open) {
        this.send("The %1 is already open.".arg(name));
    } else {
        if (!portal.invokeTrigger("onopen", this)) {
            return;
        }

        portal.open = true;
        this.send("You open the %1.".arg(name));

        var others = this.currentRoom.characters;
        others.removeOne(this);
        others.send("%1 opens the %2.".arg(this.definiteName(this.currentRoom.characters), name));
    }
};

/**
 * Regenerates health of the character.
 *
 * This method is automatically called periodically.
 */
Character.prototype.regenerate = function() {

    this.hp += Math.max(Math.floor(this.stats[VITALITY] / 15), 1);
};

/**
 * Removes a currently wielded item.
 *
 * @param item
 */
Character.prototype.remove = function(item) {

    if (this.weapon === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.weapon = null;
    } else if (this.secondaryWeapon === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.secondaryWeapon = null;
    } else if (this.shield === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.shield = null;
    }
};

/**
 * Says a messages.
 *
 * @param message The message to say.
 */
Character.prototype.say = function(message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 1.0);
    event.speaker = this;
    event.message = message;
    event.fire();

    this.setAction("talk", { "duration": 4000 });
};

/**
 * Sets the current action. Changing the action affects how the character is perceived by other
 * characters.
 *
 * @param action The new action.
 * @param options Options object (optional). May contain the following properties:
 *                - target: Target relevant to the action (e.g. the character spoken to).
 *                - duration: Time in milliseconds after which the action will be reset.
 */
Character.prototype.setAction = function(action, options) {

    options = options || {};

    this.currentAction = action;

    this.target = options.target;

    if (this.weapon && this.weapon.name === "binocular") {
        this.remove(this.weapon);
    }

    if (this.currentActionTimerId) {
        this.clearTimeout(this.currentActionTimerId);
    }

    if (options.duration) {
        this.currentActionTimerId = this.setTimeout(function() {
            this.currentAction = "";
            this.currentActionTimerId = 0;
        }, options.duration);
    } else {
        this.currentActionTimerId = 0;
    }
};

/**
 * Shouts a message.
 *
 * @param message The message to shout.
 */
Character.prototype.shout = function(message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 5.0);
    event.speaker = this;
    event.message = message;
    event.fire();

    var self = this;
    event.affectedCharacters.forEach(function(character) {
        character.invokeTrigger("onshout", self, message);
    });

    this.setAction("shout", { "duration": 4000 });
};

/**
 * Takes (picks up) one or more items from the current room.
 *
 * @param items Array containing items to take.
 */
Character.prototype.take = function(items) {

    var room = this.currentRoom;

    var takenItems = [];
    for (var i = 0, length = items.length; i < length; i++) {
        var item = items[i];
        if (item.portable) {
            if (this.inventoryWeight() + item.weight <= this.maxInventoryWeight()) {
                this.addInventoryItem(item);
                room.removeItem(item);
                takenItems.append(item);

                item.position = [0, 0, 0];
            } else {
                this.send("You can't take %1, because it's too heavy."
                          .arg(item.definiteName(room.items)));
            }
        } else {
            this.send("You can't take %1.".arg(item.definiteName(room.items)));
        }
    }

    if (!takenItems.isEmpty()) {
        var description = takenItems.joinFancy(Options.DefiniteArticles);
        this.send("You take %2.".arg(description));

        var others = room.characters;
        others.removeOne(this);
        others.send("%1 takes %2.".arg(this.definiteName(room.characters, Options.Capitalized),
                                       description));
    }
};

/**
 * Talks to another character.
 *
 * @param character The character to talk to.
 * @param message The message to say.
 */
Character.prototype.talk = function(character, message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 1.0);
    event.speaker = this;
    event.message = message;
    event.target = character;
    event.fire();

    if (!character.isPlayer()) {
        character.invokeTrigger("ontalk", this, message);

        if (this.isPlayer()) {
            LogUtil.logNpcTalk(character.name, message);
        }
    }

    this.setAction("talk", { "duration": 4000, "target": character });
};

/**
 * Tells a message to another player who is not necessarily in the same room.
 *
 * @param player The player to tell the message to.
 * @param message The message to tell.
 */
Character.prototype.tell = function(player, message) {

    if (message.isEmpty()) {
        this.send("Tell %1 what?".arg(player.name));
        return;
    }

    player.send("%1 tells you, \"%2\".".arg(this.name, message));
    this.send("You tell %1, \"%2\".".arg(player.name, message));
};

/**
 * Returns the total weight of the character, including all carried inventory.
 *
 * @param return number
 */
Character.prototype.totalWeight = function() {

    return this.weight + this.inventoryWeight();
};

Character.prototype.walkTo = function(character) {

    var currentRoom = this.currentRoom;
    var targetRoom = character.currentRoom;
    if (targetRoom === currentRoom) {
        return;
    }

    var queue = [];
    currentRoom.portals.forEach(function(portal) {
        queue.push({ room: portal.oppositeOf(currentRoom), previousIndex: null });
    });

    for (var i = 0; i < queue.length; i++) {
        var room = queue[i].room;
        if (targetRoom === room) {
            while (queue[i].previousIndex !== null) {
                i = queue[i].previousIndex;
            }
            this.go(queue[i].room);
            return;
        }
        room.portals.forEach(function(portal) {
            var nextRoom = portal.oppositeOf(room);
            var inQueue = false;
            for (var j = 0; j < queue.length; j++) {
                if (queue[j].room === nextRoom) {
                    inQueue = true;
                    break;
                }
            }
            if (!inQueue) {
                queue.push({ room: nextRoom, previousIndex: i });
            }
        });
    }
};

/**
 * Wields an item from the inventory.
 *
 * @param item The item to wield.
 */
Character.prototype.wield = function(item) {

    var inventory = this.inventory;
    if (!inventory.contains(item)) {
        return;
    }

    if (item.isWeapon()) {
        if (this.weapon === null) {
            this.send("You wield your %1.".arg(item.name));
            this.weapon = item;
        } else if (this.weapon.name === item.name) {
            this.send("You swap your %1 for another %2.".arg(this.weapon.name, item.name));
            inventory.append(this.weapon);
            this.weapon = item;
        } else {
            this.send("You remove your %1 and wield your %2.".arg(this.weapon.name, item.name));
            inventory.append(this.weapon);
            this.weapon = item;
        }
        inventory.removeOne(item);
        this.inventory = inventory;
    } else if (item.isShield()) {
        if (this.shield === null) {
            this.send("You wield your %1.".arg(item.name));
        } else {
            this.send("You remove your %1 and wield your %2.".arg(this.shield.name, item.name));
            inventory.append(this.shield);
        }
        this.shield = item;
        inventory.removeOne(item);
        this.inventory = inventory;
    } else {
        this.send("You cannot wield that.");
    }
};
