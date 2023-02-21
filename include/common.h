#pragma once

#ifndef __COMMON_H__
#define __COMMON_H__

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#include <glog/logging.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "gflags/gflags.h"

#endif