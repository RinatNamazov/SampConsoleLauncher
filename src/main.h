/*****************************************************************************
 *
 *  PROJECT:        SampConsoleLauncher
 *  LICENSE:        See LICENSE in the top level directory
 *  FILE:           main.h
 *  DESCRIPTION:    Main
 *  COPYRIGHT:      (c) 2023 RINWARES <rinwares.com>
 *  AUTHOR:         Rinat Namazov <rinat.namazov@rinwares.com>
 *
 *****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <Windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HOST_LENGTH     256
#define MAX_NICKNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 256

struct command_line_args {
    char     host[MAX_HOST_LENGTH + 1];
    uint16_t port;
    char     nickname[MAX_NICKNAME_LENGTH + 1];
    char     password[MAX_PASSWORD_LENGTH + 1];
    char     game_path[MAX_PATH + 1];
};

void show_usage(const char* name);
bool parse_cmd_args(int argc, char* argv[], struct command_line_args* args);
bool if_necessary_get_game_path_and_nickname_in_registry(char* game_path, char* nickname);
bool launch_game(struct command_line_args* args);
void make_game_cmd_line_args(struct command_line_args* args, char* cmd_line);
bool launch_process_and_inject_library(const char* executable_path, const char* library_path,
                                       const char* current_dir, char* cmd_line);
bool inject_library(HANDLE process_handle, const char* library_path);

#endif
