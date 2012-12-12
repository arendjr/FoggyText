function DropCommand() {
    Command.call(this);

    this.setDescription("Drop an item from your inventory.\n" +
                        "\n" +
                        "Example: drop stick");
}

DropCommand.prototype = new Command();
DropCommand.prototype.constructor = DropCommand;

DropCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var items = this.takeObjects(player.inventory);
    if (!this.requireSome(items, "Drop what?")) {
        return;
    }

    items.forEach(function(item) {
        player.currentRoom.addItem(item);
        player.removeInventoryItem(item);
    });

    var description = items.joinFancy(Options.DefiniteArticles);

    this.send("You drop %2.", description);

    var others = this.currentRoom.characters;
    others.removeOne(player);
    others.send("%1 drops %2.".arg(player.name, description));
}

CommandRegistry.registerCommand("drop", new DropCommand());
