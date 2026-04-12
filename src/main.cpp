#include "vue/FenetrePrincipale.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    yalta::FenetrePrincipale fenetre;
    fenetre.show();
    return app.exec();
}
