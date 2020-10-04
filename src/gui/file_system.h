#pragma once

#include "string_utils.h"

// Standard includes.
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

namespace gui 
{

//
// File system utilities.
//

inline std::string linux_open_file(
    const std::string               title,
    const std::vector<std::string>  file_filters = {});

//
// Implementation.
//

std::string linux_open_file(
    const std::string               title,
    const std::vector<std::string>  file_filters)
{
    // Thanks to https://stackoverflow.com/questions/18948783/c-simple-open-file-dialog-in-linux
    const char zenity[] = "/usr/bin/zenity";

    char command[2048];
    sprintf(command,"%s  --file-selection --modal --title=\"%s\" ", zenity, title.c_str());

    // Handle file filters.
    for(auto filter : file_filters)
    {
        sprintf(command,"%s  --file-filter=\"%s\"", command, filter.c_str());
    }

    FILE *f = popen(command,"r");
    char result[2048];
    fgets(result, 2048, f); 

    int ret = pclose(f);
    if (ret < 0) 
        perror("file_name_dialog()"); 
    
    std::string path = std::string(result);
    trim(path);
    return path;
}

} // namespace gui