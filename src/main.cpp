#include "app/headers/Application.h"

int main(int argc, char* argv[])
{
    (void)argc;

    project_playground::app::Application app;
    return app.Run(argv[0]);
}