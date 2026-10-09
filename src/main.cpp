
#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QDir>

#include "monitor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Linux System Monitor");
    window.resize(1440, 900);

    auto *view = new QWebEngineView(&window);

    auto *monitor = new Monitor(&window);
    auto *channel = new QWebChannel(view->page());

    channel->registerObject("monitor", monitor);
    view->page()->setWebChannel(channel);

    const QString htmlPath =
        QDir::current().filePath("ui/index.html");

    view->load(QUrl::fromLocalFile(htmlPath));

    window.setCentralWidget(view);
    window.show();

    return app.exec();
}
