include(environment.pri)

LIBS += -lz

TARGET = FoggyText

TEMPLATE = app

SOURCES += \
    main.cpp \
    engine/application.cpp \
    engine/characterstats.cpp \
    engine/commandinterpreter.cpp \
    engine/commandregistry.cpp \
    engine/conversionutil.cpp \
    engine/diskutil.cpp \
    engine/effect.cpp \
    engine/engine.cpp \
    engine/gameeventmultipliermap.cpp \
    engine/gameexception.cpp \
    engine/gameobjectptr.cpp \
    engine/gameobjectsyncthread.cpp \
    engine/gamethread.cpp \
    engine/logthread.cpp \
    engine/logutil.cpp \
    engine/metatyperegistry.cpp \
    engine/modifier.cpp \
    engine/point3d.cpp \
    engine/scriptengine.cpp \
    engine/scriptfunction.cpp \
    engine/scriptfunctionmap.cpp \
    engine/session.cpp \
    engine/triggerregistry.cpp \
    engine/util.cpp \
    engine/vector3d.cpp \
    engine/commands/command.cpp \
    engine/commands/gocommand.cpp \
    engine/commands/scriptcommand.cpp \
    engine/commands/admin/addcharactercommand.cpp \
    engine/commands/admin/addcontainercommand.cpp \
    engine/commands/admin/additemcommand.cpp \
    engine/commands/admin/addportalcommand.cpp \
    engine/commands/admin/addshieldcommand.cpp \
    engine/commands/admin/addweaponcommand.cpp \
    engine/commands/admin/admincommand.cpp \
    engine/commands/admin/copyitemcommand.cpp \
    engine/commands/admin/copytriggerscommand.cpp \
    engine/commands/admin/execscriptcommand.cpp \
    engine/commands/admin/getpropcommand.cpp \
    engine/commands/admin/gettriggercommand.cpp \
    engine/commands/admin/listmethodscommand.cpp \
    engine/commands/admin/listpropscommand.cpp \
    engine/commands/admin/reloadscriptscommand.cpp \
    engine/commands/admin/removeexitcommand.cpp \
    engine/commands/admin/removeitemcommand.cpp \
    engine/commands/admin/setclasscommand.cpp \
    engine/commands/admin/setpropcommand.cpp \
    engine/commands/admin/setracecommand.cpp \
    engine/commands/admin/settriggercommand.cpp \
    engine/commands/admin/stopservercommand.cpp \
    engine/commands/admin/unsettriggercommand.cpp \
    engine/commands/api/apicommand.cpp \
    engine/commands/api/datagetcommand.cpp \
    engine/commands/api/datasetcommand.cpp \
    engine/commands/api/logretrievecommand.cpp \
    engine/commands/api/objectdeletecommand.cpp \
    engine/commands/api/objectslistcommand.cpp \
    engine/commands/api/portalsetcommand.cpp \
    engine/commands/api/propertygetcommand.cpp \
    engine/commands/api/propertysetcommand.cpp \
    engine/commands/api/triggergetcommand.cpp \
    engine/commands/api/triggersetcommand.cpp \
    engine/commands/api/triggerslistcommand.cpp \
    engine/commands/generator/generateenvironmentcommand.cpp \
    engine/events/asyncreplyevent.cpp \
    engine/events/commandevent.cpp \
    engine/events/deleteobjectevent.cpp \
    engine/events/event.cpp \
    engine/events/signinevent.cpp \
    engine/events/timerevent.cpp \
    engine/gameevents/areaevent.cpp \
    engine/gameevents/gameevent.cpp \
    engine/gameevents/movementsoundevent.cpp \
    engine/gameevents/movementvisualevent.cpp \
    engine/gameevents/soundevent.cpp \
    engine/gameevents/speechevent.cpp \
    engine/gameevents/visualevent.cpp \
    engine/gameobjects/area.cpp \
    engine/gameobjects/character.cpp \
    engine/gameobjects/class.cpp \
    engine/gameobjects/container.cpp \
    engine/gameobjects/exit.cpp \
    engine/gameobjects/gameeventobject.cpp \
    engine/gameobjects/gameobject.cpp \
    engine/gameobjects/group.cpp \
    engine/gameobjects/item.cpp \
    engine/gameobjects/player.cpp \
    engine/gameobjects/portal.cpp \
    engine/gameobjects/race.cpp \
    engine/gameobjects/realm.cpp \
    engine/gameobjects/room.cpp \
    engine/gameobjects/shield.cpp \
    engine/gameobjects/statsitem.cpp \
    engine/gameobjects/weapon.cpp \
    engine/logmessages/commandlogmessage.cpp \
    engine/logmessages/logmessage.cpp \
    engine/logmessages/npctalklogmessage.cpp \
    engine/logmessages/playerdeathstatslogmessage.cpp \
    engine/logmessages/retrievestatslogmessage.cpp \
    engine/logmessages/roomvisitstatslogmessage.cpp \
    engine/logmessages/sessionlogmessage.cpp \
    interface/httpserver.cpp \
    interface/telnetserver.cpp \
    interface/websocketserver.cpp \
    ../3rdparty/qjson/json_driver.cpp \
    ../3rdparty/qjson/json_parser.cpp \
    ../3rdparty/qjson/json_scanner.cpp \
    ../3rdparty/qtiocompressor/qtiocompressor.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.cpp \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.cpp \

