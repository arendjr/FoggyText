#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QVariantList>

#include "gameobjectptr.h"
#include "scriptfunction.h"


class Area;
class Player;

class ScriptEngine : public QObject {

    Q_OBJECT

    public:
        ScriptEngine();
        virtual ~ScriptEngine();

        static ScriptEngine *instance();

        QScriptValue evaluate(const QString &program,
                              const QString &fileName = QString(), int lineNumber = 1);
        ScriptFunction defineFunction(const QString &program,
                                      const QString &fileName = QString(), int lineNumber = 1);

        bool hasUncaughtException() const;
        QScriptValue uncaughtException() const;

        QScriptValue executeFunction(ScriptFunction &function, const GameObjectPtr &thisObject,
                                     const QScriptValueList &arguments);

        QScriptValue toScriptValue(GameObject *object);
        QScriptValue toScriptValue(const GameObjectPtr &object);
        QScriptValue toScriptValue(const GameObjectPtrList &list);

        void setGlobalObject(const char *name, QObject *object);
        void unsetGlobalObject(const char *name);

        static const QMap<QString, QString> &triggers();

    private:
        QScriptEngine m_jsEngine;
};

#endif // SCRIPTENGINE_H
