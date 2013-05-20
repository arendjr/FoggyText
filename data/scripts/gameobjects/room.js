
function Room() {
}

Room.prototype.lookAtBy = function(character) {

    var text = "", self = this;

    if (!this.name.isEmpty()) {
        text += "\n" + this.name.colorized(Color.Teal) + "\n\n";
    }

    var flags = this.flags.split("|");
    var hasDynamicPortals = !flags.contains("OmitDynamicPortalsFromDescription");
    var hasDistantCharacters = !flags.contains("OmitDistantCharactersFromDescription");

    var itemGroups = VisualUtil.divideItemsIntoGroups(this.items, character.direction);

    var portalGroups;
    if (hasDynamicPortals || hasDistantCharacters) {
        portalGroups = VisualUtil.dividePortalsAndCharactersIntoGroups(character, this);
    }

    var itemTexts = [];
    for (var key in itemGroups) {
        if (!itemGroups[key].isEmpty() || hasDynamicPortals && !portalGroups[key].isEmpty()) {
            var itemGroup = itemGroups[key];
            var plural = itemGroup.firstItemIsPlural();

            var combinedItems = Util.combinePtrList(itemGroup);

            if (hasDynamicPortals) {
                portalGroups[key].forEach(function(portal) {
                    combinedItems.append(portal.nameWithDestinationFromRoom(self));
                });
            }

            var firstItem = itemGroup[0];
            var presenceVerb = (firstItem ? firstItem.presenceVerb : "be");

            var prefix = VisualUtil.prefixForGroup(key);
            var verb = VERBS[presenceVerb][plural ? "plural" : "thirdPerson"];
            itemTexts.append("%1 %2 %3.".arg(prefix, verb, Util.joinFancy(combinedItems))
                             .replace("there is", "there's"));
        }
    }

    var characterText = "";
    if (hasDistantCharacters && portalGroups.hasOwnProperty("characters")) {
        var characters = portalGroups["characters"];
        characterText = VisualUtil.describeCharactersRelativeTo(characters, character);
    }

    text += this.description;
    if (!itemTexts.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        for (var i = 0; i < itemTexts.length; i++) {
            var itemText = itemTexts[i];
            if (i > 1 && i === itemTexts.length - 1) {
                itemText = "and " + itemText;
            }
            itemTexts[i] = itemText.capitalized();
        }
        text += itemTexts.join(" ");
    }
    if (!characterText.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        text += characterText;
    }
    text += "\n";

    var exitNames = [];
    this.portals.forEach(function(portal) {
        if (!portal.isHiddenFromRoom(self)) {
            exitNames.append(portal.nameFromRoom(self));
        }
    });
    if (!exitNames.isEmpty()) {
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

Room.prototype.portalNamed = function(name) {

    for (var i = 0, length = this.portals.length; i < length; i++) {
        var portal = this.portals[i];
        if (portal.nameFromRoom(this) === name) {
            return portal;
        }
    }
    return undefined;
};
