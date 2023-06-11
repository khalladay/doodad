#include "common_header.h"
#include "doodad_app.h"
#include "win_init.h"
#include "input_handler.h"
#include <stdio.h>
#include <shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")


/*
calculator - ((2^10)*2) / 400000
bitwise calculator - ((194 & 930) | 0xFF) ^ 
radix conversion - 16 0xF 0b1111
ascii values - 'C'

dot product of two vectors (1,2,3) . (2,3,4)
normalize a vector normalize(1,2,3)
length of a vector length(1,2,3)

should spawn the window wherever your cursor is on screen. 
*/

/*
std::vector<std::string> split(const std::string& source, const std::string& delimiters) 
{
    uint32_t iter = 0;

    std::vector<char> delims;
    for (uint32_t i = 0; i < delimiters.length(); ++i)
    {
        delims.push_back( delimiters[i] );
    }

    std::vector<char> whitespace;
    whitespace.push_back(' ');
    whitespace.push_back('\n');
    whitespace.push_back('\r\n');
    whitespace.push_back('\t');

    auto IsDelimeter = [](char c, std::vector<char> delimVec) -> bool
    {
        return std::find(delimVec.begin(), delimVec.end(), c) != delimVec.end();
    };

    auto IsWhitespace = [whitespace](char c) -> bool
    {
        return std::find(whitespace.begin(), whitespace.end(), c) != whitespace.end();
    };

    std::vector<std::string> tokens;
    std::string word;

    while (iter < source.length())
    {
        char c = source[iter];
        if (IsDelimeter(c, delims))
        {
            if (word.length() > 0) tokens.push_back(word);
            word = c;
            tokens.push_back(word);
            word = "";
        }
        else if (!IsWhitespace(c))
        {
            word.push_back(c);
        }
        iter++;
    }
    tokens.push_back(word);

    return tokens;
}

std::string ProcessString(std::string str)
{
    std::vector<std::string> tokens = split(str, "+-()/^*");
    std::string output;
    for (std::string s : tokens) { output += s; output += "\n"; }
    return output;
}

*/



void mainLoop();

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    HWND wndHdl = os::makeWindow(hInstance, TEXT(APP_NAME), DEFAULT_SCREEN_W, DEFAULT_SCREEN_H);

    char path[512];
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleFileNameA(hModule, path, 512);
    PathRemoveFileSpecA(path);

    App().Initialize(wndHdl, hInstance, path);

    mainLoop();

    return 0;

}

void mainLoop()
{
   App().Tick();
   App().Draw();

    while (1)
    {
        os::handleEvents();
        App().Tick();
        App().Draw();

        Sleep(16);
        if (App().ShouldExit())
        {
            break;
        }
    }
}
