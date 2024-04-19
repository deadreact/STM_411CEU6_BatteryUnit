/*
 * program_data.cpp
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#include <shared_data.h>

SharedData& SharedData::get()
{
    static SharedData data;
    return data;
}

