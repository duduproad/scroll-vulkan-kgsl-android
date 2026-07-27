/*
 * This an unstable interface of wlroots. No guarantees are made regarding the
 * future consistency of this API.
 */
#ifndef WLR_USE_UNSTABLE
#error "Add -DWLR_USE_UNSTABLE to enable unstable wlroots features"
#endif

#ifndef _SWAY_EXT_IMAGE_CAPTURE_SOURCE_V1_H
#define _SWAY_EXT_IMAGE_CAPTURE_SOURCE_V1_H

#include <wlr/types/wlr_ext_image_capture_source_v1.h>
#include "sway/tree/scene.h"


struct wlr_ext_image_capture_source_v1 *sway_ext_image_capture_source_v1_create_with_scene_node(
	struct sway_scene_node *node, struct wl_event_loop *event_loop,
	struct wlr_allocator *allocator, struct wlr_renderer *renderer);

#endif // _SWAY_EXT_IMAGE_CAPTURE_SOURCE_V1_H
