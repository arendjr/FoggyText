#include "inventorycommand.h"

#include "util.h"


#define super Command

InventoryCommand::InventoryCommand(QObject *parent) :
    super(parent) {

    setDescription("View your current inventory.\n"
                   "\n"
                   "Example: inventory");
}

InventoryCommand::~InventoryCommand() {
}

void InventoryCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString carriedInventoryString;
    if (player->inventory().isEmpty()) {
        carriedInventoryString = "You don't carry anything.\n";
    } else if (player->inventory().length() == 1) {
        carriedInventoryString = QString("You carry %1, weighing %2.\n")
                                 .arg(player->inventory().joinFancy(),
                                      Util::formatWeight(player->inventoryWeight()));
    } else {
        carriedInventoryString = QString("You carry %1, weighing a total of %2.\n")
                                 .arg(player->inventory().joinFancy(),
                                      Util::formatWeight(player->inventoryWeight()));
    }

    send(carriedInventoryString);
}
