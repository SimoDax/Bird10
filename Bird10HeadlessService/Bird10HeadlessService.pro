APP_NAME = Bird10HeadlessService

CONFIG += qt warn_on cascades10

LIBS += -lbb -lbbsystem -lbbplatform -lbbdata
LIBS += -lcURL -lcrypto

QT += network

include(config.pri)