#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

// ADVobfuscator (legacy)
#include "MetaString.h"
#include "Log.h"

using namespace andrivet::ADVobfuscator;

int main()
{
    // 1. System call: change console code page (UTF-8)
    system(OBFUSCATED("chcp 65001 > nul"));

    // 2. System call: CMD launch message
    system(OBFUSCATED("cmd /c echo --- CMD launch completed ---"));

    // 3. Output author name (obfuscated string)
    std::cout << OBFUSCATED("Author: Pogosyan Nikolai") << std::endl;

    // 4. Path to Documents (parts are obfuscated)
    std::string path =
        std::string(OBFUSCATED("C:\\Users\\")) +
        getenv("USERNAME") +
        OBFUSCATED("\\Documents");

    std::cout << std::endl
        << OBFUSCATED("Folder content: ") << path << std::endl;

    // 5. List directory content
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            std::cout << " - " << entry.path().filename().string() << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << OBFUSCATED("Error: cannot read directory.") << std::endl;
    }

    std::cout << std::endl << OBFUSCATED("Done.") << std::endl;

    // 6. Pause (system call)
    system(OBFUSCATED("pause"));

    return 0;
}
