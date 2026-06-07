#include <iostream>
#include "Engine/Debug/ErrorDialog.h"

#include "Engine/Core/System.h"

int main() {

    System app;

    try {
        app.Run();
    } catch (const std::exception &e) {
        ShowErrorDialog(EscapeMarkup(e.what()));
        //std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;

}