/*global define:false, require:false*/
define(["controller", "lib/laces"], function(Controller, Laces) {

    "use strict";

    function GameObject(model, object) {

        Laces.Model.call(this, object);

        this.model = model;

        var self = this;
        this.bind("change", function(event) {
            var propertyName = event.key;
            if (self.constructor.savedProperties.contains(propertyName)) {
                self.saveProperty(propertyName);
            }
        });
    }

    GameObject.prototype = new Laces.Model();
    GameObject.prototype.constructor = GameObject;

    GameObject.prototype.resolvePointer = function(pointer) {

        var pointerTypes = ["area", "room", "portal"];
        for (var i = 0, length = pointerTypes.length; i < length; i++) {
            var pointerType = pointerTypes[i];
            if (pointer.startsWith(pointerType + ":")) {
                var objectId = pointer.mid(pointerType.length + 1).toInt();
                return this.model[pointerType + "s"][objectId];
            }
        }
        return null;
    };

    GameObject.prototype.resolvePointers = function(propertyNames) {

        this.holdEvents();

        for (var i = 0; i < propertyNames.length; i++) {
            var propertyName = propertyNames[i];
            if (this.contains(propertyName)) {
                var value = this[propertyName];
                if (value instanceof Array) {
                    for (var j = 0, length = value.length; j < length; j++) {
                        value[j] = this.resolvePointer(value[j]);
                    }
                } else {
                    this[propertyName] = this.resolvePointer(value);
                }
            }
        }

        this.fireHeldEvents();
    };

    GameObject.prototype.saveProperty = function(propertyName) {

        var options = this.constructor.savedProperties[propertyName] || {};

        var value = this[propertyName];

        if (options.type === "pointer") {
            value = value.constructor.name.toLower() + ":" + value.id;
        } else if (options.type === "pointerlist") {
            var pointerList = [];
            value.forEach(function(pointer) {
                pointerList.append(value.constructor.name.toLower() + ":" + pointer.id);
            });
            value = "[" + pointerList.join(",") + "]";
        } else if (options.type === "point") {
            value = "(" + value[0] + "," + value[1] + "," + value[2] + ")";
        }

        Controller.sendApiCall("property-set " + this.id + " " + propertyName + " " + value);
    };


    function Area(model, jsonString) {

        var area = JSON.parse(jsonString);
        GameObject.call(this, model, area);
    }

    Area.savedProperties = {
        "description": { "type": "string" },
        "name": { "type": "string" },
        "rooms": { "type": "pointerlist" }
    };

    Area.prototype = new GameObject();
    Area.prototype.constructor = Area;


    function Room(model, jsonString) {

        var room = JSON.parse(jsonString);
        room.portals = room.portals || [];
        room.position = room.position || [0, 0, 0];
        GameObject.call(this, model, room);

        this.set("x", room.position[0], { "type": "integer" });
        this.set("y", room.position[1], { "type": "integer" });
        this.set("z", room.position[2], { "type": "integer" });
        this.set("position", function() { return [this.x, this.y, this.z]; });
    }

    Room.savedProperties = {
        "area": { "type": "pointer" },
        "description": { "type": "string" },
        "name": { "type": "string" },
        "portals": { "type": "pointerlist" },
        "position": { "type": "point" }
    };

    Room.prototype = new GameObject();
    Room.prototype.constructor = Room;


    function Portal(model, jsonString) {

        var portal = JSON.parse(jsonString);
        GameObject.call(this, model, portal);
    }

    Portal.savedProperties = {
        "description": { "type": "string" },
        "description2": { "type": "string" },
        "flags": { "type": "flags" },
        "name": { "type": "string" },
        "name2": { "type": "string" },
        "room": { "type": "pointer" },
        "room2": { "type": "pointer" }
    };

    Portal.prototype = new GameObject();
    Portal.prototype.constructor = Portal;


    function MapModel() {

        Laces.Model.call(this, {
            "areas": {},
            "portals": {},
            "rooms": {}
        });
    }

    MapModel.prototype = new Laces.Model();
    MapModel.prototype.constructor = MapModel;

    MapModel.prototype.load = function() {

        var self = this;
        self.holdEvents();

        Controller.sendApiCall("objects-list area", function(data) {
            for (var i = 0; i < data.length; i++) {
                var area = new Area(self, data[i]);
                self.areas.set(area.id, area);
            }

            Controller.sendApiCall("objects-list room", function(data) {
                for (var i = 0; i < data.length; i++) {
                    var room = new Room(self, data[i]);
                    self.rooms.set(room.id, room);
                }

                Controller.sendApiCall("objects-list portal", function(data) {
                    for (var i = 0; i < data.length; i++) {
                        var portal = new Portal(self, data[i]);
                        self.portals.set(portal.id, portal);
                    }

                    for (var id in self.rooms) {
                        if (self.rooms.hasOwnProperty(id)) {
                            self.rooms[id].resolvePointers(["portals"]);
                        }
                    }
                    for (id in self.portals) {
                        if (self.portals.hasOwnProperty(id)) {
                            self.portals[id].resolvePointers(["room", "room2"]);
                        }
                    }
                    for (id in self.areas) {
                        if (self.areas.hasOwnProperty(id)) {
                            self.areas[id].resolvePointers(["rooms"]);
                        }
                    }
                    for (id in self.rooms) {
                        if (self.rooms.hasOwnProperty(id)) {
                            self.rooms[id].resolvePointers(["area"]);
                        }
                    }

                    self.fireHeldEvents();
                });
            });
        });
    };

    MapModel.prototype.setPortal = function(portal) {

        var command = "portal-set " + portal.id + " " + portal.room + " " + portal.room2 +
                      " " + portal.name + " " + portal.name2 +
                      (portal.position ? " (" + portal.position.join(",") + ")" : "");

        var self = this;
        Controller.sendApiCall(command, function(data) {
            var portal = new Portal(self, data["portal"]);
            self.portals[portal.id] = portal;

            if (data.contains("source")) {
                var source = new Room(self, data["source"]);
                self.rooms[source.id] = source;
                source.resolvePointers(["portals"]);
            }

            if (data.contains("destination")) {
                var destination = new Room(self, data["destination"]);
                self.rooms[destination.id] = destination;
                destination.resolvePointers(["portals"]);
            }

            portal.resolvePointers(["room", "room2"]);
        });
    };

    MapModel.prototype.deletePortal = function(portalId) {

        var self = this;
        Controller.sendApiCall("portal-delete " + portalId, function() {
            var portal = self.portals[portalId];
            for (var roomId in self.rooms) {
                var room = self.rooms[roomId];
                room.portals.removeOne(portal);
            }
            delete self.portals[portalId];
        });
    };

    return MapModel;
});
