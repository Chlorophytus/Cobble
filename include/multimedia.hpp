#if !defined(COBBLE_MULTIMEDIA)
#define COBBLE_MULTIMEDIA
#include "main.hpp"
#include "route.hpp"
namespace cobble {
/// @brief Audio/video (thumbnails and video streaming)
namespace multimedia {
route::response_get thumbnail_get(const environment::configuration &config,
                                  U64 id);
route::response_head thumbnail_head(const environment::configuration &config,
                                    U64 id);

} // namespace multimedia
} // namespace cobble
#endif