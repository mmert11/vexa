#pragma once

#include "Engine/Engine.h"
#include "Solver/Solver.h"

#define TRY() \
try \
{ \

#define CATCH(error) \
    } catch (const std::exception& e) { \
        std::cerr << error << ": " << e.what() << std::endl; \
        std::cerr << "Program halted" << std::endl; \
		exit(-1); \
    } catch (...) { \
        std::cerr << error << ": Unknown error occurred." << std::endl; \
        std::cerr << "Program halted" << std::endl; \
		exit(-1); \
    }
