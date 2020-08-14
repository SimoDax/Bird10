APP_NAME = Bird10HeadlessService

CONFIG += qt warn_on

include(config.pri)

LIBS += -lbb -lbbsystem -lbbplatform -lbbdata
LIBS += -lcURL -lcrypto

QT += network
