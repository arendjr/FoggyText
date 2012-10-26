#ifndef GENERATECOMMAND_H
#define GENERATECOMMAND_H

#include "admincommand.h"


class Point;
class Room;


class GenerateEnvironmentCommand : public AdminCommand {

    Q_OBJECT

    public:
        GenerateEnvironmentCommand(QObject *parent = 0);
        virtual ~GenerateEnvironmentCommand();

        virtual void execute(Player *player, const QString &command);

    private:
        void generateScraperFloor(int level, const Point &center);
        Room *createRoomAt(const Point &position);
};

#endif // GENERATECOMMAND_H
