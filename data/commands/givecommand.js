function GiveCommand() {
    Command.call(this);

    this.setDescription("Give an item from your inventory to another character.\n" +
                        "\n" +
                        "Examples: give stick to earl");
}

GiveCommand.prototype = new Command();
GiveCommand.prototype.constructor = GiveCommand;

GiveCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Give what?")) {
        return;
    }

    var items = this.takeObjects(player.inventory);
    if (!this.requireSome(items, "You don't have that.")) {
        return;
    }

    this.takeWord("to", Options.IfNotLast);

    var recipient = this.takeObject(this.currentRoom.characters);
    if (!this.requireSome(recipient, "That recipient is not here.")) {
        return;
    }

    var recipientName = recipient.definiteName(this.currentRoom.characters);

    var givenItems = [];
    for (var i = 0, length = items.length; i < length; i++) {
        var item = items[i];
        if (recipient.inventoryWeight() + item.weight > recipient.maxInventoryWeight()) {
            this.send("%1 is too heavy for %2.",
                      item.definiteName(player.inventory, Options.Capitalized), recipientName);
        } else if (!recipient.invokeTrigger("onreceive", player, item)) {
            continue;
        } else {
            recipient.addInventoryItem(item);
            player.removeInventoryItem(item);
            givenItems.append(item);
        }
    }

    var description = givenItems.joinFancy();

    this.send("You give %1 to %2.", description, recipientName);
    recipient.send("%1 gives you %2.".arg(player.name, description));

    var others = this.currentRoom.characters;
    others.removeOne(player);
    others.removeOne(recipient);
    others.send("%1 gives %2 to %3.".arg(player.name, description, recipientName));
};

CommandRegistry.registerCommand("give", new GiveCommand());
