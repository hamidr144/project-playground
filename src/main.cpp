#include "app/headers/Application.h"

int main(int argc, char* argv[])
{
    (void)argc;

    return project_playground::app::Application::GetInstance().Run(argv[0]);
}