#pragma once

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

extern SOF::Game *SOF::CreateGame(int argc, char **argv);

int main(int argc, char **argv)
{
    SOF::Log::Init();
    SOF_WARN("Entrypoint", "Log initialized!");
    auto app = SOF::CreateGame(argc, argv);
    app->Start();
    delete app;
    SOF::Log::Shutdown();
}

// This code is halal certified

#else
#error This engine currently only supports Windows and Linux :(

#endif