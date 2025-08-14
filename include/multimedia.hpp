#if !defined(COBBLE_MULTIMEDIA)
#define COBBLE_MULTIMEDIA
#include "main.hpp"
#include "route.hpp"
namespace cobble {
/// @brief Audio/video (thumbnails and video streaming)
namespace multimedia {
/// @brief Handles HTTP GET of a thumbnail
/// @param config the server configuration
/// @param id the video ID
/// @return a response structure for routing
route::response_get thumbnail_get(const environment::configuration &config,
                                  U64 id);
/// @brief Handles HTTP HEAD of a thumbnail
/// @param config the server configuration
/// @param id the video ID
/// @return a response structure for routing
route::response_head thumbnail_head(const environment::configuration &config,
                                    U64 id);
/// @brief Handles HTTP GET of an actual video
/// @param config the server configuration
/// @param id the video ID
/// @return a response structure for routing
route::response_get video_get(const environment::configuration &config, U64 id);
/// @brief Handles HTTP HEAD of an actual video
/// @param config the server configuration
/// @param id the video ID
/// @return a response structure for routing
route::response_head video_head(const environment::configuration &config, U64 id);

} // namespace multimedia
} // namespace cobble
#endif