HEADERS += \
    engine/application.h \
    engine/characterstats.h \
    engine/commandinterpreter.h \
    engine/commandregistry.h \
    engine/constants.h \
    engine/conversionutil.h \
    engine/diskutil.h \
    engine/effect.h \
    engine/engine.h \
    engine/foreach.h \
    engine/gameeventmultipliermap.h \
    engine/gameexception.h \
    engine/gameobjectptr.h \
    engine/gameobjectsyncthread.h \
    engine/gamethread.h \
    engine/logthread.h \
    engine/logutil.h \
    engine/metatyperegistry.h \
    engine/modifier.h \
    engine/point3d.h \
    engine/scriptengine.h \
    engine/scriptfunction.h \
    engine/scriptfunctionmap.h \
    engine/session.h \
    engine/triggerregistry.h \
    engine/util.h \
    engine/vector3d.h \
    engine/commands/command.h \
    engine/commands/gocommand.h \
    engine/commands/scriptcommand.h \
    engine/commands/admin/addcharactercommand.h \
    engine/commands/admin/addcontainercommand.h \
    engine/commands/admin/additemcommand.h \
    engine/commands/admin/addportalcommand.h \
    engine/commands/admin/addshieldcommand.h \
    engine/commands/admin/addweaponcommand.h \
    engine/commands/admin/admincommand.h \
    engine/commands/admin/copyitemcommand.h \
    engine/commands/admin/copytriggerscommand.h \
    engine/commands/admin/execscriptcommand.h \
    engine/commands/admin/getpropcommand.h \
    engine/commands/admin/gettriggercommand.h \
    engine/commands/admin/listmethodscommand.h \
    engine/commands/admin/listpropscommand.h \
    engine/commands/admin/reloadscriptscommand.h \
    engine/commands/admin/removeexitcommand.h \
    engine/commands/admin/removeitemcommand.h \
    engine/commands/admin/setclasscommand.h \
    engine/commands/admin/setpropcommand.h \
    engine/commands/admin/setracecommand.h \
    engine/commands/admin/settriggercommand.h \
    engine/commands/admin/stopservercommand.h \
    engine/commands/admin/unsettriggercommand.h \
    engine/commands/api/apicommand.h \
    engine/commands/api/datagetcommand.h \
    engine/commands/api/datasetcommand.h \
    engine/commands/api/logretrievecommand.h \
    engine/commands/api/objectdeletecommand.h \
    engine/commands/api/objectslistcommand.h \
    engine/commands/api/portalsetcommand.h \
    engine/commands/api/propertygetcommand.h \
    engine/commands/api/propertysetcommand.h \
    engine/commands/api/triggergetcommand.h \
    engine/commands/api/triggersetcommand.h \
    engine/commands/api/triggerslistcommand.h \
    engine/commands/generator/generateenvironmentcommand.h \
    engine/events/asyncreplyevent.h \
    engine/events/commandevent.h \
    engine/events/deleteobjectevent.h \
    engine/events/event.h \
    engine/events/signinevent.h \
    engine/events/timerevent.h \
    engine/gameevents/areaevent.h \
    engine/gameevents/gameevent.h \
    engine/gameevents/movementsoundevent.h \
    engine/gameevents/movementvisualevent.h \
    engine/gameevents/soundevent.h \
    engine/gameevents/speechevent.h \
    engine/gameevents/visualevent.h \
    engine/gameobjects/area.h \
    engine/gameobjects/character.h \
    engine/gameobjects/class.h \
    engine/gameobjects/container.h \
    engine/gameobjects/exit.h \
    engine/gameobjects/gameeventobject.h \
    engine/gameobjects/gameobject.h \
    engine/gameobjects/group.h \
    engine/gameobjects/item.h \
    engine/gameobjects/player.h \
    engine/gameobjects/portal.h \
    engine/gameobjects/race.h \
    engine/gameobjects/realm.h \
    engine/gameobjects/room.h \
    engine/gameobjects/shield.h \
    engine/gameobjects/statsitem.h \
    engine/gameobjects/weapon.h \
    engine/logmessages/commandlogmessage.h \
    engine/logmessages/logmessage.h \
    engine/logmessages/npctalklogmessage.h \
    engine/logmessages/playerdeathstatslogmessage.h \
    engine/logmessages/retrievestatslogmessage.h \
    engine/logmessages/roomvisitstatslogmessage.h \
    engine/logmessages/sessionlogmessage.h \
    interface/httpserver.h \
    interface/telnetserver.h \
    interface/websocketserver.h \
    ../3rdparty/qjson/json_parser.hh \
    ../3rdparty/qjson/json_driver.hh \
    ../3rdparty/qjson/json_scanner.h \
    ../3rdparty/qtiocompressor/qtiocompressor.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsServer.h \
    ../3rdparty/qtwebsocket/QtWebSocket/QWsSocket.h \

