#ifndef _SWAY_TREE_SCENE_OUTPUT_H
#define _SWAY_TREE_SCENE_OUTPUT_H

#include <wlr/types/wlr_output.h>

void scene_output_pending_resolution(struct wlr_output *output,
		const struct wlr_output_state *state, int *width, int *height);

const struct wlr_output_image_description *scene_output_pending_image_description(
	struct wlr_output *output, const struct wlr_output_state *state);

void scene_output_state_get_buffer_src_box(const struct wlr_output_state *state,
		struct wlr_fbox *out);
 
#endif // _SWAY_TREE_SCENE_OUTPUT_H

