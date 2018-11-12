#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include "winnls.h"

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>

#include "EXTERN.h"
#include "perl.h"

#ifdef __GNUC__

/* Mingw32 defaults to globing command line 
 * This is inconsistent with other Win32 ports and 
 * seems to cause trouble with passing -DXSVERSION=\"1.6\" 
 * So we turn it off like this, but only when compiling
 * perlmain.c: perlmainst.c is linked into the same executable
 * as win32.c, which also does this, so we mustn't do it twice
 * otherwise we get a multiple definition error.
 */
#ifndef PERLDLL
int _CRT_glob = 0;
#endif

#endif

static void
error_exit(const wchar_t *msg)
{
    int err = GetLastError();
    _setmode(_fileno(stderr), _O_U16TEXT);
    fwprintf(stderr, L"%s: %d\n", msg, err);
    exit( err );
}

static char *
utf8_encode_wstring(const wchar_t *src)
{
    char *encoded;
    int len;

    len = WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, src,
            -1, NULL, 0, NULL, NULL);

    encoded = malloc(len + 1);
    if (!encoded) {
        error_exit(L"Failed to allocate memory for UTF-8 encoded string");
    }

    (void) WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, src,
            -1, encoded, len, NULL, NULL);

    return encoded;
}

static void
utf8_encode_warr(const wchar_t **warr, const int n, char **arr)
{
    int i;

    for (i = 0; i < n; ++i) {
        arr[i] = utf8_encode_wstring(warr[i]);
    }

    return;
}

int
wmain(int argc, wchar_t **wargv, wchar_t **wenv)
{
    char **argv;
    char **env;
    int env_count;

#ifdef _MSC_VER
    /* Arrange for _CrtDumpMemoryLeaks() to be called automatically at program
     * termination when built with CFG = DebugFull. */
    int currentFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    currentFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(currentFlag);

    /* Change this -1 to the allocation number of any reported memory leaks to
     * break on the allocation call that was leaked. */
    _CrtSetBreakAlloc(-1L);
#endif

    ++argc; /* we are going to insert -CA between argv[0] and argv[1] */
    argv = malloc((argc + 1) * sizeof(*argv));
    if (!argv) {
        error_exit(L"Failed to allocate memory of UTF-8 encoded argv");
    }

    argv[0] = utf8_encode_wstring(wargv[0]);
    argv[1] = "-CA";
    argv[ argc ] = NULL;

    utf8_encode_warr(wargv + 1, argc - 1, argv + 2);

    env_count = 0;
    while ( wenv[env_count] ) {
        ++env_count;
    }
    env = malloc( (env_count + 1) * sizeof(*env));
    if (!env) {
        error_exit(L"Failed to allocate memory for UTF-8 encoded environment");
    }
    env[ env_count ] = NULL;

    utf8_encode_warr(wenv, env_count, env);

    return RunPerl(argc, argv, env);
}


