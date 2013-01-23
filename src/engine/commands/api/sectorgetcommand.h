#ifndef SECTORGETCOMMAND_H
#define SECTORGETCOMMAND_H

#include "apicommand.h"


class SectorGetCommand : public ApiCommand {

    public:
        SectorGetCommand(QObject *parent = 0);
        virtual ~SectorGetCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // SECTORGETCOMMAND_H
