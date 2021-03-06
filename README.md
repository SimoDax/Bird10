## Bird10 for BlackBerry

App structure: Qml (Bird10/assets) for ui and app flow, C++ (Bird10/src) for fetching, parsing & data storage.
The code for the background service has its own project folder, Bird10HeadlessService

Qml also holds and instantiates the C++ classes, which are exposed as objects in the meta-object system.
Every web request to Twitter apis is handled by libcurl because of tls requirements, QNetworkAccessManager in qt4.8 is too outdated 

External libs:
- **QJson4**: A simple library to have a json parser like the one built in qt5
- **QtCurl**: Qt wrapper around libcurl. Heavily customised wrt to the original project to make it work on OS10
- **o2**: C++ OAuth library for Qt. Customised to work with Twitter private apis and with QtCurl instead of QNetworkRequest

I tried to keep every app functionality in its own class. It does not aim to be a perfectly classful code like the ones Java programmers love to write, instead it's a tradeoff between a tidy codebase and writing/mantaining/debugging speed.
Two classes are abstract:
- **TwitterApiBase**: every class that should interface with the api should derive from this one, since it already has an authenticator and a requestor (which is a factory for o2-authenticated, QtCurl-based web requests)
- **TimelineBase**: every class that handles a timeline (or more broadly a list of tweets) should derive from this one (which already derives from TwitterApiBase), since it has all the code to parse them. It does not have a container for them however.
- Moreover, if a class with a container and retweet / like capabailities is needed, extending TwitterApi (which already derives from TimelineBase) is a good choice

Side notes:

Some (few) of the Qml code is ripped off from the official app as I wanted a similar style :)

All the o1Twitter references in the Qml actually refer to the OXTwitter object (because it is registered in that way in the meta-object system), not to O1Twitter. Originally the app used OAuth 1.0 instead of XAuth and I was too lazy to change all the names, so I just changed the actual class getting instantiated when I switched to XAuth.
