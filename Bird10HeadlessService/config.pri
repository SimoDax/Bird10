# Config.pri file version 2.0. Auto-generated by IDE. Any changes made by user will be lost!
BASEDIR = $$quote($$_PRO_FILE_PWD_)

device {
    CONFIG(debug, debug|release) {
        profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        } else {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }

    }

    CONFIG(release, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

simulator {
    CONFIG(debug, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

config_pri_source_group1 {
    SOURCES += \
        $$quote($$BASEDIR/src/DMApi.cpp) \
        $$quote($$BASEDIR/src/DMNotificator.cpp) \
        $$quote($$BASEDIR/src/NotificationsApi.cpp) \
        $$quote($$BASEDIR/src/Notificator.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonArray.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonDocument.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonObject.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonParseError.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonParser.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonValue.cpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonValueRef.cpp) \
        $$quote($$BASEDIR/src/QtCurl/CurlEasy.cpp) \
        $$quote($$BASEDIR/src/QtCurl/CurlMulti.cpp) \
        $$quote($$BASEDIR/src/TwitterApiBase.cpp) \
        $$quote($$BASEDIR/src/logger/logger.cpp) \
        $$quote($$BASEDIR/src/main.cpp) \
        $$quote($$BASEDIR/src/o2/o0baseauth.cpp) \
        $$quote($$BASEDIR/src/o2/o0settingsstore.cpp) \
        $$quote($$BASEDIR/src/o2/o1.cpp) \
        $$quote($$BASEDIR/src/o2/o1requestor.cpp) \
        $$quote($$BASEDIR/src/o2/o1timedreply.cpp) \
        $$quote($$BASEDIR/src/o2/o2reply.cpp) \
        $$quote($$BASEDIR/src/o2/o2replyserver.cpp) \
        $$quote($$BASEDIR/src/o2/o2simplecrypt.cpp) \
        $$quote($$BASEDIR/src/o2/oxtwitter.cpp) \
        $$quote($$BASEDIR/src/service.cpp)

    HEADERS += \
        $$quote($$BASEDIR/src/DMApi.hpp) \
        $$quote($$BASEDIR/src/DMNotificator.hpp) \
        $$quote($$BASEDIR/src/NotificationsApi.hpp) \
        $$quote($$BASEDIR/src/Notificator.hpp) \
        $$quote($$BASEDIR/src/QJson4/QJsonArray.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonDocument.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonObject.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonParseError.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonParser.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonRoot.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonValue.h) \
        $$quote($$BASEDIR/src/QJson4/QJsonValueRef.h) \
        $$quote($$BASEDIR/src/QtCurl/CurlEasy.h) \
        $$quote($$BASEDIR/src/QtCurl/CurlMulti.h) \
        $$quote($$BASEDIR/src/TwitterApiBase.hpp) \
        $$quote($$BASEDIR/src/logger/logger.h) \
        $$quote($$BASEDIR/src/o2/o0abstractstore.h) \
        $$quote($$BASEDIR/src/o2/o0baseauth.h) \
        $$quote($$BASEDIR/src/o2/o0export.h) \
        $$quote($$BASEDIR/src/o2/o0globals.h) \
        $$quote($$BASEDIR/src/o2/o0requestparameter.h) \
        $$quote($$BASEDIR/src/o2/o0settingsstore.h) \
        $$quote($$BASEDIR/src/o2/o0simplecrypt.h) \
        $$quote($$BASEDIR/src/o2/o1.h) \
        $$quote($$BASEDIR/src/o2/o1freshbooks.h) \
        $$quote($$BASEDIR/src/o2/o1requestor.h) \
        $$quote($$BASEDIR/src/o2/o1timedreply.h) \
        $$quote($$BASEDIR/src/o2/o1twitter.h) \
        $$quote($$BASEDIR/src/o2/o2reply.h) \
        $$quote($$BASEDIR/src/o2/o2replyserver.h) \
        $$quote($$BASEDIR/src/o2/oxtwitter.h) \
        $$quote($$BASEDIR/src/secret.h) \
        $$quote($$BASEDIR/src/service.hpp)
}

CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

lupdate_inclusion {
    SOURCES += \
        $$quote($$BASEDIR/../src/*.c) \
        $$quote($$BASEDIR/../src/*.c++) \
        $$quote($$BASEDIR/../src/*.cc) \
        $$quote($$BASEDIR/../src/*.cpp) \
        $$quote($$BASEDIR/../src/*.cxx) \
        $$quote($$BASEDIR/../src/QJson4/*.c) \
        $$quote($$BASEDIR/../src/QJson4/*.c++) \
        $$quote($$BASEDIR/../src/QJson4/*.cc) \
        $$quote($$BASEDIR/../src/QJson4/*.cpp) \
        $$quote($$BASEDIR/../src/QJson4/*.cxx) \
        $$quote($$BASEDIR/../src/QtCurl/*.c) \
        $$quote($$BASEDIR/../src/QtCurl/*.c++) \
        $$quote($$BASEDIR/../src/QtCurl/*.cc) \
        $$quote($$BASEDIR/../src/QtCurl/*.cpp) \
        $$quote($$BASEDIR/../src/QtCurl/*.cxx) \
        $$quote($$BASEDIR/../src/logger/*.c) \
        $$quote($$BASEDIR/../src/logger/*.c++) \
        $$quote($$BASEDIR/../src/logger/*.cc) \
        $$quote($$BASEDIR/../src/logger/*.cpp) \
        $$quote($$BASEDIR/../src/logger/*.cxx) \
        $$quote($$BASEDIR/../src/o2/*.c) \
        $$quote($$BASEDIR/../src/o2/*.c++) \
        $$quote($$BASEDIR/../src/o2/*.cc) \
        $$quote($$BASEDIR/../src/o2/*.cpp) \
        $$quote($$BASEDIR/../src/o2/*.cxx) \
        $$quote($$BASEDIR/../assets/*.qml) \
        $$quote($$BASEDIR/../assets/*.js) \
        $$quote($$BASEDIR/../assets/*.qs)

    HEADERS += \
        $$quote($$BASEDIR/../src/*.h) \
        $$quote($$BASEDIR/../src/*.h++) \
        $$quote($$BASEDIR/../src/*.hh) \
        $$quote($$BASEDIR/../src/*.hpp) \
        $$quote($$BASEDIR/../src/*.hxx)
}

TRANSLATIONS = $$quote($${TARGET}.ts)
