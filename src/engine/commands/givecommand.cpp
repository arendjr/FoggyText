#include "givecommand.h"


#define super Command

GiveCommand::GiveCommand(QObject *parent) :
    super(parent) {

    setDescription("Give an item from your inventory to another character.\n"
                   "\n"
                   "Examples: give stick earl, give $5 to beggar");
}

GiveCommand::~GiveCommand() {
}

void GiveCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Give what?")) {
        return;
    }

    GameObjectPtrList items = takeObjects(player->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    takeWord("to", IfNotLast);

    GameObjectPtrList recipients = takeObjects(currentRoom()->characters());
    if (!requireSome(recipients, "That recipient is not here.")) {
        return;
    }

    Character *recipient = recipients[0].cast<Character *>();
    QString recipientName = recipient->definiteName(currentRoom()->characters());

    GameObjectPtrList givenItems;
    for (const GameObjectPtr &itemPtr : items) {
        Item *item = itemPtr.cast<Item *>();
        if (recipient->inventoryWeight() + item->weight() > recipient->maxInventoryWeight()) {
            send("%1 is too heavy for %2.",
                 item->definiteName(player->inventory(), Capitalized), recipientName);
        } else if (!recipient->invokeTrigger("onreceive", player, itemPtr)) {
            continue;
        } else {
            recipient->addInventoryItem(itemPtr);
            givenItems.append(itemPtr);
        }
    }

    QString description = givenItems.joinFancy();

    for (const GameObjectPtr &itemPtr : givenItems) {
        player->removeInventoryItem(itemPtr);
    }

    send("You give %1 to %2.", description, recipientName);
    recipient->send(QString("%1 gives you %2.").arg(player->name(), description));

    GameObjectPtrList others = currentRoom()->players();
    others.removeOne(player);
    others.removeOne(recipient);
    others.send(QString("%1 gives %2 to %3.").arg(player->name(), description, recipientName));
}
