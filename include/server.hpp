#if !defined(COBBLE_SERVER)
#define COBBLE_SERVER
#include "main.hpp"
#include "environment.hpp"
#include <atomic>
namespace cobble {
/// @brief Handles HTTP requests
namespace server {
/// @brief Starts the HTTP server
/// @param config The desired configuration
/// @param run If false, the server will terminate. 
void start(const environment::configuration &config, std::atomic<bool> &run);
} // namespace server
} // namespace cobble
#endif