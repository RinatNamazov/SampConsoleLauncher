/*****************************************************************************
 *
 *  PROJECT:        SampConsoleLauncher
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           main.c
 *  DESCRIPTION:    Main
 *  COPYRIGHT:      (c) 2023 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#include "main.h"

#include "utils.h"

int main(int argc, char* argv[]) {
    struct command_line_args args = {0};

    if (!parse_cmd_args(argc, argv, &args)) {
        return 1;
    }

    if (!if_necessary_get_game_path_and_nickname_in_registry(args.game_path, args.nickname)) {
        return 2;
    }

    if (!launch_game(&args)) {
        return 3;
    }

    return 0;
}

void show_usage(const char* name) {
    fprintf(stderr,
            "SampConsoleLauncher v.1.0.0 "
            "<https://github.com/RinatNamazov/SampConsoleLauncher>\n"
            "Developer: Rinat Namazov\n"
            "Copyright (c) 2023 RINWARES <rinwares.com>\n"
            "Usage: %s"
            "\n--host (-h) 127.0.0.1 (required)"
            "\n--port (-p) 7777 (required)"
            "\n--nickname (-n) Rinat_Namazov (optional)"
            "\n--password (-z) strong1pass (optional)"
            "\n--game-path (-g) \"D:\\Games\\GTA_SA\\gta_sa.exe\" (optional)"
            "\n\nIf the game-path and/or nickname is not specified, "
            "the program will try to find them in the registry.",
            name);
}

bool parse_cmd_args(int argc, char* argv[], struct command_line_args* args) {
    // Not enough arguments or an odd number of arguments.
    if (argc < 5 || ((argc - 1) % 2) != 0) {
        show_usage(argv[0]);
        return false;
    }

    bool  port_setted = false;
    char* arg;
    for (int i = 1; i < argc; ++i) {
        arg = argv[i];
        if (!strcmp(arg, "--host") || !strcmp(arg, "-h")) {
            arg = argv[++i];
            if (strlen(arg) > MAX_HOST_LENGTH) {
                fprintf(stderr, "Invalid host address, length longer than %d\n", MAX_HOST_LENGTH);
                return false;
            }
            strcpy(args->host, arg);
        } else if (!strcmp(arg, "--port") || !strcmp(arg, "-p")) {
            arg = argv[++i];
            char* end;
            long  port = strtol(arg, &end, 0);
            if (end == arg || end[0] != '\0' || errno == ERANGE || port < 0 || port > UINT16_MAX) {
                fprintf(stderr, "Invalid port, must be in the range 0-%d\n", UINT16_MAX);
                return false;
            }
            args->port  = (uint16_t)port;
            port_setted = true;
        } else if (!strcmp(arg, "--nickname") || !strcmp(arg, "-n")) {
            arg = argv[++i];
            if (strlen(arg) > MAX_NICKNAME_LENGTH) {
                fprintf(stderr, "Invalid nickname, length longer than %d\n", MAX_NICKNAME_LENGTH);
                return false;
            }
            strcpy(args->nickname, arg);
        } else if (!strcmp(arg, "--password") || !strcmp(arg, "-z")) {
            arg = argv[++i];
            if (strlen(arg) > MAX_PASSWORD_LENGTH) {
                fprintf(stderr, "Invalid password, length longer than %d\n", MAX_PASSWORD_LENGTH);
                return false;
            }
            strcpy(args->password, arg);
        } else if (!strcmp(arg, "--game-path") || !strcmp(arg, "-g")) {
            arg = argv[++i];
            if (strlen(arg) > MAX_PATH) {
                fprintf(stderr, "Invalid gta_sa.exe path, length longer than %d\n", MAX_PATH);
                return false;
            }
            strcpy(args->game_path, arg);
        }
    }

    // These 2 arguments are always required.
    if (args->host[0] == '\0' || !port_setted) {
        show_usage(argv[0]);
        return false;
    }

    return true;
}

bool if_necessary_get_game_path_and_nickname_in_registry(char* game_path, char* nickname) {
    bool is_game_path_empty = game_path[0] == '\0';
    bool is_nickname_empty  = nickname[0] == '\0';

    if (!is_game_path_empty && !is_nickname_empty) {
        return true;
    }

    HKEY  key_handle;
    DWORD data_size;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\SAMP", 0, KEY_READ, &key_handle)
        != ERROR_SUCCESS) {
        print_last_error("RegOpenKeyEx");
        return false;
    }

    if (is_game_path_empty) {
        data_size = MAX_PATH;
        if (RegQueryValueEx(key_handle, "gta_sa_exe", NULL, NULL, (LPBYTE)game_path, &data_size)
            != ERROR_SUCCESS) {
            print_last_error("RegQueryValueEx(gta_sa_exe)");
            if (RegCloseKey(key_handle) != ERROR_SUCCESS) {
                print_last_error("RegCloseKey");
            }
            return false;
        }
    }

    if (is_nickname_empty) {
        data_size = MAX_NICKNAME_LENGTH;
        if (RegQueryValueEx(key_handle, "PlayerName", NULL, NULL, (LPBYTE)nickname, &data_size)
            != ERROR_SUCCESS) {
            print_last_error("RegQueryValueEx(PlayerName)");
            if (RegCloseKey(key_handle) != ERROR_SUCCESS) {
                print_last_error("RegCloseKey");
            }
            return false;
        }
    }

    if (RegCloseKey(key_handle) != ERROR_SUCCESS) {
        print_last_error("RegCloseKey");
    }

    return true;
}

bool launch_game(struct command_line_args* args) {
    // Max length of args.
    char cmd_line[18 + MAX_HOST_LENGTH + 5 + MAX_NICKNAME_LENGTH + MAX_PASSWORD_LENGTH + 1];
    make_game_cmd_line_args(args, cmd_line);

    char game_dir_path[MAX_PATH];
    get_dir_path(args->game_path, game_dir_path);

    char samp_dll_path[MAX_PATH];
    _makepath(samp_dll_path, NULL, game_dir_path, "samp.dll", NULL);

    return launch_process_and_inject_library(args->game_path, samp_dll_path, game_dir_path,
                                             cmd_line);
}

void make_game_cmd_line_args(struct command_line_args* args, char* cmd_line) {
    if (args->password[0] == '\0') {
        sprintf(cmd_line, "-c -h %s -p %d -n %s", args->host, args->port, args->nickname);
    } else {
        sprintf(cmd_line, "-c -h %s -p %d -n %s -z %s", args->host, args->port, args->nickname,
                args->password);
    }
}

bool launch_process_and_inject_library(const char* executable_path, const char* library_path,
                                       const char* current_dir, char* cmd_line) {
    PROCESS_INFORMATION process_info = {0};
    STARTUPINFO         startup_info = {0};

    if (!CreateProcess(executable_path, cmd_line, NULL, NULL, FALSE,
                       DETACHED_PROCESS | CREATE_SUSPENDED, NULL, current_dir, &startup_info,
                       &process_info)) {
        print_last_error("CreateProcess");
        return false;
    }

    if (!inject_library(process_info.hProcess, library_path)) {
        if (!TerminateProcess(process_info.hProcess, 0)) {
            print_last_error("TerminateProcess");
        }
        return false;
    }

    if (ResumeThread(process_info.hThread) == -1) {
        print_last_error("ResumeThread");
        if (!TerminateProcess(process_info.hProcess, 0)) {
            print_last_error("TerminateProcess");
        }
        return false;
    }

    if (!CloseHandle(process_info.hProcess)) {
        print_last_error("CloseHandle(hProcess)");
    }

    if (!CloseHandle(process_info.hThread)) {
        print_last_error("CloseHandle(hThread)");
    }

    return true;
}

bool inject_library(HANDLE process_handle, const char* library_path) {
    HMODULE kernel32 = GetModuleHandle("kernel32.dll");
    if (kernel32 == NULL) {
        print_last_error("GetModuleHandle(kernel32.dll)");
        return false;
    }

    FARPROC load_library_addr = GetProcAddress(kernel32, "LoadLibraryA");
    if (load_library_addr == NULL) {
        print_last_error("GetProcAddress(LoadLibraryA)");
        return false;
    }

    LPVOID proc_arg = VirtualAllocEx(process_handle, NULL, strlen(library_path),
                                     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (proc_arg == NULL) {
        print_last_error("VirtualAllocEx");
        return false;
    }

    if (!WriteProcessMemory(process_handle, proc_arg, library_path, strlen(library_path), NULL)) {
        print_last_error("WriteProcessMemory");
        return false;
    }

    HANDLE load_library_thread = CreateRemoteThread(
        process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)load_library_addr, proc_arg, 0, NULL);
    if (load_library_thread == NULL) {
        print_last_error("CreateRemoteThread");
        return false;
    }

    if (WaitForSingleObject(load_library_thread, INFINITE) == WAIT_FAILED) {
        print_last_error("WaitForSingleObject");
        return false;
    }

    if (!VirtualFreeEx(process_handle, proc_arg, 0, MEM_RELEASE)) {
        print_last_error("VirtualFreeEx");
    }

    return true;
}
