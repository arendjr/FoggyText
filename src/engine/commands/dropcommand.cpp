#include "dropcommand.h"


#define super Command

DropCommand::DropCommand(QObject *parent) :
    super(parent) {

    setDescription("Drop an item from your inventory.\n"
                   "\n"
                   "Example: drop stick");
}

DropCommand::~DropCommand() {
}

void DropCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList items = takeObjects(player->inventory());
    if (!requireSome(items, "Drop what?")) {
        return;
    }

    for (const GameObjectPtr &item : items) {
        currentRoom()->addItem(item);
        player->removeInventoryItem(item);
    }

    QString description = items.joinFancy(DefiniteArticles);

    send("You drop %2.", description);

    GameObjectPtrList others = currentRoom()->players();
    others.removeOne(player);
    others.send(QString("%1 drops %2.").arg(player->name(), description));
}
