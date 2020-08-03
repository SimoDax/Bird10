APP_NAME = Bird10

CONFIG += qt warn_on cascades10
LIBS   += -lbbdata
LIBS += -lbbsystem
LIBS += -lbb -lbbcascadesdatamanager
LIBS += -lcURL -lcrypto

QT += network sql

include(config.pri)

