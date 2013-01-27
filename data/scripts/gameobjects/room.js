
function Room() {
}

Room.prototype.lookAtBy = function(character) {

    var text = "";

    if (!this.name.isEmpty()) {
        text += "\n" + this.name.colorized(Color.Teal) + "\n\n";
    }

    text += this.description + "\n";

    if (!this.portals.isEmpty()) {
        var exitNames = [];
        for (var i = 0, length = this.portals.length; i < length; i++) {
            var portal = this.portals[i];

            if (portal.isHiddenFromRoom(this)) {
                continue;
            }

            exitNames.append(portal.nameFromRoom(this));
        }
        exitNames = Util.sortExitNames(exitNames);
        text += ("Obvious exits: " + exitNames.join(", ") + ".").colorized(Color.Green) + "\n";
    }

    var others = this.characters;
    others.removeOne(character);
    if (!others.isEmpty()) {
        text += "You see %1.\n".arg(others.joinFancy());
    }

    return text;
};