OTHER_FILES += \
    engine/util.js \
    engine/commands/command.js \
    ../data/commands/buycommand.js \
    ../data/commands/closecommand.js \
    ../data/commands/descriptioncommand.js \
    ../data/commands/disbandcommand.js \
    ../data/commands/dropcommand.js \
    ../data/commands/drinkcommand.js \
    ../data/commands/eatcommand.js \
    ../data/commands/equipmentcommand.js \
    ../data/commands/followcommand.js \
    ../data/commands/getcommand.js \
    ../data/commands/givecommand.js \
    ../data/commands/groupcommand.js \
    ../data/commands/gtalkcommand.js \
    ../data/commands/helpcommand.js \
    ../data/commands/inventorycommand.js \
    ../data/commands/killcommand.js \
    ../data/commands/lookcommand.js \
    ../data/commands/losecommand.js \
    ../data/commands/opencommand.js \
    ../data/commands/putcommand.js \
    ../data/commands/quitcommand.js \
    ../data/commands/removecommand.js \
    ../data/commands/replycommand.js \
    ../data/commands/saycommand.js \
    ../data/commands/searchcommand.js \
    ../data/commands/shoutcommand.js \
    ../data/commands/slashmecommand.js \
    ../data/commands/statscommand.js \
    ../data/commands/talkcommand.js \
    ../data/commands/tellcommand.js \
    ../data/commands/usecommand.js \
    ../data/commands/wieldcommand.js \
    ../data/commands/whocommand.js \
    ../data/commands/admin/enterroomcommand.js \
    ../data/scripts/combat.js \
    ../data/scripts/sessionhandler.js \
    ../data/scripts/stats.js \
    ../data/scripts/gameobjects/character.js \
    ../data/scripts/gameobjects/container.js \
    ../data/scripts/gameobjects/gameobject.js \
    ../data/scripts/gameobjects/item.js \
    ../data/scripts/gameobjects/player.js \
    ../data/scripts/gameobjects/portal.js \
    ../data/scripts/gameobjects/room.js \
    ../data/scripts/gameobjects/statsitem.js \
    ../web/grunt.js \
    ../web/index.html \
    ../web/css/admin.css \
    ../web/css/main.css \
    ../web/js/controller.js \
    ../web/js/fabric.js \
    ../web/js/main.js \
    ../web/js/notifications.js \
    ../web/js/require.js \
    ../web/js/util.js \
    ../web/js/zepto.js \
    ../web/js/admin/admin.js \
    ../web/js/admin/mapeditor.js \
    ../web/js/admin/map.model.js \
    ../web/js/admin/map.view.js \
    ../web/js/admin/portaldeletedialog.js \
    ../web/js/admin/portaleditor.js \
    ../web/js/admin/propertyeditor.js \
    ../web/js/admin/slider.widget.js \
    ../web/js/codemirror/codemirror.css \
    ../web/js/codemirror/codemirror.js \
    ../web/js/codemirror/javascript.js \
    ../web/js/codemirror/util/javascript-hint.js \
    ../web/js/codemirror/util/simple-hint.js \
    ../DESIGN.txt \
    ../README.md \
    ../TODO.txt \

INCLUDEPATH += \
    engine \
    engine/commands \
    engine/commands/admin \
    engine/commands/api \
    engine/commands/generator \
    engine/events \
    engine/gameevents \
    engine/gameobjects \
    engine/logmessages \
    $$PWD/../3rdparty \
    $$PWD/../3rdparty/qtiocompressor \
    $$PWD/../3rdparty/qtwebsocket/QtWebSocket

RESOURCES += \
    engine/resources.qrc
