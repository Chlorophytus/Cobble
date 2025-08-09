#if !defined(COBBLE_SERVER)
#define COBBLE_SERVER
#include "main.hpp"
#include "environment.hpp"
namespace cobble {
/// @brief Handles HTTP requests
namespace server {
void start(const environment::configuration &config);
} // namespace server
} // namespace cobble
#endif